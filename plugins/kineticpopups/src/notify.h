#ifndef KINETICPOPUPS_NOTIFY_H
#define KINETICPOPUPS_NOTIFY_H

#include <QObject>
#include <QQmlListProperty>
#include <qutim/notification.h>

namespace KineticPopups {

class Action : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text CONSTANT)
public:
    Action(const qutim_sdk_0_3::NotificationAction &action, QObject *parent = 0);

    QString text() const;

public slots:
    void trigger();

private:
    qutim_sdk_0_3::NotificationAction m_action;
};

class Notify : public QObject
{
    Q_OBJECT
    Q_ENUMS(qutim_sdk_0_3::Notification::Type)

	Q_PROPERTY(QString title READ title CONSTANT)
	Q_PROPERTY(QString text  READ text CONSTANT)
	Q_PROPERTY(QString avatar READ avatar CONSTANT)
	Q_PROPERTY(QObject* object READ object CONSTANT)
	Q_PROPERTY(qutim_sdk_0_3::Notification::Type type READ type CONSTANT)
	Q_PROPERTY(QQmlListProperty<KineticPopups::Action> actions READ actions CONSTANT)
	Q_PROPERTY(QVariantMap additional READ additional CONSTANT)
public:
    explicit Notify(qutim_sdk_0_3::Notification *parent = 0);

	QString title() const;
	QString text() const;
	QString avatar() const;
	QObject* object() const;
	qutim_sdk_0_3::Notification::Type type() const;
	QVariantMap additional() const;
	QQmlListProperty<Action> actions();

	qutim_sdk_0_3::Notification *notification() const;

public slots:
    void accept();
    void ignore();
    void remove();

protected:
    static int list_count(QQmlListProperty<Action> *list);
    static Action *list_at(QQmlListProperty<Action> *list, int index);

private:
    qutim_sdk_0_3::Notification *m_notify;
	QPointer<qutim_sdk_0_3::Notification> m_safeNotify;
	qutim_sdk_0_3::NotificationRequest m_request;
    QVariantMap m_additional;
	QList<Action *> m_actions;
};

} // namespace KineticPopups

#endif // KINETICPOPUPS_NOTIFY_H
