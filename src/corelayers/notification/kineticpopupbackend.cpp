#include "kineticpopupbackend.h"
#include "kineticpopup.h"
#include "kineticpopupsmanager.h"
#include <QVariant>
#include <QTime>


void KineticPopupBackend::show(NotificationType type, QObject* sender, const QString& body, const QString& customTitle)
{
	KineticPopupsManager *manager =  KineticPopupsManager::self();
	if (!(manager->showFlags & type))
	{
		return;
	}
	QString popup_id = (sender==0) ? getCurrentTime() : sender->property("id").toString();
	QString title = getTitle(type,popup_id,sender->property("name").toString());
	QString image_path = (sender==0) ? QString() : sender->property("imagepath").toString();
	KineticPopup *popup = manager->getById(popup_id);
	if (popup != 0)
	{
		if (manager->appendMode) {
			popup->appendMessage(body);
			return;
		}
		else if (sender!=0)
		{
			popup_id.append("."+getCurrentTime());
		}
	}
	popup  = new KineticPopup ();
	popup->setId(popup_id);
	popup->setTimeOut(manager->timeout);
	popup->setMessage(customTitle.isEmpty() ? title : customTitle,body,image_path);
	setActions(sender,type,popup);
	popup->send();
}

QString KineticPopupBackend::getTitle(NotificationType type, QString& id,const QString& sender) const
{
	QString title;
	QString append_id;
	switch ( type )
	{
	case NotifySystem:
		title = tr("System message from %1:").arg(sender.isEmpty() ? "qutIM" : sender);
		append_id = "SystemMessage";
		break;
	case NotifyStatusChange:
		title = tr("%1 was changed status").arg(sender);
		append_id = "NotifyStatusChange";
		break;
	case NotifyMessageGet:
		title = tr("Message from %1:").arg(sender);
		append_id = "NotifyMessageGet";
		break;
	case NotifyTyping:
		title = tr("%1 is typing").arg(sender);
		append_id = "NotifyTyping";
		break;
	case NotifyBlockedMessage:
		title = tr("Blocked message from %1").arg(sender);
		append_id = "NotifyBlockedMessage";
		break;
	case NotifyBirthday:
		title = tr("%1 has birthday today!!").arg(sender);
		append_id = "NotifyBirthday";
		break;
	case NotifyCustom:
		title = tr("Custom message for %1").arg(sender);
		append_id = "NotifyCustom";
		break;
	case NotifyOnline:
		title = tr("%1 is online").arg(sender);
		append_id = "NotifyOnline";
		break;
	case NotifyOffline:
		title = tr("%1 is offline").arg(sender);
		append_id = "NotifyOffline";
		break;
	case NotifyStartup:
		title = tr("qutIM launched");
		append_id = "NotifyStartup";
		break;
	case NotifyCount:
		title = tr("Count");
		append_id = "NotifyCount";
		break;
	default:
		return title;
	}
	id.append("."+append_id);
	return title;
}


void KineticPopupBackend::setActions(QObject* sender, NotificationType type, KineticPopup* popup)
{

}


QString KineticPopupBackend::getCurrentTime()
{
	return QString::number(QDateTime::currentDateTime().toTime_t());
}
