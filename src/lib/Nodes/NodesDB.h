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
#ifndef NodesDB_H
#define NodesDB_H
//=============================================================================
#include <QtCore>
#include <QtSql>
class Nodes;
class NodeItem;
class NodeField;
//=============================================================================
class NodesDB: public QObject
{
  Q_OBJECT

public:
  explicit NodesDB(Nodes *parent=0);

  void createTables();

  void nodeInfoWrite(NodeItem *node);
  void nodeInfoRead(NodeItem *node);

  void nodeDictWrite(NodeItem *node);
  void nodeDictClear(NodeItem *node);
  void nodeDictRead(NodeItem *node);

  void nodeDataWrite(NodeItem *node);
  typedef QList<QPair<QString,quint64>> NodeDataKeys;
  NodeDataKeys nodeDataReadKeys(NodeItem *node,int limit=25);
  void nodeDataRead(NodeItem *node, quint64 dataID);


private:
  Nodes *nodes;
  bool m_enabled;

  bool checkResult(QSqlQuery &query);

  quint64 nodeGetID(NodeItem *node, QSqlQuery *query, bool *ok);

};
//=============================================================================
#endif

