#ifndef NOTIFICATIONSLAYER_H
#define NOTIFICATIONSLAYER_H

#include <QObject>
#include <QPointer>

namespace qutim_sdk_0_3 {
    enum NotificationType {
        //TODO
    };

	class Contact;
    class NotificationsLayer : public QObject
    {
        Q_OBJECT
    public:
        virtual void SendSimpleNotification(QString title, QString body = "") = 0;
        virtual void SendNotification(NotificationType type,
									  Contact contact,
									  QString title,
									  QString body = ""
									  ) = 0;
    protected:
        NotificationsLayer();
        virtual ~NotificationsLayer();
        virtual void playSound(NotificationType type);
    private:
        static QPointer<NotificationsLayer> instance;
    };

}

#endif // NOTIFICATIONSLAYER_H
