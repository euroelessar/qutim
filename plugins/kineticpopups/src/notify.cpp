#include "notify.h"
#include "backend.h"

namespace KineticPopups {

using namespace qutim_sdk_0_3;

Action::Action(const NotificationAction &action, QObject *parent) :
    QObject(parent),
    m_action(action)
{
}

QString Action::text() const
{
    return m_action.title();
}

void Action::trigger()
{
    m_action.trigger();
}

Notify::Notify(Notification *notify) :
    m_notify(notify),
    m_safeNotify(notify),
    m_request(m_notify->request())
{
    for (const QByteArray &name : notify->dynamicPropertyNames())
        m_additional.insert(QString::fromLatin1(name), notify->property(name));

    //add Actions
    foreach (NotificationAction action, notify->request().actions())
        m_actions << new Action(action, this);
}

QString Notify::text() const
{
    return m_request.text();
}

QString Notify::title() const
{
    QString title = m_request.title();
    if (title.isEmpty())
        title = tr("Notification from qutIM");
    return title;
}

QString Notify::avatar() const
{
    QObject *sender = m_request.object();
    if(!sender)
        return "images/qutim.svg";
    QString avatar = sender->property("avatar").toString();
    return avatar;
}

Notification::Type Notify::type() const
{
    return m_request.type();
}

QVariantMap Notify::additional() const
{
    return m_additional;
}

QObject *Notify::object() const
{
    return m_request.object();
}

QQmlListProperty<Action> Notify::actions()
{
    return QQmlListProperty<Action>(this, this, nullptr, list_count, list_at, nullptr);
}

Notification *Notify::notification() const
{
    return m_notify;
}

void Notify::accept()
{
    if (m_safeNotify)
        m_safeNotify->accept();
}

void Notify::ignore()
{
    if (m_safeNotify)
        m_safeNotify->ignore();
}

void Notify::remove()
{
    if (Backend *backend = Backend::instance())
        backend->removeNotification(m_notify);
}

int Notify::list_count(QQmlListProperty<Action> *list)
{
    return static_cast<Notify *>(list->object)->m_actions.size();
}

Action *Notify::list_at(QQmlListProperty<Action> *list, int index)
{
    return static_cast<Notify *>(list->object)->m_actions.at(index);
}

} // namespace KineticPopups
