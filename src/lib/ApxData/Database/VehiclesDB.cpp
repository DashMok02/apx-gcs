﻿/*
 * Copyright (C) 2011 Aliaksei Stratsilatau <sa@uavos.com>
 *
 * This file is part of the UAV Open System Project
 *  http://www.uavos.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "VehiclesDB.h"
#include "Database.h"
#include <App/AppDirs.h>

VehiclesDB::VehiclesDB(QObject *parent, QString sessionName)
    : DatabaseSession(parent, "vehicles", sessionName, "v2")
{
    new DBReqMakeTable(this,
                       "Nodes",
                       QStringList() << "key INTEGER PRIMARY KEY NOT NULL"
                                     << "sn TEXT NOT NULL UNIQUE"
                                     << "time INTEGER"  //time seen
                                     << "name TEXT"     //latest
                                     << "version TEXT"  //latest
                                     << "hardware TEXT" //latest
    );
    new DBReqMakeIndex(this, "Nodes", "sn", true);
    new DBReqMakeIndex(this, "Nodes", "time", false);

    new DBReqMakeTable(this,
                       "NodeUsers",
                       QStringList()
                           << "key INTEGER PRIMARY KEY NOT NULL"
                           << "nodeID INTEGER NOT NULL"
                           << "time INTEGER DEFAULT 0" //seen and synced
                           << "firstTime TEXT"         //first time seen
                           << "machineUID TEXT"
                           << "hostname TEXT"
                           << "username TEXT"
                           << "FOREIGN KEY(nodeID) REFERENCES Nodes(key) ON DELETE CASCADE");
    new DBReqMakeIndex(this, "NodeUsers", "nodeID", true);
    new DBReqMakeIndex(this, "NodeUsers", "time", false);

    //Node dictionary
    new DBReqMakeTable(this,
                       "NodeDicts",
                       QStringList()
                           << "key INTEGER PRIMARY KEY NOT NULL"
                           << "nodeID INTEGER NOT NULL"
                           << "time INTEGER DEFAULT 0" //seen and synced
                           << "hash TEXT"              //generated by mcu
                           << "name TEXT"
                           << "version TEXT"
                           << "hardware TEXT"
                           << "FOREIGN KEY(nodeID) REFERENCES Nodes(key) ON DELETE CASCADE");
    new DBReqMakeIndex(this, "NodeDicts", "nodeID", false);
    new DBReqMakeIndex(this, "NodeDicts", "time", false);
    new DBReqMakeIndex(this, "NodeDicts", "hash", false);

    new DBReqMakeTable(this,
                       "NodeDictDataFields",
                       QStringList() << "key INTEGER PRIMARY KEY NOT NULL"
                                     << "name TEXT NOT NULL"
                                     << "title TEXT"
                                     << "units TEXT"
                                     << "type TEXT NOT NULL"
                                     << "array INTEGER");

    new DBReqMakeTable(this,
                       "NodeDictData",
                       QStringList()
                           << "key INTEGER PRIMARY KEY NOT NULL"
                           << "dictID INTEGER NOT NULL"
                           << "fieldID INTEGER NOT NULL"
                           << "fidx INTEGER"
                           << "gidx INTEGER"
                           << "FOREIGN KEY(dictID) REFERENCES NodeDicts(key) ON DELETE CASCADE"
                           << "FOREIGN KEY(fieldID) REFERENCES NodeDictDataFields(key)");
    new DBReqMakeIndex(this, "NodeDictData", "dictID", false);
    new DBReqMakeIndex(this, "NodeDictData", "fieldID", false);
    new DBReqMakeIndex(this, "NodeDictData", "fidx", false);
    new DBReqMakeIndex(this, "NodeDictData", "gidx", false);

    //Node configs
    new DBReqMakeTable(this,
                       "NodeConfigs",
                       QStringList()
                           << "key INTEGER PRIMARY KEY NOT NULL"
                           << "nodeID INTEGER NOT NULL"
                           << "hash TEXT"    //sha1 from records
                           << "time INTEGER" //creation time
                           << "dictID INTEGER NOT NULL"
                           << "title TEXT" //auto generated
                           << "FOREIGN KEY(nodeID) REFERENCES Nodes(key) ON DELETE CASCADE"
                           << "FOREIGN KEY(dictID) REFERENCES NodeDicts(key) ON DELETE CASCADE");
    new DBReqMakeIndex(this, "NodeConfigs", "nodeID", false);
    new DBReqMakeIndex(this, "NodeConfigs", "hash", true);
    new DBReqMakeIndex(this, "NodeConfigs", "time", false);

    new DBReqMakeTable(this,
                       "NodeConfigHistory",
                       QStringList()
                           << "key INTEGER PRIMARY KEY NOT NULL"
                           << "nconfID INTEGER NOT NULL"
                           << "time INTEGER" //creation time
                           << "FOREIGN KEY(nconfID) REFERENCES NodeConfigs(key) ON DELETE CASCADE");
    new DBReqMakeIndex(this, "NodeConfigHistory", "nconfID", false);
    new DBReqMakeIndex(this, "NodeConfigHistory", "time", false);

    new DBReqMakeTable(this,
                       "NodeConfigDataValues",
                       QStringList() << "key INTEGER PRIMARY KEY NOT NULL"
                                     << "value TEXT");
    new DBReqMakeIndex(this, "NodeConfigDataValues", "value", true);

    new DBReqMakeTable(this,
                       "NodeConfigData",
                       QStringList()
                           << "nconfID INTEGER NOT NULL"
                           << "fieldID INTEGER NOT NULL"
                           << "subidx INTEGER"
                           << "valueID INTEGER NOT NULL"
                           << "FOREIGN KEY(nconfID) REFERENCES NodeConfigs(key) ON DELETE CASCADE"
                           << "FOREIGN KEY(fieldID) REFERENCES NodeDictData(key) ON DELETE CASCADE"
                           << "FOREIGN KEY(valueID) REFERENCES NodeConfigDataValues(key) ON DELETE "
                              "CASCADE");
    new DBReqMakeIndex(this, "NodeConfigData", "nconfID", false);
    new DBReqMakeIndex(this, "NodeConfigData", "nconfID,fieldID,subidx", true);

    //Vehicles
    new DBReqMakeTable(this,
                       "Vehicles",
                       QStringList() << "key INTEGER PRIMARY KEY NOT NULL"
                                     << "uid TEXT"
                                     << "callsign TEXT"
                                     << "class TEXT"
                                     << "time INTEGER DEFAULT 0" //time seen
                                     << "squawk TEXT");
    new DBReqMakeIndex(this, "Vehicles", "uid,callsign,class", true);
    new DBReqMakeIndex(this, "Vehicles", "uid", false);
    new DBReqMakeIndex(this, "Vehicles", "callsign", false);
    new DBReqMakeIndex(this, "Vehicles", "class", false);
    new DBReqMakeIndex(this, "Vehicles", "time", false);

    //vehicle nodes configs (commits)
    new DBReqMakeTable(this,
                       "VehicleConfigs",
                       QStringList()
                           << "key INTEGER PRIMARY KEY NOT NULL"
                           << "hash TEXT NOT NULL UNIQUE" //sha1
                           << "time INTEGER DEFAULT 0"    //time of snapshot
                           << "title TEXT"                //auto generated
                           << "vehicleID INTEGER NOT NULL"
                           << "notes TEXT"
                           << "FOREIGN KEY(vehicleID) REFERENCES Vehicles(key) ON DELETE CASCADE");
    new DBReqMakeIndex(this, "VehicleConfigs", "hash", true);
    new DBReqMakeIndex(this, "VehicleConfigs", "time", false);
    new DBReqMakeIndex(this, "VehicleConfigs", "title", false);
    new DBReqMakeIndex(this, "VehicleConfigs", "vehicleID", false);
    new DBReqMakeIndex(this, "VehicleConfigs", "notes", false);

    new DBReqMakeTable(
        this,
        "VehicleConfigData",
        QStringList() << "configID INTEGER NOT NULL"
                      << "nconfID INTEGER NOT NULL"
                      << "FOREIGN KEY(configID) REFERENCES VehicleConfigs(key) ON DELETE CASCADE"
                      << "FOREIGN KEY(nconfID) REFERENCES NodeConfigs(key) ON DELETE CASCADE");
    new DBReqMakeIndex(this, "VehicleConfigData", "configID", false);
}

DBReqVehicles::DBReqVehicles(QString sn)
    : DatabaseRequest(Database::instance()->vehicles)
    , sn(sn)
    , nodeID(0)
{}
bool DBReqVehicles::run(QSqlQuery &query)
{
    query.prepare("SELECT * FROM Nodes WHERE sn = ?");
    query.addBindValue(sn);
    if (!query.exec())
        return false;
    if (query.next()) {
        nodeID = query.value(0).toULongLong();
    }
    return true;
}
QHash<QString, quint64> DBReqVehicles::getFieldsByName(QSqlQuery &query, quint64 dictID) const
{
    QHash<QString, quint64> map;
    query.prepare("SELECT * FROM NodeDictData "
                  "INNER JOIN NodeDictDataFields ON NodeDictData.fieldID=NodeDictDataFields.key "
                  "WHERE dictID=? AND type!='command' AND type!='group'");
    query.addBindValue(dictID);
    if (!query.exec())
        return map;
    while (query.next()) {
        QString s = query.value("name").toString();
        if (map.contains(s)) {
            qWarning() << "duplicate field name";
            map.clear();
            break;
        }
        map.insert(s, query.value(0).toULongLong());
    }
    if (map.isEmpty()) {
        qWarning() << "missing fields in dictionary" << dictID;
    }
    return map;
}