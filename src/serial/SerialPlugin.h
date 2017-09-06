/*
 * Copyright (C) 2013 Kapacheuski Yury <ky@uavos.com>
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
#ifndef SerialPLUGIN_H
#define SerialPLUGIN_H

#include <QtCore>
#include "plugin_interface.h"
#include "SerialForm.h"
//=============================================================================
class SerialPlugin: public PluginInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "com.uavos.gcu.PluginInterface/1.0")
  Q_INTERFACES(PluginInterface)
public:
  void init(void);
private:
    QAction *aShow;
    SerialForm *fobj;
private slots:
    void showDlg(void);
    void dlgClosed();
};
//=============================================================================
#endif // SerialPLUGIN_H
