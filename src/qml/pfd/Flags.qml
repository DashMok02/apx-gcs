import QtQuick 2.2
import "../components"

Column {
    property double txtHeight
    spacing: 4
    //anchors.fill: parent
    Flag {
        id: flaps
        show: ctr_flaps.value > 0
        height: txtHeight
        flagColor: "cyan"
        text: qsTr("FLAPS")
        toolTip: ctr_flaps.descr
        //control: ctr_flaps
        Text {
            visible: mandala.test || flaps.show
            color: "white"
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.right
            anchors.leftMargin: 2
            text: (ctr_flaps.value*100).toFixed()
            font.pixelSize: height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.family: font_narrow
        }
    }
    Flag {
        id: brakes
        show: ctr_brake.value > 0
        height: txtHeight
        text: qsTr("BRAKE")
        toolTip: ctr_brake.descr
        //control: ctr_brake
        Text {
            visible: mandala.test || (brakes.show && (ctr_brake.value>0) && (ctr_brake.value<1))
            color: "white"
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.right
            anchors.leftMargin: 2
            text: (ctr_brake.value*100).toFixed()
            font.pixelSize: height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.family: font_narrow
        }
    }
    Row {
        spacing: 1
        Flag {
            id: ers_flag
            show: ctrb_ers.value > 0
            visible: opacity
            height: txtHeight
            flagColor: "red"
            text: qsTr("ERS")
            toolTip: ctrb_ers.descr
            //control: ctrb_ers
        }
        Flag {
            show: ctrb_rel.value > 0
            //anchors.left: ers_flag.show?ers_flag.right:ers_flag.left
            //anchors.top: ers_flag.top
            height: txtHeight
            flagColor: "yellow"
            text: qsTr("REL")
            toolTip: ctrb_rel.descr
            //control: ctrb_rel
        }
    }
    Flag {
        show: power_ignition.value <= 0 && mandala.dlcnt>0
        height: txtHeight
        flagColor: "red"
        text: qsTr("IGN")
        toolTip: power_ignition.descr
        //control: power_ignition
    }
    Flag {
        show: power_payload.value > 0
        height: txtHeight
        text: qsTr("PYLD")
        toolTip: power_payload.descr
        //control: power_payload
    }
    Flag {
        show: sw_taxi.value > 0
        height: txtHeight
        text: qsTr("TAXI")
        toolTip: sw_taxi.descr
        //control: sw_taxi
    }
    /*Flag {
        show: sw_lights.value > 0
        height: txtHeight
        text: qsTr("LGHT")
        toolTip: cmode_throvr.descr
    }*/
}

