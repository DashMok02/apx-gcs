﻿import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.2

import APX.Facts 1.0

import Apx.Common 1.0
import Apx.Menu 1.0

Popup {
    id: popup

    property point pos: Qt.point(parent.width/2,parent.height/2)
    property bool pinned: false

    property real implicitOpacity: ui.smooth?0.92:1
    property real inactiveOpacity: 0.65

    //forward props
    property alias fact: factMenu.fact
    property bool menuEnabled: true

    function showFact(f)
    {
        Menu.raisePopup(popup)
        factMenu.showFact(f)
    }

    opacity: menuEnabled?implicitOpacity:inactiveOpacity

    onAboutToHide: {
        Menu.unregisterMenuView(factMenu)
        Menu.unregisterMenuPopup(popup)
    }
    onOpened: {
        Menu.registerMenuPopup(popup)
    }
    onClosed: {
        if(popup && popup.destroy)popup.destroy()
    }
    onFactChanged: if(!fact)factMenu.back()

    x: pos.x - width/2
    y: pos.y

    padding: 0
    margins: 0

    enter: Transition { NumberAnimation { property: "opacity"; from: 0.0; to: implicitOpacity } }

    closePolicy: pinned?Popup.NoAutoClose:(Popup.CloseOnEscape|Popup.CloseOnPressOutside)

    contentItem: FactMenu {
        id: factMenu
        priority: popup.z
        autoResize: true

        onFactOpened: {
            Menu.raisePopup(popup)
        }

        onFactButtonTriggered: {
            if(fact.options & Fact.CloseOnTrigger)
                popup.close()
            else Menu.raisePopup(popup)
        }
        Connections {
            target: fact
            onProgressChanged: popup.pinned=true
        }
        onStackEmpty: popup.close()
        titleRightMargin: btnClose.width
        CleanButton {
            id: btnClose
            z: 10
            anchors.right: factMenu.right
            anchors.top: factMenu.top
            anchors.margins: 5
            iconName: "close"
            color: popup.pinned?Material.BlueGrey:undefined
            height: MenuStyle.titleSize*0.8
            width: height
            onClicked: popup.close()
        }
    }

    //draggable window
    MouseArea {
        id: mouseArea
        z: popup.menuEnabled?0:(contentItem.z+100)
        anchors.fill: parent
        propagateComposedEvents: popup.menuEnabled
        property point clickPos: Qt.point(0,0)
        onPressed: {
            clickPos = Qt.point(mouse.x,mouse.y)
            Menu.raisePopup(popup)
        }
        onPositionChanged: {
            var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
            popup.x+=delta.x
            popup.y+=delta.y
            popup.pinned=true
        }
        onDoubleClicked: popup.pinned=true
        onPressAndHold: popup.pinned=true
    }
}
