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
#ifndef FactData_H
#define FactData_H
//=============================================================================
#include "FactTree.h"
//=============================================================================
class FactData: public FactTree
{
  Q_OBJECT
  Q_ENUMS(DataType)
  Q_ENUMS(ActionType)

  Q_PROPERTY(DataType dataType READ dataType CONSTANT)

  Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

  Q_PROPERTY(QString name READ name NOTIFY nameChanged)

  Q_PROPERTY(QString title READ title NOTIFY titleChanged)
  Q_PROPERTY(QString descr READ descr NOTIFY descrChanged)

  Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:

  enum DataType {
    NoData =0,      // no editor
    TextData,
    FloatData,
    IntData,
    BoolData,
    EnumData,       // value=index of child item(set by name or index)
    ItemIndexData,  // value=index in parent item container
    ActionData,     // button, value=action type
  };

  enum ActionType {
    NormalAction =0,
    RemoveAction,
    UplinkAction,
  };

  explicit FactData(FactTree *parent, QString name, QString title, QString descr, ItemType treeItemType, DataType dataType);


  Q_INVOKABLE FactData * child(const QString &name) const;
  Q_INVOKABLE FactData * valueEnumItem() const;

  Q_INVOKABLE void copyValuesFrom(const FactData *item);
  Q_INVOKABLE void bindValue(FactData *item);



  //LIST MODEL
  enum FactModelRoles {
    ModelDataRole = Qt::UserRole + 1,
    NameRole,
    ValueRole,
    TextRole,
  };
  enum { //model columns
    FACT_ITEM_COLUMN_NAME=0,
    FACT_ITEM_COLUMN_VALUE,
    FACT_ITEM_COLUMN_DESCR,
  };

protected:
  //ListModel override
  virtual QHash<int, QByteArray> roleNames() const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

private:
  FactData *_bindedFact;
private slots:
  void updateBindedValue();
  void bindedValueChanged();

public:
  //---------------------------------------
  virtual DataType dataType() const;

  virtual QVariant value(void) const;
  virtual bool setValue(const QVariant &v);

  virtual QString name(void) const;
  virtual QString title(void) const;
  virtual QString descr(void) const;

  virtual QString text() const;
  virtual void setText(const QString &v);

protected:
  DataType m_dataType;

  QVariant m_value;

  QString  m_name;
  QString  m_title;
  QString  m_descr;

signals:
  void valueChanged();

  void nameChanged();
  void titleChanged();
  void descrChanged();

  void textChanged();
};
//=============================================================================
#endif
