#ifndef KINETICPOPUPBACKEND_H
#define KINETICPOPUPBACKEND_H

#include <libqutim/notificationslayer.h>

class KineticPopup;
using namespace qutim_sdk_0_3;

class KineticPopupBackend : public PopupBackend
{
	Q_OBJECT
public:
    virtual void show(Notifications::Type type, QObject* sender, const QString& body, const QString& customTitle);
    KineticPopupBackend ();
private:
	QString getTitle(Notifications::Type type, QString& id, const QString& sender = QString()) const;//also modify notify id
	void setActions(QObject* sender, Notifications::Type type, KineticPopup *popup);
};

#endif //KINETICPOPUPBACKEND_H
