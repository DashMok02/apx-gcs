TEMPLATE = lib
TARGET = gcs

!mac {
TARGET = $$qtLibraryTarget($$TARGET)
}

include( ../../common.pri )

DESTDIR = $$DESTDIR_LIB

QT += network
QT += serialport
QT += quick quickwidgets quickcontrols2 svg multimedia opengl
QT += sql

target.path = $$INSTALLBASE_LIB/lib

RESOURCES += $$RES_DIR/fonts.qrc
RESOURCES += $$RES_DIR/styles.qrc
RESOURCES += $$RES_DIR/material-icons.qrc
RESOURCES += $$RES_DIR/standard-icons.qrc


# Core APX shared lib
SOURCES += \
    $${APX_TOP}/lib/Mandala.cpp \
    $${APX_TOP}/lib/MandalaCore.cpp \
    $${APX_TOP}/lib/Comm.cpp \

HEADERS += \
    $${APX_TOP}/lib/Mandala.h \
    $${APX_TOP}/lib/MandalaCore.h \
    $${APX_TOP}/lib/Comm.h \
    $${APX_TOP}/lib/MandalaVars.h \


# Fact System
SOURCES += \
    FactSystem/FactSystem.cpp \
    FactSystem/FactSystemApp.cpp \
    FactSystem/FactSystemJS.cpp \
    FactSystem/FactTree.cpp \
    FactSystem/FactData.cpp \
    FactSystem/FactAction.cpp \
    FactSystem/FactListModel.cpp \
    FactSystem/FactListModelActions.cpp \
    FactSystem/Fact.cpp \

HEADERS += \
    FactSystem/FactSystem.h \
    FactSystem/FactSystemApp.h \
    FactSystem/FactSystemJS.h \
    FactSystem/FactTree.h \
    FactSystem/FactData.h \
    FactSystem/FactAction.h \
    FactSystem/FactListModel.h \
    FactSystem/FactListModelActions.h \
    FactSystem/FactValue.h \
    FactSystem/Fact.h \
    Facts.h \

# Facts TreeModel
SOURCES += \
    TreeModel/FactTreeModel.cpp \
    TreeModel/FactTreeView.cpp \
    TreeModel/FactDelegate.cpp \
    TreeModel/FactDelegateDialog.cpp \
    TreeModel/FactDelegateArray.cpp \
    TreeModel/FactDelegateScript.cpp \
    TreeModel/SourceEdit.cpp \

HEADERS += \
    TreeModel/FactTreeModel.h \
    TreeModel/FactTreeView.h \
    TreeModel/FactDelegate.h \
    TreeModel/FactDelegateDialog.h \
    TreeModel/FactDelegateArray.h \
    TreeModel/FactDelegateScript.h \
    TreeModel/SourceEdit.h \


# Database
SOURCES += \
    Database/DatabaseConnection.cpp \
    Database/VehiclesDB.cpp \
    Database/TelemetryDB.cpp \
    Database/MapsDB.cpp \
    Database/MissionsDB.cpp \

HEADERS += \
    Database/DatabaseConnection.h \
    Database/VehiclesDB.h \
    Database/TelemetryDB.h \
    Database/MapsDB.h \
    Database/MissionsDB.h \

# Vehicles & Mandala
SOURCES += \
    Vehicles/QMandalaField.cpp \
    Vehicles/QMandalaItem.cpp \
    Vehicles/QMandala.cpp \
    Vehicles/Vehicles.cpp \
    Vehicles/VehicleSelect.cpp \
    Vehicles/Vehicle.cpp \
    Vehicles/VehicleMandala.cpp \
    Vehicles/VehicleMandalaFact.cpp \
    Vehicles/VehicleNmtManager.cpp \
    Vehicles/VehicleRecorder.cpp \
    Vehicles/VehicleWarnings.cpp \

HEADERS += \
    Vehicles/QMandalaField.h \
    Vehicles/QMandalaItem.h \
    Vehicles/QMandala.h \
    Vehicles/Vehicles.h \
    Vehicles/VehicleSelect.h \
    Vehicles/Vehicle.h \
    Vehicles/VehicleMandala.h \
    Vehicles/VehicleMandalaFact.h \
    Vehicles/MandalaValue.h \
    Vehicles/VehicleNmtManager.h \
    Vehicles/VehicleRecorder.h \
    Vehicles/VehicleWarnings.h \

