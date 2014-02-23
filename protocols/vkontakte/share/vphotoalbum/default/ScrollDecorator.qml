/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.1

Item {
    id: scrollBar

    property real position: orientation == Qt.Vertical ? flickableItem.visibleArea.yPosition
                                                       : flickableItem.visibleArea.xPosition
    property real pageSize: orientation == Qt.Vertical ? flickableItem.visibleArea.heightRatio
                                                       : flickableItem.visibleArea.widthRatio
    property Flickable flickableItem
    property int orientation: Qt.Vertical

    property int __scrollSize: 6

    width:  orientation == Qt.Vertical ? __scrollSize : flickableItem.width
    height: orientation == Qt.Vertical ? flickableItem.height : __scrollSize
    anchors.right:  flickableItem.right

    // A light, semi-transparent background
    Rectangle {
        id: background
        anchors.fill: parent
        radius: orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
        color: "black"
        opacity: area.containsMouse ? 0.1 : 0
    }

    // Size the bar to the required size, depending upon the orientation.
    Rectangle {
        id: handle
        x: orientation == Qt.Vertical ? 1 : (scrollBar.position * (scrollBar.width-2) + 1)
        y: orientation == Qt.Vertical ? (scrollBar.position * (scrollBar.height-2) + 1) : 1
        width: orientation == Qt.Vertical ? (parent.width-2) : (scrollBar.pageSize * (scrollBar.width-2))
        height: orientation == Qt.Vertical ? (scrollBar.pageSize * (scrollBar.height-2)) : (parent.height-2)
        radius: orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
        color: "black"
        opacity: 0
    }

    MouseArea {
        id: area
        hoverEnabled: true
        anchors.fill: parent
    }

    // Only show the scrollbars when the view is moving.
    states: [
        State {
        name: "ShowBars"
        when: flickableItem.movingVertically || area.containsMouse
        PropertyChanges { target: handle; opacity: 0.3 }
    }
    ]
}
