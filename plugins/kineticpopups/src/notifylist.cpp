#include "notifylist.h"
#include "backend.h"

namespace KineticPopups {

using namespace qutim_sdk_0_3;

NotifyList::NotifyList(QObject *parent) :
    QObject(parent)
{
    m_backend = Backend::instance();

    if (m_backend) {
        foreach (Notification *notification, m_backend->notifications())
            m_notifies << new Notify(notification);

        connect(m_backend.data(), &Backend::notificationAdded, this, &NotifyList::onNotificationAdded);
        connect(m_backend.data(), &Backend::notificationRemoved, this, &NotifyList::onNotificationRemoved);
    }
}

QQmlListProperty<Notify> NotifyList::notifies()
{
    return QQmlListProperty<Notify>(this, this, nullptr, list_count, list_at, nullptr);
}

int NotifyList::list_count(QQmlListProperty<Notify> *list)
{
    return static_cast<NotifyList *>(list->object)->m_notifies.size();
}

Notify *NotifyList::list_at(QQmlListProperty<Notify> *list, int index)
{
    return static_cast<NotifyList *>(list->object)->m_notifies.at(index);
}

void NotifyList::onNotificationAdded(Notification *notification)
{
    Notify *notify = new Notify(notification);

    m_notifies << notify;
    emit notifyAdded(notify);
    emit notifiesChanged();
}

void NotifyList::onNotificationRemoved(Notification *notification)
{
    for (int i = 0; i < m_notifies.size(); ++i) {
        Notify *notify = m_notifies.at(i);
        if (notify->notification() == notification) {
            m_notifies.removeAt(i);
            emit notifyRemoved(notify);
            emit notifiesChanged();

            notify->deleteLater();
            --i;
        }
    }
}

} // namespace KineticPopups
