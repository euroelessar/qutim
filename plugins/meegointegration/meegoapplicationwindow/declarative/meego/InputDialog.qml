/*
 * Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

import QtQuick 1.0
import com.nokia.meego 1.0
import org.qutim 0.3
                          
QueryDialog {
    id: dialog
    property QtObject widget: null
    property bool hasWidget: widget !== null
    titleText: dialog.hasWidget ? dialog.widget.windowTitle : ""
    property string __okButtonText: hasWidget ? widget.okButtonText : "";
    property string __cancelButtonText: hasWidget ? widget.cancelButtonText : "";
    acceptButtonText: __okButtonText == "" ? qsTr("Ok") : __okButtonText
    rejectButtonText: __cancelButtonText == "" ? qsTr("Cancel") : __cancelButtonText
    content: Column {
        spacing: 10
        Text {
            id: queryText
            width: dialog.width
            horizontalAlignment: Text.AlignHCenter
            font.family: dialog.platformStyle.messageFontFamily
            font.pixelSize: dialog.platformStyle.messageFontPixelSize
            color: dialog.platformStyle.messageTextColor
            wrapMode: Text.WordWrap
            text: dialog.hasWidget ? dialog.widget.labelText : ""
        }
        TextField {
            id: textField
            width: dialog.width
            text: dialog.hasWidget ? dialog.widget.textValue : ""
        }
    }
    onAccepted: {
        widget.textValue = textField.text;
        widget.accept();
    }
    onRejected: widget.reject();
}
