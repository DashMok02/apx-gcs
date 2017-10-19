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
#include "Fact.h"
//=============================================================================
Fact::Fact(FactTree *parent, QString name, QString title, QString descr, ItemType treeItemType, DataType dataType)
 : FactData(parent,name,title,descr,treeItemType,dataType),
   m_enabled(true), m_visible(true)
{
}
//=============================================================================
QString Fact::path(int fromLevel, const QChar pathDelimiter) const
{
  QString s;
  for(const Fact *i=this;i && i->level()>=fromLevel;i=static_cast<Fact*>(i->parentItem())){
    if(s.isEmpty())s=i->name();
    else s.prepend(i->name()+pathDelimiter);
    if(i->treeItemType()==RootItem && fromLevel>=0)break;
  }
  return s.isEmpty()?name():s;
}
//=============================================================================
Fact * Fact::fact(const QString &factNamePath) const
{
  foreach(FactTree *item,m_items){
    Fact *f=static_cast<Fact*>(item);
    if(f->treeItemType()==FactItem && (f->name()==factNamePath || f->path().endsWith(factNamePath)))
      return f;
  }
  foreach(FactTree *item,m_items){
    Fact *f=static_cast<Fact*>(item);
    f=f->fact(factNamePath);
    if(f)return f;
  }
  return NULL;
}
//=============================================================================
/*Fact * Fact::byPath(const QString &itemNamePath) const
{
  QString s=itemNamePath;
  Fact *item=const_cast<Fact*>(this);
  while(!s.isEmpty()){
    if(item->isFactsList()){
      item=item->childByName(s);
      break;
    }else{
      int i=s.indexOf(pathDelimiter);
      if(i<=0)break;
      item=item->childByName(s.left(i));
      s.remove(0,i+1);
    }
  }
  return item;
}*/
//=============================================================================
void Fact::trigger(void)
{
  //qDebug()<<"trigger"<<name();
  emit triggered();
}
//=============================================================================
bool Fact::enabled() const
{
  return m_enabled;
}
void Fact::setEnabled(const bool &v)
{
  if(m_enabled==v)return;
  m_enabled=v;
  emit enabledChanged();
}
bool Fact::visible() const
{
  return m_visible;
}
void Fact::setVisible(const bool &v)
{
  if(m_visible==v)return;
  m_visible=v;
  emit visibleChanged();
}
QString Fact::section() const
{
  return m_section;
}
void Fact::setSection(const QString &v)
{
  if(m_section==v)return;
  m_section=v;
  emit sectionChanged();
}
//=============================================================================
//=============================================================================
