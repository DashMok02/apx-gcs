/*
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
#ifndef ValueEditorScript_H
#define ValueEditorScript_H
#include "ValueEditor.h"
#include "ui_ValueEditorScript.h"
#include "PawnScript.h"
//=============================================================================
class ValueEditorScript: public ValueEditor, public Ui::ValueEditorScript
{
  Q_OBJECT
public:
  explicit ValueEditorScript(NodesItemField *field, QWidget *parent = 0);
  ~ValueEditorScript();
protected:
  QWidget * getWidget(QWidget *parent);
  bool aboutToUpload(void);
  bool aboutToClose(void);
private:
  QString scrName;
  bool saveToFile(QString fname);
  bool loadFromFile(QString fname);

  PawnScript *pawn;

  //data
private slots:
  void aSave_triggered(void);
  void aLoad_triggered(void);
  void aUndo_triggered(void);

  //compiler
  void logView_itemClicked(QListWidgetItem *item);

  bool compile();
};
//=============================================================================
#endif //ValueEditorScript_H