# -------------------------------------------------
# Project created by QtCreator 2012-11-05T23:21:28
# -------------------------------------------------
QT += qml quick
QT.gui.CONFIG -= opengl
TEMPLATE = app
DESTDIR = bin
CONFIG += release
DEFINES -= UNICODE \
    _UNICODE
DEFINES += \
	PLAYER_WIDTH=1024 \
	PLAYER_HEIGHT=600 \
	OSD_SIZE=65

CONFIG(debug, debug|release) {
    OBJECTS_DIR = build/debug/obj
    MOC_DIR = build/debug/moc
    UI_HEADERS_DIR = build/debug/ui_header
	TARGET = TMPlayerGUI
    INCLUDEPATH += build/debug/ui_header
} else {
    OBJECTS_DIR = build/release/obj
    MOC_DIR = build/release/moc
    UI_HEADERS_DIR = build/release/ui_header
	TARGET = TMPlayerGUI
    INCLUDEPATH += build/release/ui_header
}

INVITE_LIBS = dbus-1 \
    glib-2.0 \
	libxml-2.0 \
	TcUtils

CONFIG += link_pkgconfig

PKGCONFIG += $$INVITE_LIBS

INCLUDEPATH += ./include

SOURCES += src/main.cpp \
    src/DBusMsgDefNames.c \
    src/MediaPlayer.cpp \
    src/AudioPlayer.cpp \
	src/TMPlayerGUIIFManager.cpp \
	src/TMPlayerGUIDBusManager.cpp

HEADERS += include/DBusMsgDef.h \
	include/MediaPlayerType.h \
    include/MediaPlayer.h \
	include/TMPlayerGUIIFManager.h \
	include/TMPlayerGUIDBusManager.h

LIBS += -lpthread \
	-lrt

RESOURCES += res/TMPlayerGUI.qrc
target.path = /usr/bin
coldplug.files = coldplug
coldplug.path = /usr/bin
INSTALLS += target \
          	coldplug
