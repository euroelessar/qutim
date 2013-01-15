// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."
import "../constants.js" as UI

SettingsItemPage {
	id: emoticonsPage
	property variant __currentEmoticons: emoticons.currentEmoticons

	impl: QtObject {
		function save() {
		}
		function load() {
		}
	}

	function rebuildComboBox() {
		var currentTheme = emoticons.currentThemeName;
		emoticonThemesBox.model.clear();
		var themes = emoticons.themes;
		var currentIndex = 0;
		for (var i = 0; i < themes.length; ++i) {
			var theme = themes[i];
			if (theme === currentTheme)
				currentIndex = i;
			emoticonThemesBox.model.append({
							       "name": theme
						       });
		}
		emoticonThemesBox.currentIndex = currentIndex;
	}

	Emoticons {
		id: emoticons
		currentThemeName: emoticonThemesBox.currentTheme
	}


	ComboBox {
		id: emoticonThemesBox
		anchors{ top: parent.top; left: parent.left; right: parent.right;}
		title: qsTr("Selected theme")
		property variant currentTheme: currentIndex < 0 ? undefined : currentItem.name
		onCurrentThemeChanged: {
			if (currentTheme !== undefined)
			{
				__currentEmoticons = emoticons.emoticons(currentTheme)
				emoticons.currentThemeName = currentTheme
			}
		}
	}

	Flickable {
		id: emoticonsFlickable
		anchors{ top: emoticonThemesBox.bottom; left: parent.left; right: parent.right; bottom: parent.bottom; }
		contentWidth: emoticonsGrid.width
		contentHeight: emoticonsGrid.height
		clip:true
		flickableDirection: Flickable.VerticalFlick

		Grid {
			id: emoticonsGrid
			columns: screen.currentOrientation === Screen.Portrait ? 5 : 10
			width: parent.width

			Repeater {
				model: __currentEmoticons
				Item {
					id: animatedEmoticon
					width: emoticonsPage.width / emoticonsGrid.columns
					height: animatedEmoticon.width

					AnimatedImage {
						id: animatedEmoticonImage
						anchors.centerIn: parent
						source: modelData
						playing: emoticonsGrid.visible
					}
				}
			}
		}
	}

	Component.onCompleted: emoticonsPage.rebuildComboBox()
}
