// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.1
import org.qutim.status 0.4

Rectangle {
    id: listItem
    height: 28
    width: parent.width
    clip: true
    property bool hasAvatar: model.avatar !== ""

    color: {
        if (ListView.isCurrentItem)
            return palette.highlight;
        else
            return index % 2 == 0 ? palette.alternateBase : palette.base
    }

    property string fontFamily: "DejaVu Sans"
    
    property int titleSize: 10
    property int titleWeight: Font.Bold
    property color titleColor: ListView.isCurrentItem ? palette.highlightedText : palette.text

    property int subtitleSize: 9
    property int subtitleWeight: Font.Light
    property color subtitleColor: ListView.isCurrentItem ? palette.highlightedText : palette.text

    ShaderEffectSource {
        id: avatarImageSource

        sourceItem: AnimatedImage {
            source: model.avatar
            smooth: true
            asynchronous: true
            width: 24
            height: 24
        }

        hideSource: true
    }
    
    Row {
        anchors.fill: parent
        spacing: 2

        Item {
            id: statusItem
            width: 24
            height: 24
            anchors.verticalCenter: parent.verticalCenter

            ShaderEffect {
                id: avatarImage
                anchors.fill: parent
                visible: listItem.hasAvatar

                property variant source: avatarImageSource

                property real thickness: 0.05

                fragmentShader: "
                           varying highp vec2 qt_TexCoord0;
                           uniform sampler2D source;
                           uniform float thickness;
                           void main(void)
                           {
                               highp vec4 texColor = texture2D(source, qt_TexCoord0.st);
                               highp float x = 0.5 - abs(0.5 - qt_TexCoord0.x);
                               highp float y = 0.5 - abs(0.5 - qt_TexCoord0.y);
                               float factor = 1.0;
                               if (x < thickness) {
                                   if (y > thickness)
                                       factor = x / thickness;
                                   else {
                                       factor = (y / thickness) * (x / thickness);
                                   }
                               } else if (y < thickness)
                                   factor = y / thickness;
                               gl_FragColor = texColor * factor;
                       }
                       "
            }

            Image {
                id: statusImage
                width: listItem.hasAvatar ? parent.width / 2 : parent.width
                height: listItem.hasAvatar ? parent.height / 2 : parent.height
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                smooth: false
                sourceSize.width: width
                sourceSize.height: height

                source: "image://statustheme/" + model.statusId
            }
        }
        
        Column {
            anchors.verticalCenter: parent.verticalCenter

            Text {
                id: mainText
                width: listItem.width - statusItem.width - 4
                text: model.title
                font.weight: listItem.titleWeight
                font.pixelSize: listItem.titleSize
                color: listItem.titleColor
                font.family: listItem.fontFamily
                maximumLineCount: 1
                elide: Text.ElideRight
            }

            Text {
                id: subText
                width: listItem.width - statusItem.width - 4
                text: model.statusText
                font.weight: listItem.subtitleWeight
                font.pixelSize: listItem.subtitleSize
                color: listItem.subtitleColor
                font.family: listItem.fontFamily
                maximumLineCount: 1
                elide: Text.ElideRight

                visible: text != ""
            }
        }
    }
//    MouseArea {
//        id: mouseArea;
//        anchors.fill: parent
//        onClicked: {
//            listItem.clicked();
//        }
//        onPressAndHold: {
//            listItem.pressAndHold();
//        }
//    }
}
