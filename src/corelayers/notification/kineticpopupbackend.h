#ifndef KINETICPOPUPBACKEND_H
#define KINETICPOPUPBACKEND_H

#include <libqutim/notificationslayer.h>

using namespace qutim_sdk_0_3;

namespace KineticPopups
{
	class Popup;
	class Backend : public PopupBackend
	{
		Q_OBJECT
	public:
		virtual void show(Notifications::Type type, QObject* sender, const QString& body, const QString& customTitle);
		Backend ();
	private:
		QString getTitle(Notifications::Type type, QString& id, const QString& sender = QString()) const;//also modify notify id
		void setActions(QObject* sender, Notifications::Type type, Popup *popup);
	};

}

#endif //KINETICPOPUPBACKEND_H
