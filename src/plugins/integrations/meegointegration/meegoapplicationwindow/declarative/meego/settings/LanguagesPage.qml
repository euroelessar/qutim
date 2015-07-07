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

    LanguagesModel {
		id: languagesModel
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
		model: languagesModel
		ItemDelegate {
		    id: objectItem
		    title: fullName
		    subtitle: lang
		    onClicked: {
			    queryChangeLanguage.lang = lang;
			    queryChangeLanguage.langFullName = fullName;
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
	    property variant langFullName:"";
	    property variant lang:"";
	    titleText: qsTr("Language change")
	    message: qsTr("Do you really want to change qutIM's language to %1?").arg(langFullName)
	    onAccepted:{
		    config.setValue("localization/lang",lang);
	    }

    }
}
