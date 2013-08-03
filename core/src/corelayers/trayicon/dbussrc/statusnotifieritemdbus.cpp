#include "statusnotifieritemdbus.h"

using namespace qutim_dbus;

// Marshall the ImageStruct data into a D-BUS argument
const QDBusArgument &operator<<(QDBusArgument &argument, const KDbusImageStruct &icon)
{
    argument.beginStructure();
    argument << icon.width;
    argument << icon.height;
    argument << icon.data;
    argument.endStructure();
    return argument;
}
// Retrieve the ImageStruct data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, KDbusImageStruct &icon)
{
    qint32 width;
    qint32 height;
    QByteArray data;
    argument.beginStructure();
    argument >> width;
    argument >> height;
    argument >> data;
    argument.endStructure();
    icon.width = width;
    icon.height = height;
    icon.data = data;
    return argument;
}
// Marshall the ImageVector data into a D-BUS argument
const QDBusArgument &operator<<(QDBusArgument &argument, const KDbusImageVector &iconVector)
{
    argument.beginArray(qMetaTypeId<KDbusImageStruct>());
    for (int i=0; i<iconVector.size(); ++i) {
        argument << iconVector[i];
    }
    argument.endArray();
    return argument;
}
// Retrieve the ImageVector data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, KDbusImageVector &iconVector)
{
    argument.beginArray();
    iconVector.clear();
    while ( !argument.atEnd() ) {
        KDbusImageStruct element;
        argument >> element;
        iconVector.append(element);
    }
    argument.endArray();
    return argument;
}
// Marshall the ToolTipStruct data into a D-BUS argument
const QDBusArgument &operator<<(QDBusArgument &argument, const KDbusToolTipStruct &toolTip)
{
    argument.beginStructure();
    argument << toolTip.icon;
    argument << toolTip.image;
    argument << toolTip.title;
    argument << toolTip.subTitle;
    argument.endStructure();
    return argument;
}
// Retrieve the ToolTipStruct data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, KDbusToolTipStruct &toolTip)
{
    QString icon;
    KDbusImageVector image;
    QString title;
    QString subTitle;
    argument.beginStructure();
    argument >> icon;
    argument >> image;
    argument >> title;
    argument >> subTitle;
    argument.endStructure();
    toolTip.icon = icon;
    toolTip.image = image;
    toolTip.title = title;
    toolTip.subTitle = subTitle;
    return argument;
}
