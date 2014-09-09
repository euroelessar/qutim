/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2014 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import org.qutim.simpleabout 0.4

ApplicationWindow {
    id: root
    width: 450
    height: 400
    
    title: qsTr("About qutIM")
    
    AboutInfo {
        id: info
    }
    
    readonly property string translators: info.translators
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight | Qt.AlignTop

            Label {
                text: "qutIM"
                font.bold: true
                font.pointSize: qutimVersion.font.pointSize * 4
            }
            Label {
                id: qutimVersion
                text: info.qutimVersion
            }
        }
        
        Label {
            text: qsTr("Module based instant messenger. Based on Qt %1 (%2 bits)").arg(info.qtVersion).arg(info.wordSize)
        }
        
        GridLayout {
            columns: 2
            Label {
                text: qsTr("Project site:")
            }
            Label {
                text: "<a href=\"http://qutim.org/\">http://qutim.org</a>"
                onLinkActivated: Qt.openUrlExternally(link)
            }
            Label {
                text: qsTr("Source code repository:")
            }
            Label {
                text: "<a href=\"http://github.com/euroelessar/qutim/\">http://github.com/euroelessar/qutim/</a>"
                onLinkActivated: Qt.openUrlExternally(link)
            }
            Label {
                text: qsTr("Bug tracker:")
            }
            Label {
                text: "<a href=\"http://trac.qutim.org/\">http://trac.qutim.org/</a>"
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }

        TabView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Tab {
                title: qsTr("Developers")
                
                sourceComponent: TextArea {
                    anchors.fill: parent
                    readOnly: true
                    textFormat: Qt.RichText
                    text: info.developers
                }
            }
            Tab {
                title: qsTr("Translators")
                visible: root.translators.length > 0
                
                sourceComponent: TextArea {
                    anchors.fill: parent
                    readOnly: true
                    textFormat: Qt.RichText
                    text: root.translators
                }
            }
            Tab {
                title: qsTr("License")
                
                sourceComponent: TextArea {
                    anchors.fill: parent
                    readOnly: true
                    textFormat: Qt.RichText
                    text: info.license
                }
            }
        }
    }
}
