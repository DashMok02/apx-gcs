﻿import QtQuick 2.3
import QtQuick.Layouts 1.3

import Apx.Common 1.0

Rectangle {
    border.width: 0
    color: "#000"
    implicitWidth: itemWidth
    Layout.margins: 1

    readonly property int maxItems: 14
    readonly property real aspectRatio: 3.8

    readonly property real itemWidth: height/maxItems*aspectRatio
    readonly property real itemHeight: itemWidth/aspectRatio

    property bool isLanding:
        m.mode.value===mode_LANDING ||
        m.mode.value===mode_TAKEOFF ||
        m.mode.value===mode_TAXI ||
        (m.mode.value===mode_WPT && m.mtype.value===mtype_line)


    ColumnLayout {
        width: itemWidth
        spacing: 1

        ValueRss {
            implicitWidth: itemWidth
            implicitHeight: itemHeight
        }

        ValueDL {
            implicitWidth: itemWidth
            implicitHeight: itemHeight
        }

        /*FactValue {
            title: qsTr("EXT")
            value: apx.datalink.hosts.availableCount
        }
        FactValue {
            title: qsTr("CTR")
            //value
        }*/

        Item {
            Layout.fillWidth: true
            implicitHeight: width/aspectRatio/2
        }

        FactValue {
            title: qsTr("FL")
            fact: m.fuel
            value: fact.value.toFixed(1)
            implicitWidth: itemWidth
            implicitHeight: itemHeight
        }
        FactValue {
            title: qsTr("DH")
            fact: m.dHome
            property double v: (m.mode.value===mode_TAXI)?m.delta.value:m.dHome.value
            value: v>=1000?(v/1000).toFixed(1):v.toFixed()
            implicitWidth: itemWidth
            implicitHeight: itemHeight
        }
        FactValue {
            title: qsTr("DME")
            fact: m.dWPT
            property double v: m.dWPT.value
            value: v>=1000?(v/1000).toFixed(1):v.toFixed()
            implicitWidth: itemWidth
            implicitHeight: itemHeight
        }
        FactValue {
            title: qsTr("ETA")
            fact: m.ETA
            property int v: fact.value
            property double valid: v>0
            property int tsec: ("0"+Math.floor(v%60)).slice(-2)
            property int tmin: ("0"+Math.floor(v/60)%60).slice(-2)
            property int thrs: Math.floor(v/60/60)
            property string sETA: (thrs?thrs+":":"")+("0"+tmin).slice(-2)+":"+("0"+tsec).slice(-2)
            value: valid?sETA:"--:--"
            implicitWidth: itemWidth
            implicitHeight: itemHeight
        }

        Item {
            Layout.fillWidth: true
            implicitHeight: itemHeight/2
        }

        FactValue {
            title: qsTr("WPT")
            fact: m.wpidx
            visible: ui.test || (m.mode.value===mode_WPT || m.mode.value===mode_STBY)
            implicitWidth: itemWidth
            implicitHeight: itemHeight
        }
        FactValue {
            title: qsTr("LPS")
            fact: m.loops
            visible: ui.test || (m.mode.value===mode_STBY && m.loops.value>0)
            implicitWidth: itemWidth
            implicitHeight: itemHeight
        }
        FactValue {
            title: qsTr("RD")
            fact: m.rwDelta
            visible: ui.test || isLanding
            property double v: fact.value
            value: (Math.abs(v)<1?0:v.toFixed())+(m.rwAdj.value>0?"+"+m.rwAdj.value.toFixed():m.rwAdj.value<0?"-"+(-m.rwAdj.value).toFixed():"")
            implicitWidth: itemWidth
            implicitHeight: itemHeight
        }
        FactValue {
            title: qsTr("LR")
            fact: m.turnR
            visible: ui.test || m.mode.value===mode_STBY || m.mode.value===mode_LANDING
            property double v: fact.value
            value: v>=1000?(v/1000).toFixed(1):v.toFixed()
            implicitWidth: itemWidth
            implicitHeight: itemHeight
        }
        FactValue {
            title: qsTr("AGL")
            fact: m.agl
            value: fact.value.toFixed(1)
            visible: ui.test || (m.status_agl.value>0)
            implicitWidth: itemWidth
            implicitHeight: itemHeight
        }
    }
}
