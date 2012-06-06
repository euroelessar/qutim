// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    impl: QtObject {
	property variant list: []
	function save() {

	}
	function load() {
		    var lang=column.children[config.value("localization/lang","")];
		    if (lang!==undefined)
		    {
			var children = column.children;
			for (var i = 0; i < children.length; ++i) {
				if (children[i].subtitle && children[i].subtitle===lang)
				{
					//currentLang
				}
			}
		    }
	}
    }

    ThemeManager {
		id: themeManager
	}
    Config {
	id: config
    }
    Flickable {
	id: flickable
	anchors.fill: parent
	contentWidth: column.width
	contentHeight: column.height
	Column {
	    id: column
	    width: flickable.width
	    spacing: 10
	    Repeater {
		model: themeManager.list("languages")
		ItemDelegate {
		    id: objectItem
		    title: modelData
		    subtitle: modelData
		    onClicked: {
			    queryChangeLanguage.lang = modelData;
			    queryChangeLanguage.open();
		    }
		}
	    }
	}
    }
    QueryDialog {
	    id:queryChangeLanguage

	    acceptButtonText: qsTr("Yes")
	    rejectButtonText: qsTr("No")
	    property variant lang:"";
	    titleText: qsTr("You really want change \nqutIM language to ") + lang + "?";
	    onAccepted:{
		    config.setValue("localization/lang",lang);
	    }

    }
}
