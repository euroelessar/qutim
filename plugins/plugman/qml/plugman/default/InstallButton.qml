import QtQuick 1.1
import org.qutim.plugman 0.3
import QtDesktop 0.1

Button {
    id: itemButton

    text: suggestData(status).text
    iconSource: suggestData(status).iconSource
    enabled: status !== PackageModel.Installing && status !== PackageModel.Updating

    function suggestData(state) {
        var text;
        var iconSource = "";
        switch (state) {
        default:
            text = "Invalid - " + state;
            break;
        case PackageModel.Installable:
            text = qsTr("Install");
            //                        iconSource = "image://desktoptheme/dialog-ok"
            break;
        case PackageModel.Updateable:
            text = qsTr("Update");
            //                        iconSource = "image://desktoptheme/system-software-update"
            break;
        case PackageModel.Installing:
            text = qsTr("Installing");
            //                        iconSource = "image://desktoptheme/dialog-ok"
            break;
        case PackageModel.Updating:
            text = qsTr("Updating");
            //                        iconSource = "image://desktoptheme/dialog-ok"
            break;
        case PackageModel.Installed:
            text = qsTr("Uninstall");
            //                        iconSource = "image://desktoptheme/edit-delete"
            break;
        }
        return { text: text, iconSource: iconSource };
    }
    onClicked: {
        if (status == PackageModel.Installable || status == PackageModel.Updateable)
            packageModel.install(index);
        else if (status == PackageModel.Installed)
            packageModel.remove(index);
    }
}
