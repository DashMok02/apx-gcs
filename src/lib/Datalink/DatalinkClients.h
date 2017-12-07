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
#ifndef DatalinkClients_H
#define DatalinkClients_H
//=============================================================================
#include <QtCore>
#include "FactSystem.h"
class Datalink;
class DatalinkClient;
//=============================================================================
class DatalinkClients: public Fact
{
  Q_OBJECT
public:
  explicit DatalinkClients(Datalink *parent);

  Fact *f_alloff;
  Fact *f_list;

  Datalink *f_datalink;

  QTcpServer *server;
private:
  uint retryBind;

private slots:
  void updateStats();
  void serverActiveChanged();
  void tryBindServer();

  //tcp server
  void newConnection();

public slots:
  void forward(DatalinkClient *src, const QByteArray &ba);
signals:
  void bindError();
};
//=============================================================================
#endif