# Nodes
SOURCES += \
    Nodes/Nodes.cpp \
    Nodes/NodeTools.cpp \
    Nodes/NodeItem.cpp \
    Nodes/NodeItemBase.cpp \
    Nodes/NodeItemData.cpp \
    Nodes/NodeFieldBase.cpp \
    Nodes/NodeField.cpp \
    Nodes/PawnScript.cpp \
    Nodes/NodesXml.cpp \

HEADERS += \
    Nodes/Nodes.h \
    Nodes/NodeTools.h \
    Nodes/NodeItem.h \
    Nodes/NodeItemBase.h \
    Nodes/NodeItemData.h \
    Nodes/NodeFieldBase.h \
    Nodes/NodeField.h \
    Nodes/PawnScript.h \
    Nodes/NodesXml.h \

# Mission
SOURCES += \
    Mission/VehicleMission.cpp \
    Mission/MissionXml.cpp \
    Mission/MissionTools.cpp \
    Mission/MissionStorage.cpp \
    Mission/MissionListModel.cpp \
    Mission/MissionGroup.cpp \
    Mission/MissionItem.cpp \
    Mission/MissionField.cpp \
    Mission/Waypoint.cpp \
    Mission/Runway.cpp \
    Mission/Taxiway.cpp \
    Mission/Poi.cpp \

HEADERS += \
    Mission/VehicleMission.h \
    Mission/MissionXml.h \
    Mission/MissionTools.h \
    Mission/MissionStorage.h \
    Mission/MissionListModel.h \
    Mission/MissionGroup.h \
    Mission/MissionItem.h \
    Mission/MissionField.h \
    Mission/Waypoint.h \
    Mission/Runway.h \
    Mission/Taxiway.h \
    Mission/Poi.h \


# Communication
SOURCES += \
    Datalink/Datalink.cpp \
    Datalink/DatalinkPorts.cpp \
    Datalink/DatalinkPort.cpp \
    Datalink/DatalinkSocket.cpp \
    Datalink/DatalinkHosts.cpp \
    Datalink/DatalinkHost.cpp \
    Datalink/DatalinkClients.cpp \
    Datalink/DatalinkClient.cpp \
    Datalink/DatalinkStats.cpp \
    Datalink/EscReader.cpp \
    Datalink/HttpService.cpp \
    Datalink/Serial.cpp \

HEADERS += \
    Datalink/Datalink.h \
    Datalink/DatalinkPorts.h \
    Datalink/DatalinkPort.h \
    Datalink/DatalinkSocket.h \
    Datalink/DatalinkHosts.h \
    Datalink/DatalinkHost.h \
    Datalink/DatalinkClients.h \
    Datalink/DatalinkClient.h \
    Datalink/DatalinkStats.h \
    Datalink/EscReader.h \
    Datalink/HttpService.h \
    Datalink/Serial.h \

# settings
SOURCES += \
    AppSettings/AppDirs.cpp \
    AppSettings/AppSettings.cpp \
    AppSettings/AppShortcuts.cpp \
    AppSettings/AppShortcut.cpp \

HEADERS += \
    AppSettings/AppDirs.h \
    AppSettings/AppSettings.h \
    AppSettings/AppShortcuts.h \
    AppSettings/AppShortcut.h \

include( QtlocationPlugin/QtlocationPlugin.pri )



# other
SOURCES += \
    PawnCompiler.cpp \
    FlightDataFile.cpp \
    QmlView.cpp \
    SoundEffects.cpp \
    SvgImageProvider.cpp \
    SvgIcon.cpp \
    SvgMaterialIcon.cpp \

HEADERS += \
    PawnCompiler.h \
    FlightDataFile.h \
    QmlView.h \
    SoundEffects.h \
    SvgImageProvider.h \
    SvgIcon.h \
    SvgMaterialIcon.h \










