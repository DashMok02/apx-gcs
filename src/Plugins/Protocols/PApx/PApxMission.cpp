/*
 * APX Autopilot project <http://docs.uavos.com>
 *
 * Copyright (c) 2003-2020, Aliaksei Stratsilatau <sa@uavos.com>
 * All rights reserved
 *
 * This file is part of APX Ground Control.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "PApxMission.h"

#include "PApxNode.h"
#include "PApxNodeFile.h"
#include "PApxNodes.h"
#include "PApxVehicle.h"

#include <App/AppLog.h>

#include <xbus/XbusMission.h>

PApxMission::PApxMission(PApxVehicle *parent)
    : PMission(parent)
    , _vehicle(parent)
{}

void PApxMission::requestMission()
{
    for (auto node : static_cast<PApxNodes *>(_vehicle->nodes())->nodes()) {
        auto f = node->file("mission");
        if (!f)
            continue;
        connect(f, &PApxNodeFile::downloaded, this, &PApxMission::parseMissionData);
        new PApxNodeRequestFileRead(node, "mission");
        return;
    }
    apxMsgW() << tr("Mission source unavailable");
}

void PApxMission::parseMissionData(const xbus::node::file::info_s &info, const QByteArray data)
{
    //qDebug() << "mission data" << info.size << data.size();
    PStreamReader stream(data);
    QJsonValue json = unpack(stream);

    emit missionReceived(json);
}
QJsonValue PApxMission::unpack(PStreamReader &stream)
{
    //unpack mission
    if (stream.available() < xbus::mission::file_hdr_s::psize())
        return QJsonValue();

    xbus::mission::file_hdr_s fhdr{};
    fhdr.read(&stream);
    QString title(QByteArray(fhdr.title, sizeof(fhdr.title)));

    qDebug() << title << stream.size() << "bytes";

    if (fhdr.size != stream.available()) {
        qWarning() << "data size" << fhdr.size << stream.available();
        return QJsonValue();
    }

    QJsonArray wp, rw, tw, pi;

    int ecnt = 0, wpcnt = 0, rwcnt = 0;

    while (stream.available() > 0) {
        ecnt++;
        xbus::mission::hdr_s hdr;
        hdr.read(&stream);

        switch (hdr.type) {
        case xbus::mission::STOP:
            stream.reset(stream.size()); //finish
            ecnt--;
            continue;
        case xbus::mission::WP: {
            if (stream.available() < xbus::mission::wp_s::psize())
                break;
            xbus::mission::wp_s e;
            e.read(&stream);
            wpcnt++;

            QJsonObject item;
            item.insert("lat", static_cast<qreal>(e.lat));
            item.insert("lon", static_cast<qreal>(e.lon));
            item.insert("altitude", e.alt);
            QString type = "direct";
            if (hdr.option == 1)
                type = "path";
            item.insert("type", type);

            wp.append(item);
            continue;
        }
        case xbus::mission::ACT: { //wp actions
            QJsonObject a;
            switch (hdr.option) {
            default:
                break;
            case xbus::mission::ACT_SPEED: {
                xbus::mission::act_speed_s e;
                e.read(&stream);
                a.insert("speed", e.speed);
                break;
            }
            case xbus::mission::ACT_PI: {
                xbus::mission::act_pi_s e;
                e.read(&stream);
                a.insert("poi", e.index + 1);
                break;
            }
            case xbus::mission::ACT_SCR: {
                xbus::mission::act_scr_s e;
                e.read(&stream);
                a.insert("script", QString(QByteArray(e.scr, sizeof(e.scr))));
                break;
            }
            case xbus::mission::ACT_SHOT: {
                xbus::mission::act_shot_s e;
                e.read(&stream);
                switch (e.opt) {
                case 0: //single
                    a.insert("shot", "single");
                    a.insert("dshot", 0);
                    break;
                case 1: //start
                    a.insert("shot", "start");
                    a.insert("dshot", e.dist);
                    break;
                case 2: //stop
                    a.insert("shot", "stop");
                    a.insert("dshot", 0);
                    break;
                }
                break;
            }
            }
            if (wp.isEmpty()) {
                qWarning() << "Orphan actions in mission";
                continue;
            }
            QJsonObject wpt = wp.last().toObject();
            if (!wpt.contains("actions"))
                wpt.insert("actions", QJsonObject());
            QJsonObject actions = wpt["actions"].toObject();
            for (auto i : a.keys())
                actions.insert(i, a[i]);
            wpt.insert("actions", actions);
            QJsonValueRef ref = wp[wp.size() - 1];
            ref = wpt;
            continue;
        }
        case xbus::mission::RW: {
            if (stream.available() < xbus::mission::rw_s::psize())
                break;
            xbus::mission::rw_s e;
            e.read(&stream);
            rwcnt++;

            QJsonObject item;
            item.insert("lat", static_cast<qreal>(e.lat));
            item.insert("lon", static_cast<qreal>(e.lon));
            item.insert("hmsl", e.hmsl);
            item.insert("dN", e.dN);
            item.insert("dE", e.dE);
            item.insert("approach", e.approach);
            QString type = "left";
            if (hdr.option == 1)
                type = "right";
            item.insert("type", type);

            rw.append(item);
            continue;
        }
        case xbus::mission::TW: {
            if (stream.available() < xbus::mission::tw_s::psize())
                break;
            xbus::mission::tw_s e;
            e.read(&stream);

            QJsonObject item;
            item.insert("lat", static_cast<qreal>(e.lat));
            item.insert("lon", static_cast<qreal>(e.lon));

            tw.append(item);
            continue;
        }
        case xbus::mission::PI: {
            if (stream.available() < xbus::mission::pi_s::psize())
                break;
            xbus::mission::pi_s e;
            e.read(&stream);
            QJsonObject item;
            item.insert("lat", static_cast<qreal>(e.lat));
            item.insert("lon", static_cast<qreal>(e.lon));
            item.insert("hmsl", e.hmsl);
            item.insert("radius", e.radius);
            item.insert("loops", e.loops);
            item.insert("timeout", e.timeout);

            pi.append(item);
            continue;
        }
        case xbus::mission::EMG:
        case xbus::mission::DIS: {
            uint16_t sz = stream.available();
            size_t pointsCnt = hdr.option;
            if (sz < xbus::mission::area_s::psize(pointsCnt))
                break;
            //TODO - implement areas in GCS
            for (size_t i = 0; i < pointsCnt; ++i) {
                xbus::mission::area_s p;
                p.read(&stream);
            }
            continue;
        }
        }
        //error in mission
        return QJsonValue();
    }

    QJsonObject json;
    json.insert("title", title);

    if (!wp.isEmpty())
        json.insert("wp", wp);
    if (!rw.isEmpty())
        json.insert("rw", rw);
    if (!tw.isEmpty())
        json.insert("tw", tw);
    if (!pi.isEmpty())
        json.insert("pi", pi);
    return json;
}
