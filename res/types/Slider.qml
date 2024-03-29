/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.2

Item {
    id: slider

    height: handleBack.height
    // value is read/write.
    property real value: 0
    property real maximum: 1
    property real minimum: 0
    property int xMax: width - handle.width
    onXMaxChanged: updatePos()
    onMinimumChanged: updatePos()
    onValueChanged: if (!pressed) updatePos()
    property bool mutable: true
    property alias pressed : backgroundMouse.pressed

    signal valueChangedByHandle(int newValue)

    function updatePos() {
        if (maximum > minimum) {
            var pos = 0 + (value - minimum) * slider.xMax / (maximum - minimum);
            pos = Math.min(pos, width - handle.width - 0);
            pos = Math.max(pos, 0);
            handle.x = pos;
        } else {
            handle.x = 0;
        }
    }

    Rectangle {
        id: background
        width: slider.width
        anchors.verticalCenter: slider.verticalCenter
        height: 2
        color: "#666666"

        MouseArea {
            id: backgroundMouse
            anchors.fill: parent
            anchors.topMargin: -24
            anchors.bottomMargin: -24
            enabled: slider.mutable
            drag.target: handle
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: slider.xMax
			/*
            onClicked: {
                value = Math.max(minimum, Math.min(maximum, (maximum - minimum) * (mouseX - handle.width/2) / slider.xMax + minimum));
                valueChangedByHandle(value);
                updatePos();
            }
            onWheel: {
                value = Math.max(minimum, Math.min(maximum, value + (wheel.angleDelta.y > 0 ? 1 : -1) * (10 / slider.xMax) * (slider.maximum - slider.minimum)));
                valueChangedByHandle(value);
                updatePos();
            }
			*/
            onPressedChanged: {
                value = Math.max(minimum, Math.min(maximum, (maximum - minimum) * (mouseX - handle.width/2) / slider.xMax + minimum));
                valueChangedByHandle(value);
                updatePos();
            }
            onPositionChanged: {
                value = Math.max(minimum, Math.min(maximum, (maximum - minimum) * (mouseX - handle.width/2) / slider.xMax + minimum));
                //valueChangedByHandle(value);
            }
        }
    }

    Rectangle {
        id: progress
        height: 5
        anchors.verticalCenter: background.verticalCenter
        anchors.left: background.left
        anchors.right: handle.right
        anchors.rightMargin: handle.width / 2
        visible: slider.enabled
        color: "#98c66c"
    }

    Rectangle {
        id: handleBack
        width: 40
        height: width
        radius: width / 2
        color: "#8898c66c"
        antialiasing: true
        anchors.centerIn: handle
        visible: handle.visible
    }

    Rectangle {
        id: handle
        width: 14
        height: width
        radius: width / 2
        antialiasing: true
        color: "#98c66c"
        anchors.verticalCenter: background.verticalCenter
        visible: slider.enabled
    }
}

