#ifndef STATUSNOTIFIERITEMDBUS_H
#define STATUSNOTIFIERITEMDBUS_H

#include <QByteArray>
#include <QString>
#include <QVector>
#include <QMetaType>
#include <QDBusArgument>

namespace qutim_dbus {
//Custom message type for DBus
struct KDbusImageStruct {
    qint32 width;
    qint32 height;
    QByteArray data;
};
typedef QVector<KDbusImageStruct> KDbusImageVector;

struct KDbusToolTipStruct {
    QString icon;
    KDbusImageVector image;
    QString title;
    QString subTitle;
};
}

const QDBusArgument &operator<<(QDBusArgument &argument, const qutim_dbus::KDbusImageStruct &icon);
const QDBusArgument &operator>>(const QDBusArgument &argument, qutim_dbus::KDbusImageStruct &icon);
Q_DECLARE_METATYPE(qutim_dbus::KDbusImageStruct)
const QDBusArgument &operator<<(QDBusArgument &argument, const qutim_dbus::KDbusImageVector &iconVector);
const QDBusArgument &operator>>(const QDBusArgument &argument, qutim_dbus::KDbusImageVector &iconVector);
Q_DECLARE_METATYPE(qutim_dbus::KDbusImageVector)
const QDBusArgument &operator<<(QDBusArgument &argument, const qutim_dbus::KDbusToolTipStruct &toolTip);
const QDBusArgument &operator>>(const QDBusArgument &argument, qutim_dbus::KDbusToolTipStruct &toolTip);
Q_DECLARE_METATYPE(qutim_dbus::KDbusToolTipStruct)


#endif // STATUSNOTIFIERITEMDBUS_H
