#ifndef KINETICPOPUPBACKEND_H
#define KINETICPOPUPBACKEND_H

#include <libqutim/notificationslayer.h>

class KineticPopup;
using namespace qutim_sdk_0_3;

class KineticPopupBackend : public PopupBackend
{
	Q_OBJECT
public:
    virtual void show(NotificationType type, QObject* sender, const QString& body, const QString& customTitle);
private:
	QString getTitle(NotificationType type, QString& id, const QString& sender = QString()) const;//also modify notify id
	void setActions(QObject* sender, NotificationType type, KineticPopup *popup);
};

#endif //KINETICPOPUPBACKEND_H
