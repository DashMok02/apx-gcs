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
#pragma once

#include "MandalaFactStream.h"

#include <Mandala/MandalaMetaBase.h>
#include <Protocols/ProtocolStream.h>

#include <Fact/Fact.h>

class Mandala;

class MandalaFact : public Fact, public MandalaFactStream
{
    Q_OBJECT

public:
    explicit MandalaFact(Mandala *tree, Fact *parent, const mandala::meta_s &meta);

    // send value to uplink when set
    bool setValue(const QVariant &v) override;

    // set value locally and do not send uplink
    bool setValueLocal(const QVariant &v);

    // set values array and send uplink batch update (f.ex. vecors)
    bool setValues(const QVariantList &vlist);

    // raw value size
    size_t psize() const;

    Q_INVOKABLE mandala::uid_t uid() const;
    Q_INVOKABLE void request();
    Q_INVOKABLE void send();

    Q_INVOKABLE Fact *classFact() const;
    Q_INVOKABLE QString mpath() const;

    void addAlias(const QString &a);
    Q_INVOKABLE QString alias() const;
    Q_INVOKABLE mandala::uid_t offset() const;

    void unpack(const xbus::pid_s &pid, const mandala::spec_s &spec, ProtocolStreamReader &stream);
    template<typename _T>
    bool unpack(ProtocolStreamReader &stream)
    {
        if (stream.available() != sizeof(_T))
            return false;
        setValueLocal(QVariant::fromValue(stream.read<_T>()));
        return true;
    }

private:
    Mandala *m_tree;
    const mandala::meta_s &m_meta;
    QString m_alias;
    size_t m_psize{0};

    uint8_t txbuf[32];
    ProtocolStreamWriter ostream{txbuf, sizeof(txbuf)};

    QElapsedTimer sendTime;
    QTimer sendTimer;

    int getPrecision();
    QColor getColor();

protected:
    //Fact override
    virtual QVariant data(int col, int role) const override;
    virtual bool showThis(QRegExp re) const override; //filter helper

    //stream
    void setValueFromStream(const QVariant &v) override;
    QVariant getValueForStream() const override;

signals:
    void sendUplink(QByteArray data); //forwarded to tree

    //---------------------------------------
    // PROPERTIES
public:
    bool isSystem() const;
};
