Product {
    type: ["installed_content"]

    property string qutim_version_major: project.qutim_version_major
    property string qutim_version_minor: project.qutim_version_minor
    property string qutim_version_release: project.qutim_version_release
    property string qutim_version_patch: project.qutim_version_patch
    property string qutim_version: project.qutim_version

    name: "qutim-headers"

    Transformer {
        inputs: [ "libqutim/version.h.cmake" ]
        Artifact {
            fileName: "include/qutim/libqutim_version.h"
            fileTags: [ "hpp" ]
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "generating libqutim_version.h";
            cmd.highlight = "codegen";
            cmd.qutim_version = product.qutim_version;
            cmd.qutim_version_major = product.qutim_version_major;
            cmd.qutim_version_minor = product.qutim_version_minor;
            cmd.qutim_version_release = product.qutim_version_release;
            cmd.qutim_version_patch = product.qutim_version_patch;
            cmd.onWindows = (product.modules.qbs.targetOS === "windows");
            cmd.sourceCode = function() {
                var file = new TextFile(input.fileName);
                var content = file.readAll();
                // replace Windows line endings
                if (onWindows)
                    content = content.replace(/\r\n/g, "\n");
                content = content.replace(/\${CMAKE_QUTIM_VERSION_STRING}/g, qutim_version);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_MAJOR}/g, qutim_version_major);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_MINOR}/g, qutim_version_minor);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_SECMINOR}/g, qutim_version_release);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_PATCH}/g, qutim_version_patch);
                file = new TextFile(output.fileName, TextFile.WriteOnly);
                file.truncate();
                file.write(content);
                file.close();
            }
            return cmd;
        }
    }

    files: "libqutim/version.h.cmake"

    Group {
        qbs.installDir: "include/qutim"
        fileTags: ["install"]
        files: [
            "libqutim/abstractsearchrequest.h",
            "libqutim/abstractwizardpage.h",
            "libqutim/account.h",
            "libqutim/accountmanager.h",
            "libqutim/actionbox.h",
            "libqutim/actiongenerator.h",
            "libqutim/actiontoolbar.h",
            "libqutim/authorizationdialog.h",
            "libqutim/buddy.h",
            "libqutim/chatsession.h",
            "libqutim/chatunit.h",
            "libqutim/conference.h",
            "libqutim/configbase.h",
            "libqutim/config.h",
            "libqutim/contact.h",
            "libqutim/contactsearch.h",
            "libqutim/cryptoservice.h",
            "libqutim/dataforms.h",
            "libqutim/datasettingsobject.h",
            "libqutim/debug.h",
            "libqutim/declarativeview.h",
            "libqutim/emoticons.h",
            "libqutim/event.h",
            "libqutim/extensionicon.h",
            "libqutim/extensioninfo.h",
            "libqutim/filetransfer.h",
            "libqutim/groupchatmanager.h",
            "libqutim/history.h",
            "libqutim/icon.h",
            "libqutim/iconloader.h",
            "libqutim/inforequest.h",
            "libqutim/jsonfile.h",
            "libqutim/json.h",
            "libqutim/libqutim_global.h",
            "libqutim/localizedstring.h",
            "libqutim/menucontroller.h",
            "libqutim/message.h",
            "libqutim/messagehandler.h",
            "libqutim/metacontact.h",
            "libqutim/metacontactmanager.h",
            "libqutim/metaobjectbuilder.h",
            "libqutim/mimeobjectdata.h",
            "libqutim/modulemanager.h",
            "libqutim/networkproxy.h",
            "libqutim/notification.h",
            "libqutim/objectgenerator.h",
            "libqutim/passworddialog.h",
            "libqutim/personinfo.h",
            "libqutim/plugin.h",
            "libqutim/profilecreatorpage.h",
            "libqutim/profile.h",
            "libqutim/protocol.h",
            "libqutim/qtwin.h",
            "libqutim/rosterstorage.h",
            "libqutim/scripttools.h",
            "libqutim/servicemanager.h",
            "libqutim/settingslayer.h",
            "libqutim/settingswidget.h",
            "libqutim/shortcut.h",
            "libqutim/sound.h",
            "libqutim/spellchecker.h",
            "libqutim/startupmodule.h",
            "libqutim/statusactiongenerator.h",
            "libqutim/status.h",
            "libqutim/systeminfo.h",
            "libqutim/systemintegration.h",
            "libqutim/tcpsocket.h",
            "libqutim/thememanager.h",
            "libqutim/tooltip.h",
            "libqutim/utils/avatarfilter.h",
            "libqutim/utils.h",
            "libqutim/utils/itemdelegate.h",
            "libqutim/widgetactiongenerator.h",
        ]
    }
}
