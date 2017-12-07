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
#ifndef NodesXml_H
#define NodesXml_H
//=============================================================================
#include <QtCore>
#include <QDomDocument>
class Nodes;
class NodeItem;
class NodeField;
//=============================================================================
class NodesXml: public QObject
{
  Q_OBJECT

public:
  explicit NodesXml(Nodes *parent=0);

  void write(QDomNode dom, NodeItem *node) const;
  void write(QDomNode dom) const;
  QDomDocument write() const;

  int read(QDomNode dom) const;
  int read(QDomNode dom, NodeItem *node, int fmt) const;
  bool read(QDomNode dom, NodeField *field) const;

  int import(QDomNode dom) const;

private:
  Nodes *nodes;
  int format;
};
//=============================================================================
#endif
