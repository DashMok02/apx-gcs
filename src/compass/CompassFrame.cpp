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
#include <QtCore>
#include "CompassFrame.h"
//==============================================================================
CompassFrame::CompassFrame(QWidget *parent) :
  QWidget(parent)
{
  if (this->objectName().isEmpty())
    this->setObjectName("compassFrame");
  this->setWindowTitle(tr("Compass Calibration"));
  vbLayout = new QVBoxLayout(this);
  hbLayoutBottom = new QHBoxLayout();

  buttonClear.setText(tr("Clear"));
  checkBoxTrace.setText(tr("Trace"));

  toolBar=new QToolBar(this);
  toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  toolBar->setIconSize(QSize(16,16));
  toolBar->layout()->setMargin(0);
  vbLayout->addWidget(toolBar);
  buttonClose.setText(tr("Close"));
  buttonClose.setObjectName("killButton");
  toolBar->addWidget(&buttonClose);
  toolBar->addSeparator();
  toolBar->addWidget(&buttonClear);
  toolBar->addSeparator();
  toolBar->addWidget(&checkBoxTrace);

  vbLayout->addWidget(&lbInfo);
  vbLayout->addLayout(hbLayoutBottom);
  //vbLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum));
  /*QWidget* empty = new QWidget();
  empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
  toolBar->addWidget(empty);*/


  for (int i = 0; i < 3; i++)
  {
    dArea[i] = new DrawingArea();
    //dArea[i]->resizeArea(this->size().width() / 3, this->size().width() / 3);
    hbLayoutBottom->insertWidget(i, dArea[i]);
  }
  dArea[0]->sAxis[0] = "X";
  dArea[0]->sAxis[1] = "Y";
  dArea[1]->sAxis[0] = "X";
  dArea[1]->sAxis[1] = "Z";
  dArea[2]->sAxis[0] = "Z";
  dArea[2]->sAxis[1] = "Y";

  connect(&checkBoxTrace, SIGNAL(stateChanged(int)), this,
          SLOT(oncheckBoxTraceChange(int)));
  connect(&buttonClear, SIGNAL(clicked()), this, SLOT(onbuttonClearPressed()));
  connect(&buttonClose, SIGNAL(clicked()), this, SLOT(close()));

  checkBoxTrace.setChecked(true);

  connect(QMandala::instance(),SIGNAL(currentChanged(QMandalaItem*)),this,SLOT(mandalaCurrentChanged(QMandalaItem*)));
  mandalaCurrentChanged(QMandala::instance()->current);
}
void CompassFrame::closeEvent(QCloseEvent *event)
{
  Q_UNUSED(event)
  disconnect(this);
  QSettings().setValue(objectName(),saveGeometry());
  emit closed();
}
//=============================================================================
void CompassFrame::mandalaCurrentChanged(QMandalaItem *m)
{
  onbuttonClearPressed();
  foreach(QMetaObject::Connection c,mcon) disconnect(c);
  mcon.clear();
  mcon.append(connect(m, SIGNAL(updated(uint)), SLOT(dataReceived(uint)), Qt::QueuedConnection));
}
//=============================================================================
void CompassFrame::dataReceived(uint var_idx)
{
  if (var_idx != idx_downstream)
    return;
  dArea[0]->addData(QMandala::instance()->current->mag[0], QMandala::instance()->current->mag[1]);
  dArea[1]->addData(QMandala::instance()->current->mag[0], QMandala::instance()->current->mag[2]);
  dArea[2]->addData(QMandala::instance()->current->mag[2], QMandala::instance()->current->mag[1]);
  lbInfo.setText(
        QString("bias {%1, %2, %3}\tscale {%4, %5, %6}")
        .arg(dArea[0]->bX,0,'f',2)
      .arg(dArea[0]->bY,0,'f',2)
      .arg(dArea[1]->bY,0,'f',2)
      .arg(dArea[0]->sX,0,'f',2)
      .arg(dArea[0]->sY,0,'f',2)
      .arg(dArea[1]->sY,0,'f',2)
      );
}
//==============================================================================
void CompassFrame::action_toggled(bool checked)
{
  if (!checked)
    return;
}
//==============================================================================
void CompassFrame::oncheckBoxTraceChange(int val)
{
  if (val)
  {
    for (int i = 0; i < 3; i++)
    {
      dArea[i]->startTrace();
    }
  }
  else
  {
    for (int i = 0; i < 3; i++)
    {
      dArea[i]->stopTrace();
    }
  }
}
//==============================================================================
void CompassFrame::onbuttonClearPressed()
{
  for (int i = 0; i < 3; i++)
  {
    dArea[i]->clearTrace();
  }

}
