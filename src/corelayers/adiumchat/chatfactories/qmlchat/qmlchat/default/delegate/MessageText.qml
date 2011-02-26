import Qt 4.7

Text {
	id: message
	textFormat: "RichText"
	wrapMode: "WordWrap"
	onLinkActivated: Qt.openUrlExternally(link)
}
