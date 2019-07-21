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
#include "FactBase.h"
//=============================================================================
FactBase::FactBase(FactBase *parent, const QString &name, FactBase::Flags flags)
    : QObject(parent)
    , m_treeType(Flag(int(flags) & TypeMask))
    , m_options(flags & OptsMask)
    , m_parentFact(nullptr)
    , m_name(makeNameUnique(name))
    , m_size(0)
    , m_num(0)
{
    setObjectName(m_name);
    if (m_options & Section)
        m_treeType = Group;
}
FactBase::~FactBase()
{
    setParentFact(nullptr);

    /*for (int i = 0; i < m_children.size(); ++i) {
        FactBase *f = child(i);
        if (!f)
            continue;
        disconnect(this, nullptr, f, nullptr);
        //f->setParentFact(nullptr);
        f->deleteLater();
    }
    m_children.clear();*/
}
//=============================================================================
void FactBase::addChild(FactBase *item)
{
    item->setParent(this);
    emit itemToBeInserted(m_children.size(), item);
    m_children.append(item);
    updateChildrenNums();
    updateSize();
    emit itemInserted(item);
}
void FactBase::removeChild(FactBase *item)
{
    int i = m_children.indexOf(item);
    if (i < 0)
        return;
    emit itemToBeRemoved(i, item);
    m_children.removeAt(i);
    updateChildrenNums();
    updateSize();
    emit itemRemoved(item);
}
void FactBase::removeAll()
{
    if (m_children.size() <= 0)
        return;
    for (int i = 0; i < m_children.size(); i++) {
        FactBase *item = child(i);
        disconnect(this, nullptr, item, nullptr);
        item->removeAll();
    }
    //qDebug()<<"removeAll"<<this;
    while (m_children.size() > 0) {
        FactBase *item = child(m_children.size() - 1);
        disconnect(this, nullptr, item, nullptr);
        emit itemToBeRemoved(m_children.size() - 1, item);
        m_children.takeLast();
        m_size = m_children.size();
        item->removed();
        emit itemRemoved(item);
        item->deleteLater();
        //delete item;
    }
    emit sizeChanged();
}
void FactBase::moveChild(FactBase *item, int n, bool safeMode)
{
    int i = m_children.indexOf(item);
    if (i < 0 || i == n)
        return;
    if (safeMode) {
        emit itemToBeRemoved(i, item);
        m_children.removeAt(i);
        emit itemRemoved(item);
        emit itemToBeInserted(n, item);
        m_children.insert(n, item);
        emit itemInserted(item);
        updateChildrenNums();
    } else {
        if (n > i)
            n++;
        emit itemToBeMoved(i, n, item);
        m_children.removeAt(i);
        m_children.insert(n, item);
        emit itemMoved(item);
    }
    for (int i = 0; i < m_children.size(); ++i)
        child(i)->updateNum();
}
//=============================================================================
void FactBase::remove()
{
    setParentFact(nullptr);
    emit removed();
    deleteLater();
}
void FactBase::move(int n, bool safeMode)
{
    FactBase *p = parentFact();
    if (p)
        p->moveChild(this, n, safeMode);
}
//=============================================================================
QString FactBase::makeNameUnique(const QString &s)
{
    QString sr = s.simplified()
                     .replace(' ', '_')
                     .replace('.', '_')
                     .replace(':', '_')
                     .replace('/', '_')
                     .replace('\\', '_')
                     .replace('?', '_')
                     .replace('-', '_')
                     .replace('+', '_');
    if (!parentFact())
        return sr;
    int i = 0;
    nameSuffix = QString();
    QString suffix;
    while (1) {
        FactBase *dup = nullptr;
        for (int i = 0; i < parentFact()->m_children.size(); ++i) {
            FactBase *item = parentFact()->child(i);
            if (item == this)
                continue;
            if (item->name() == (sr + suffix)) {
                dup = item;
                break;
            }
        }
        if (!dup)
            break;
        suffix = QString("_%1").arg(++i, 3, 10, QChar('0'));
    }
    nameSuffix = suffix;
    return sr;
}
//=============================================================================
int FactBase::num() const
{
    return m_num;
}
//=============================================================================
FactBase *FactBase::child(int n) const
{
    return qobject_cast<FactBase *>(m_children.value(n, nullptr));
}
//=============================================================================
int FactBase::indexOfChild(FactBase *item) const
{
    return m_children.indexOf(item);
}
int FactBase::indexInParent() const
{
    return parentFact() ? parentFact()->indexOfChild(const_cast<FactBase *>(this)) : -1;
}
//=============================================================================
FactBase *FactBase::child(const QString &name) const
{
    for (int i = 0; i < m_children.size(); ++i) {
        FactBase *item = child(i);
        if (item && item->name() == name)
            return item;
    }
    return nullptr;
}
//=============================================================================
QString FactBase::path(const QChar pathDelimiter) const
{
    QString s;
    for (const FactBase *i = this; i; i = i->parentFact()) {
        if (s.isEmpty())
            s = i->name();
        else
            s.prepend(i->name() + pathDelimiter);
        if (i->treeType() == Root)
            break;
    }
    return s.isEmpty() ? name() : s;
}
QList<FactBase *> FactBase::pathList() const
{
    QList<FactBase *> list;
    for (const FactBase *i = this; i;) {
        list.append(const_cast<FactBase *>(i));
        if (i->treeType() == Root)
            break;
        i = i->parentFact();
    }
    return list;
}
//=============================================================================
bool FactBase::hasParent(const FactBase *parent) const
{
    for (const FactBase *i = parentFact(); i; i = i->parentFact()) {
        if (i == parent)
            return true;
    }
    return false;
}
bool FactBase::hasChild(const FactBase *child) const
{
    return children().contains(const_cast<FactBase *>(child));
}
//=============================================================================
void FactBase::updateNum()
{
    int v = indexInParent();
    if (v < 0)
        return;
    if (m_num == v)
        return;
    m_num = v;
    emit numChanged();
}
void FactBase::updateSize()
{
    int v = m_children.size();
    if (m_size == v)
        return;
    m_size = v;
    emit sizeChanged();
}
void FactBase::updateChildrenNums()
{
    for (int i = 0; i < m_children.size(); ++i)
        child(i)->updateNum();
}
//=============================================================================
FactBase::Flag FactBase::treeType(void) const
{
    return m_treeType;
}
void FactBase::setTreeType(FactBase::Flag v)
{
    v = static_cast<Flag>(v & TypeMask);
    if (m_treeType == v)
        return;
    m_treeType = v;
    emit treeTypeChanged();
}
FactBase::Flags FactBase::options(void) const
{
    return m_options;
}
void FactBase::setOptions(FactBase::Flags v)
{
    v &= OptsMask;
    if (m_options == v)
        return;
    m_options = v;
    emit optionsChanged();
}
void FactBase::setOption(FactBase::Flag opt, bool v)
{
    if (v)
        setOptions(options() | opt);
    else
        setOptions(options() & (~opt));
}
int FactBase::size(void) const
{
    return m_size;
}
QString FactBase::name(void) const
{
    return (m_name.contains('#') ? QString(m_name).replace('#', QString::number(num() + 1)) : m_name)
           + nameSuffix;
}
void FactBase::setName(const QString &v)
{
    QString s = makeNameUnique(v);
    if (m_name == s && nameSuffix.isEmpty())
        return;
    //emit itemRemoved(this);
    m_name = s;
    setObjectName(name());
    //emit itemAdded(this);
    emit nameChanged();
}
FactBase *FactBase::parentFact() const
{
    return m_parentFact;
}
void FactBase::setParentFact(FactBase *v)
{
    FactBase *prevParent = parentFact();
    if (prevParent == v)
        return;
    if (prevParent) {
        prevParent->removeChild(this);
    }

    setParent(v);
    m_parentFact = v;
    emit parentFactChanged();
    if (v)
        v->addChild(this);
}
//=============================================================================