﻿import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.2

import Apx.Common 1.0

CleanButton {
    id: control
    property var fact
    property int size: buttonHeight

    ui_scale: 1

    title: fact.title
    toolTip: fact.descr

    defaultHeight: size
}
