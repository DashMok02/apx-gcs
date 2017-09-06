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
#include "TelemetryPlot.h"
#include <QtGui>
#include <QScriptEngine>
#include "QMandala.h"
//=============================================================================
TelemetryPlot::TelemetryPlot(QWidget *parent)
  :QwtPlot(parent),isCopy(false)
{
  lbFileName.setParent(this);

  setAutoFillBackground(true);
  setAutoReplot(false);
  setCanvasBackground(QColor(Qt::black));
  setStyleSheet("font: 8pt 'Arial'; background-color: rgb(0, 0, 0); gridline-color: rgb(255, 255, 255); color: rgb(255, 255, 255);");

  // legend
  legend = new PlotLegend;
  legend->setFrameStyle(QFrame::NoFrame);//|QFrame::Plain);
  legend->setDefaultItemMode(QwtLegendData::Checkable);
  insertLegend(legend, QwtPlot::RightLegend);
  connect(legend, SIGNAL(checked(QVariant,bool,int)), this, SLOT(showCurve(QVariant,bool,int)));

  // grid
  grid = new QwtPlotGrid;
  grid->setMajorPen(QPen(QColor(80,80,80), 0, Qt::DotLine));
  grid->attach(this);

  //move pan
  panner = new QwtPlotPanner(canvas());
  panner->setMouseButton(Qt::MidButton);
  panner2 = new QwtPlotPanner(canvas());
  panner2->setMouseButton(Qt::LeftButton,Qt::ShiftModifier);

  //zoom
  zoomer=new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft,canvas());
  zoomer->setMousePattern(QwtEventPattern::MouseSelect1,Qt::LeftButton);
  zoomer->setRubberBand(QwtPicker::RectRubberBand);
  zoomer->setRubberBandPen(QColor(Qt::green));
  zoomer->setTrackerMode(QwtPicker::ActiveOnly);
  zoomer->setTrackerPen(QColor(Qt::white));

  mag=new PlotMagnifier(canvas());
  magX=new PlotMagnifier(canvas());
  magX->setAxisEnabled(QwtPlot::yLeft,false);
  magX->setWheelModifiers(Qt::ShiftModifier);
  magY=new PlotMagnifier(canvas());
  magY->setAxisEnabled(QwtPlot::xBottom,false);
  magY->setWheelModifiers(Qt::ControlModifier);

  picker=new PlotPicker(canvas());
  picker->setStateMachine(new QwtPickerDragPointMachine());
  picker->setTrackerPen(QPen(Qt::white));
  picker->setRubberBandPen(QPen(Qt::white));
  picker->setMousePattern(QwtEventPattern::MouseSelect1,Qt::LeftButton,Qt::ControlModifier);

  timeTracker=new QwtPlotMarker();
  timeTracker->setLinePen(QPen(Qt::darkMagenta));
  timeTracker->setLineStyle(QwtPlotMarker::VLine);
  timeTracker->attach(this);

  canvas()->setCursor(Qt::ArrowCursor);

  foreach(QMandalaField *f,QMandala::instance()->local->fields){
    //fill params
    uint type=f->type();
    uint varmsk=f->varmsk();
    QString sn=f->name();
    uint ci=0;
    if(type==vt_vect || type==vt_point) ci=(varmsk>>8)+1;
    QColor c(Qt::cyan);
    if(sn.contains("ctr_"))c=Qt::magenta;
    else if(sn.contains("ctrb_"))c=Qt::magenta;
    else if(type==vt_flag)c=QColor(Qt::blue).lighter();
    else if(ci==1)c=Qt::red;
    else if(ci==2)c=Qt::green;
    else if(ci==3)c=Qt::yellow;
    //if(sv.size()>=2)c=QColor(sv.at(1));
    //if(sv.size()>=3)divider_dsp=sv.at(2).toDouble();
    Qt::PenStyle style = Qt::SolidLine;
    if(sn.contains("cmd_"))style=Qt::DotLine;
    else if(sn.contains("gps_"))style=Qt::DashLine;
    else if(sn.contains("rc_"))style=Qt::DotLine;
    registerField(sn,f->descr(),QPen(c, 0, style));
  }

  //QtScript calculated
  registerField("calculated",tr("Calculated user variable"),QColor(Qt::yellow).lighter());

  //restore visible
  if(!QSettings().contains("plots"))QSettings().setValue("plots",QStringList()<<"pitch"<<"cmd_airspeed"<<"cmd_roll"<<"cmd_pitch"<<"altitude"<<"cmd_altitude"<<"roll"<<"airspeed");
  QStringList sps=QSettings().value("plots").toStringList();
  foreach (const QString &name,fields.uniqueKeys())
    showCurve(itemToInfo(fields.value(name).curve), (name=="calculated") ? false : sps.contains(name));
}
//=============================================================================
TelemetryPlot::~TelemetryPlot()
{
  //clear();
  delete zoomer;
  delete legend;
  delete grid;
  delete panner;
  delete panner2;
  delete mag;
  delete magX;
  delete magY;
  delete picker;
  delete timeTracker;
}
//=============================================================================
void TelemetryPlot::showCurves(bool on, const QStringList &names, bool toggle)
{
  if(toggle&&on){
    bool bAllOn=true;
    foreach (const QString &name,fields.uniqueKeys())
      if((!names.size())||names.contains(name)){
        bAllOn&=fields.value(name).curve->isVisible();
      }
    if(bAllOn)on=false;
  }
  foreach (const QString &name,fields.uniqueKeys())
    if((!names.size())||names.contains(name)){
      showCurve(itemToInfo(fields.value(name).curve),on);
    }
  resetZoom();
}
//=============================================================================
void TelemetryPlot::timeTrack(uint time_ms)
{
  timeTracker->setXValue(time_ms/1000.0);
  timeTracker->setVisible(time_ms!=0);
  replot();
}
//=============================================================================
void TelemetryPlot::showCurve(const QVariant &itemInfo, bool on, int index)
{
  Q_UNUSED(index);
  QwtPlotItem *item = infoToItem(itemInfo);
  bool allWereHidden=true;
  foreach(const _telemetry_field &f,fields){
    if(f.curve->isVisible()){
      allWereHidden=false;
      break;
    }
  }
  if(on && item->title().text()=="calculated")
    refreshCalculated();
  item->setVisible(on);
  QWidget *w = legend->legendWidget(itemInfo);
  if (w && w->inherits("QwtLegendLabel"))
    ((QwtLegendLabel *)w)->setChecked(on);
  if(allWereHidden && !isCopy) resetZoom();
  else replot();

  //save state
  if(isCopy)
    return;
  QStringList sps;
  foreach (const QString &name,fields.uniqueKeys())
    if(name!="calculated" && fields.value(name).curve->isVisible())
      sps.append(name);
  QSettings().setValue("plots",sps);
}
//=============================================================================
//=============================================================================
void TelemetryPlot::load(int idx)
{
  QProgressBar progressBar(this);
  initProgressBar(&progressBar);
  updateGeometry();
  lbFileName.setText("LOADING...");
  QCoreApplication::flush();
  QCoreApplication::processEvents();
  QCoreApplication::flush();
  QMandala::instance()->local->rec->loadFlight(idx,&progressBar);
  lbFileName.setText(QFileInfo(QMandala::instance()->local->rec->loadFile.fileName()).baseName());
  lbFileName.setToolTip(QMandala::instance()->local->rec->loadFile.fileName());
  lbFileName.adjustSize();
  QSettings().setValue("telemetryFile",QFileInfo(QMandala::instance()->local->rec->loadFile.fileName()).fileName());
  setAutoReplot(false);

  showCurve(itemToInfo(fields["calculated"].curve), false);
  //clear data
  data_x.clear();
  foreach(const QString &name,fields.uniqueKeys())
    fields[name].fdata.clear();

  initProgressBar(&progressBar);

  //fill internal data
  uint cnt=0,ti=0;
  foreach(uint time,QMandala::instance()->local->rec->file.time){
    if(((cnt++)&0x00FF)==0){
      QCoreApplication::flush();
      QCoreApplication::processEvents();
      QCoreApplication::flush();
      progressBar.setValue(progressBar.maximum()*0.8+time*0.2);
    }
    QCoreApplication::processEvents();
    data_x.append(time/1000.0); //time
    if(QMandala::instance()->local->rec->file.data.size()<=(int)ti)break;
    const FlightDataFile::ListDouble &vlist=QMandala::instance()->local->rec->file.data.at(ti++);
    for(int i=0;i<QMandala::instance()->local->fields.size();i++){
      const QString &name=QMandala::instance()->local->fields.at(i)->name();
      if(fields.contains(name))
        fields[name].fdata.append(vlist.at(i));
    }
  }

  //install data
  foreach(const QString &name,fields.uniqueKeys()){
    QCoreApplication::flush();
    QCoreApplication::processEvents();
    QCoreApplication::flush();
    _telemetry_field &f=fields[name];
    QVector<QPointF> d;
    for(int ix=0;ix<data_x.size();ix++)
      d.append(QPointF(data_x.at(ix),ix<f.fdata.size()?f.fdata.at(ix):0));
    f.curve->setData(new QwtPointSeriesData(d));
  }
  resetZoom();
}
//=============================================================================
void TelemetryPlot::refreshCalculated(void)
{
  bool ok;
  if(expCalc.isEmpty())expCalc="altitude+down";
  QString exp=QInputDialog::getText(this, tr("Calculated field"),
                                    tr("QtScript expression:"), QLineEdit::Normal,
                                    expCalc, &ok);
  if(!ok)return;
  expCalc=exp;

  QProgressBar progressBar(this);
  initProgressBar(&progressBar);
  //fill internal data
  _telemetry_field &fcalc=fields["calculated"];
  fcalc.fdata.clear();
  QScriptEngine engine;

  uint cnt=0,i=0;
  foreach(uint time,QMandala::instance()->local->rec->file.time){
    if(((cnt++)&0x00FF)==0){
      QCoreApplication::processEvents();
      progressBar.setValue(progressBar.maximum()*0.8+time*0.2);
    }

    engine.globalObject().setProperty("time",time/1000.0,QScriptValue::ReadOnly);
    const FlightDataFile::ListDouble &vlist=QMandala::instance()->local->rec->file.data.at(i++);
    for(int i=0;i<QMandala::instance()->local->fields.size();i++)
      engine.globalObject().setProperty(QMandala::instance()->local->fields.at(i)->name(),vlist.at(i),QScriptValue::ReadOnly);
    fcalc.fdata.append(engine.evaluate(expCalc).toNumber());
  }
  //install data
  QVector<QPointF> d;
  for(int ix=0;ix<data_x.size();ix++)
    d.append(QPointF(data_x.at(ix),ix<fcalc.fdata.size()?fcalc.fdata.at(ix):0));
  fcalc.curve->setData(new QwtPointSeriesData(d));
}
//=============================================================================
void TelemetryPlot::initProgressBar(QProgressBar *progressBar)
{
  progressBar->setStyleSheet("QProgressBar {text-align: left; border: 1px solid lightGray;padding: 0px;background: black;height: 5px;} QProgressBar::chunk {background: gray;border: 0px;}");
  progressBar->setGeometry(0,12,width()/2,10);
  progressBar->setVisible(true);
  progressBar->setMaximum(QMandala::instance()->local->rec->file.time.size()?QMandala::instance()->local->rec->file.time.last():0);
}
//=============================================================================
//=============================================================================
void TelemetryPlot::resetZoom()
{
  //time scale
  double timeMax=0;
  foreach(double v,data_x)
    if (timeMax<v)timeMax=v;
  //min-max
  double vmax=0.0,vmin=0.0;
  foreach (const _telemetry_field &f,fields.values())
    foreach(double v,f.fdata)
      if(f.curve->isVisible()&&(!isnan(v))&&(!isinf(v))){
        if (vmax<v)vmax=v;
        if (vmin>v)vmin=v;
      }
  //reset zoom
  QRectF r(0,vmin,timeMax,vmax-vmin);
  zoomer->zoom(r);
  zoomer->setZoomBase(r);
  //zoomer->zoom(r);
  //zoomer->setZoomBase(r);
  //QCoreApplication::processEvents();
  //replot();
}
//=============================================================================
void TelemetryPlot::copyFromPlot(TelemetryPlot *plot)
{
  isCopy=true;
  //install data
  foreach(const QString &name,fields.uniqueKeys()){
    fields[name].curve->setData(new QwtPointSeriesData(((QwtPointSeriesData*)plot->fields[name].curve->data())->samples()));
    showCurve(itemToInfo(fields[name].curve),plot->fields[name].curve->isVisible());
  }
  //same zoom
  setAxisScaleDiv(QwtPlot::yLeft, plot->axisScaleDiv(QwtPlot::yLeft));
  setAxisScaleDiv(QwtPlot::xBottom, plot->axisScaleDiv(QwtPlot::xBottom));
  zoomer->setZoomBase(plot->zoomer->zoomBase());

  lbFileName.setText(plot->lbFileName.text()+tr(" (view copy)"));
  lbFileName.setToolTip(plot->lbFileName.toolTip());
  lbFileName.adjustSize();

  replot();
}
//=============================================================================
//=============================================================================
void TelemetryPlot::registerField(const QString &varName,const QString &dsc,const QPen &pen)
{
  _telemetry_field f;
  f.curve=new QwtPlotCurve();
  f.curve->setYAxis(QwtPlot::yLeft);
  f.curve->setTitle(varName);
  f.curve->setPen(pen);
  f.curve->attach(this);
  f.curve->setLegendAttribute(QwtPlotCurve::LegendShowLine);
  f.curve->setRenderHint(QwtPlotItem::RenderAntialiased);
  fields.insert(varName,f);
  QString s="<html><NOBR>";
  s+="<div style='background-color: black;font-family: monospace; font-weight: bold;'><font size=+1>"+varName+"</font></div>";
  if(dsc.size())s+="<div style='background-color: black;'>"+dsc+"</div>";
  QWidget *w = legend->legendWidget(itemToInfo(f.curve));
  if(w)
    w->setToolTip(s);
}
//=============================================================================
QwtText PlotPicker::trackerText(const QPoint &pos)const
{
  double t=plot()->invTransform(QwtPlot::xBottom,pos.x());
  QString s="<html><NOBR><table>";
  s+="<tr><td colspan=2 align=left style='font-family: monospace; font-weight: bold;'><PRE><font size=+4>";
  s+=QTime(0,0).addSecs(t).toString("hh:mm:ss");//+QString::number(t)+" sec)";
  s+="</font></PRE></td></tr>";
  foreach(const QString &name,((TelemetryPlot*)plot())->fields.uniqueKeys()){
    const _telemetry_field &f=((TelemetryPlot*)plot())->fields.value(name);
    if(!f.curve->isVisible())continue;
    double v=0;
    for(size_t i=0;i<f.curve->data()->size();i++)
      if(f.curve->data()->sample(i).x()>=t){
        v=f.curve->data()->sample(i).y();
        break;
      }
    QColor c=f.curve->pen().color();
    if(f.curve->pen().style()!=Qt::SolidLine)
      c=c.darker();
    s+="<tr><td align=right><font size=+2 color="+c.name()+">"+f.curve->title().text()+"</font>&nbsp;&nbsp;</td><td align=left><font size=+2>"+QString().sprintf("%.2f",v)+" "+QMandala::instance()->local->field(name)->units()+"</font></td></tr>";
  }
  s+="</table>";
  return QwtText(s);
}
//=============================================================================
void LegendItem::paintEvent(QPaintEvent *e)
{
  const QRect cr=contentsRect();
  QPainter painter( this );
  painter.setClipRegion(e->region());
  if (isChecked()){
    painter.save();
    painter.setBrush(QColor(50,50,50));
    painter.setPen(QPen(QColor(100,100,100),1,Qt::DotLine));
    painter.drawRoundedRect(cr.adjusted(0,0,-1,-1),2,2);
    painter.restore();
  }
  painter.save();
  painter.setClipRect( cr );
  drawContents(&painter);
  QRect identRect = cr;
  identRect.setX( identRect.x() + margin() );
  identRect.setSize( icon().size() );
  identRect.moveCenter( QPoint( identRect.center().x(), cr.center().y() ) );
  painter.drawPixmap( identRect, icon() );
  painter.restore();
}
//=============================================================================
void PlotMagnifier::widgetMouseMoveEvent(QMouseEvent *mouseEvent)
{
  mwPos=mouseEvent->pos();
  QwtPlotMagnifier::widgetMouseMoveEvent(mouseEvent);
}
void PlotMagnifier::rescale( double factor )
{
  factor = qAbs( factor );
  if ( factor == 1.0 || factor == 0.0 )
    return;
  bool doReplot = false;
  QwtPlot* plt = plot();
  const bool autoReplot = plt->autoReplot();
  plt->setAutoReplot( false );
  for ( int axisId = 0; axisId < QwtPlot::axisCnt; axisId++ ){
    const QwtScaleDiv &scaleDiv = plt->axisScaleDiv( axisId );
    if ( isAxisEnabled( axisId )){
      // get the range of the axis
      double min_value = scaleDiv.lowerBound();
      double max_value = scaleDiv.upperBound();
      // convert to screen coordinates
      QwtScaleMap mapper = plt->canvasMap(axisId);
      double min_pixel = mapper.transform(min_value);
      double max_pixel = mapper.transform(max_value);
      // get the mouse position on this axis
      double center;
      if ( axisId == QwtPlot::yLeft || axisId == QwtPlot::yRight )
        center = mwPos.y();
      else
        center = mwPos.x();
      // use the mouse as the center when we can,
      // if its -1, then its a keyboard event. use the window center.
      if (center == -1)
        center = min_pixel + (max_pixel-min_pixel)/2;
      // convert back to real values
      min_value = mapper.invTransform( center - (center - min_pixel)*factor );
      max_value = mapper.invTransform( center + (max_pixel - center)*factor );
      // set the new range
      plt->setAxisScale(axisId, min_value, max_value);
      //plt->setAxisScale( axisId, center - width_2, center + width_2 );
      doReplot = true;
    }
  }
  plt->setAutoReplot( autoReplot );
  if ( doReplot ) plt->replot();
}
//=============================================================================
PlotLegend::PlotLegend(QWidget *parent) :
  QwtLegend(parent)
{
}
//=============================================================================
QWidget *PlotLegend::createWidget(const QwtLegendData &data) const
{
  Q_UNUSED(data);
  QwtLegendLabel *w = new LegendItem();
  w->setItemMode(defaultItemMode());
  w->setSpacing(3);
  w->setMargin(0);
  connect(w, SIGNAL(clicked()), this, SLOT(itemClicked()));
  connect(w, SIGNAL(checked(bool)), this, SLOT(itemChecked(bool)));
  return w;
}
//=============================================================================
