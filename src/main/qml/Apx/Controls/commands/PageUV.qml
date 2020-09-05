﻿import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import QtQml.Models 2.12

import Apx.Common 1.0

RowLayout {

    readonly property var f_mode: mandala.cmd.proc.mode
    readonly property var f_stage: mandala.cmd.proc.stage
    readonly property var f_action: mandala.cmd.proc.action
    readonly property var f_adj: mandala.cmd.proc.adj

    readonly property var f_flaps: mandala.ctr.wing.flaps
    readonly property var f_brake: mandala.ctr.str.brake
    readonly property var f_thr: mandala.cmd.rc.thr

    readonly property bool m_reg_str: mandala.cmd.reg.str.value

    //spacing: buttonHeight/4
    ColumnLayout {
        Layout.fillHeight: true
        Layout.alignment: Qt.AlignTop|Qt.AlignLeft
        spacing: 3
        CtrNum { title: "THR"; fact: f_thr; min: 0; max: 100; mult: 100; stepSize: 1; }
        CtrNum { title: "FLP"; fact: f_flaps; min: 0; max: 100; mult: 100; stepSize: 10; }
        CtrNum { title: "ADJ"; fact: f_adj; }
    }
    CtrFlow {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.alignment: Qt.AlignTop
        key: f_mode.text
        controls: {
            "TAXI": [btnCANCEL,btnATAXI,btnBRK_TAXI],
            "TAKEOFF": [btnCANCEL,btnNEXT,btnBRK],
            "LANDING": [btnCANCEL,btnNEXT,btnBRK],
            "WPT": [btnINC,btnDEC],
            "STBY": [btnRESET],
            "UAV": [btnNEXT,btnCANCEL],
        }
    }


    Component {
        id: btnFLP
        CtrButton {
            title: "FLAPS"
            fact: f_flaps
            onTriggered: fact.value=highlighted?0:1
            highlighted: fact.value>0
        }
    }
    Component {
        id: btnBRK_TAXI
        CtrButton {
            title: "BRAKE"
            fact: f_brake
            readonly property real v: fact.value
            color: (v>0 && v<1)?Qt.darker(Material.color(Material.Orange),1.5):undefined
            width: height*3
            highlighted: v>0
            onTriggered: {
                if(m_reg_str){
                    f_brake.value=1
                }else{
                    f_brake.value=v>0?0:1
                }
            }
        }
    }
    Component {
        id: btnBRK
        CtrButton {
            title: "BRAKE"
            fact: f_brake
            width: height*3
            onTriggered: fact.value=highlighted?0:1
            highlighted: fact.value>0
        }
    }
    Component {
        id: btnATAXI
        CtrButton {
            fact: f_action
            title: m_reg_str?"STOP":"AUTO"
            width: height*4
            highlighted: m_reg_str
            onTriggered: fact.value=m_reg_str?proc_action_reset:proc_action_next
        }
    }
    Component {
        id: btnNEXT
        CtrButton {
            fact: f_action
            title: "NEXT"
            width: height*4
            onTriggered: fact.value=proc_action_next
        }
    }
    Component {
        id: btnINC
        CtrButton {
            fact: f_action
            title: "NEXT"
            width: height*4
            onTriggered: fact.value=proc_action_inc
        }
    }
    Component {
        id: btnDEC
        CtrButton {
            fact: f_action
            title: "PREV"
            width: height*4
            onTriggered: fact.value=proc_action_dec
        }
    }
    Component {
        id: btnCANCEL
        CtrButton {
            fact: f_action
            title: "CANCEL"
            width: height*4
            onTriggered: fact.value=proc_action_reset
        }
    }
    Component {
        id: btnRESET
        CtrButton {
            fact: f_action
            title: "RESET"
            width: height*4
            onTriggered: fact.value=proc_action_reset
        }
    }
}
