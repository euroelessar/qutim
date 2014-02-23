#ifndef KINETICPOPUPS_NOTIFYLIST_H
#define KINETICPOPUPS_NOTIFYLIST_H

#include <QObject>
#include <QQmlListProperty>
#include "notify.h"

namespace KineticPopups {

class Backend;

class NotifyList : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<KineticPopups::Notify> notifies READ notifies NOTIFY notifiesChanged)
public:
    explicit NotifyList(QObject *parent = 0);

    QQmlListProperty<Notify> notifies();

protected:
    static int list_count(QQmlListProperty<Notify> *list);
    static Notify *list_at(QQmlListProperty<Notify> *list, int index);

    void onNotificationAdded(qutim_sdk_0_3::Notification *notification);
    void onNotificationRemoved(qutim_sdk_0_3::Notification *notification);

signals:
    void notifiesChanged();
    void notifyAdded(Notify *notify);
    void notifyRemoved(Notify *notify);
    
private:
    QPointer<Backend> m_backend;
    QList<Notify *> m_notifies;
};

} // namespace KineticPopups

#endif // KINETICPOPUPS_NOTIFYLIST_H
