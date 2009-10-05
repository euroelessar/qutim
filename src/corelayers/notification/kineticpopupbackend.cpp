#include "kineticpopupbackend.h"
#include "kineticpopup.h"
#include "kineticpopupsmanager.h"
#include "src/modulemanagerimpl.h"
#include <QVariant>
#include <QTime>
#include <QDebug>

static Core::CoreModuleHelper<KineticPopupBackend> kinetic_popup_static(
		QT_TRANSLATE_NOOP("Plugin", "Kinetic popups"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM popup realization. Powered by Kinetic")
		);

void KineticPopupBackend::show(NotificationType type, QObject* sender, const QString& body, const QString& customTitle)
{
	KineticPopupsManager *manager =  KineticPopupsManager::self();
	if (!(manager->showFlags & type))
	{
		return;
	}
	static int id_counter = 0;
	QString sender_id = sender ? sender->property("id").toString() : QString();
	QString sender_name = sender ? sender->property("name").toString() : QString();
	if(sender_name.isEmpty())
		sender_name = sender_id;
	QString popup_id = sender_id.isEmpty() ? QString::number(id_counter++) : sender_id;
	QString title = getTitle(type, popup_id, sender_name);
	QString image_path = sender ? sender->property("imagepath").toString() : QString();
	if(image_path.isEmpty())
		image_path = QLatin1String(":/icons/qutim_64");
	KineticPopup *popup = manager->getById(popup_id);
	if (popup != 0)
	{
		if (manager->appendMode) {
			popup->appendMessage(body);
			return;
		}
		else if (sender!=0)
		{
			popup_id.append("." + QString::number(id_counter++));
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
		title = tr("%1 changed status").arg(sender);
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
