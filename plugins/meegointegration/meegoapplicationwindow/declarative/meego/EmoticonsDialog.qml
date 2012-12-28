/*
 * Copyright (C) 2012 Evgeniy Degtyarev <degtep@gmail.com>
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

Rectangle {
	id: emoticonsDialog
	property variant selectedEmoticon
	property variant __currentEmoticons: emoticons.currentEmoticons

	function hide() {
		state = "hidden"
	}

	function show() {
		state = "visible"
	}

	color: "white"
	state: "hidden"

	Emoticons {
		id: emoticons
	}

	Flickable {
		anchors.fill: parent
		contentWidth: emoticonsGrid.width
		contentHeight: emoticonsGrid.height
		clip: true
		flickableDirection: Flickable.VerticalFlick

		Grid {
			id: emoticonsGrid
			columns:  width <= 500 ? 5 : 10
			width: parent.width

			Repeater {
				model: __currentEmoticons
				Item {
					id: animatedEmoticon
					width: emoticonsDialog.width / emoticonsGrid.columns
					height: animatedEmoticon.width

					AnimatedImage {
						id: animatedEmoticonImage
						anchors.centerIn: parent
						source: modelData
						playing: emoticonsDialog.visible
					}

					MouseArea {
						id: mouse
						anchors.fill: animatedEmoticon
						hoverEnabled: true
						onClicked: {
							selectedEmoticon = emoticons.currentEmoticonString(modelData);
							hide();
						}
					}
				}
			}
		}
	}

	states: [
		State {
			name: "visible"
			PropertyChanges { target: emoticonsDialog; z: 10 }
			PropertyChanges { target: emoticonsDialog; selectedEmoticon: undefined }
			PropertyChanges { target: emoticonsDialog; visible: true }
		},
		State {
			name: "hidden"
			PropertyChanges { target: emoticonsDialog; visible: false }
		}
	]
}

