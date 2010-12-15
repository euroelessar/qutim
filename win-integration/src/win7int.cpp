#include "win7int.h"
#include <WinThings/OverlayIcon.h>
#include <qutim/extensionicon.h>
#include <qutim/conference.h>
#include <qutim/servicemanager.h>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <QPainter>
#include <QImage>
#include <QSysInfo>

using qutim_sdk_0_3::ExtensionIcon;
using qutim_sdk_0_3::ChatSession;
using qutim_sdk_0_3::ChatLayer;
using qutim_sdk_0_3::Conference;
using qutim_sdk_0_3::Icon;
using qutim_sdk_0_3::ServiceManager;
using qutim_sdk_0_3::warning;

Win7Int2::Win7Int2()
{

}

void Win7Int2::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Vizir Ivan"),
		QT_TRANSLATE_NOOP("Task", "Author"),
		QLatin1String("define-true-false@yandex.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Windows 7 Integration"),
		QT_TRANSLATE_NOOP("Plugin", "/* TODO */"),
		PLUGIN_VERSION(1, 2, 3, 4),
		ExtensionIcon());
}

bool Win7Int2::load()
{
	if(QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS7){ // even if plugin suddenly loads on WinVista, it won't work
		connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	}
	return true;
}

bool Win7Int2::unload()
{
	return true;
}

void Win7Int2::onSessionCreated(qutim_sdk_0_3::ChatSession *s)
{
	connect(s, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)), Qt::UniqueConnection);
}

void Win7Int2::onUnreadChanged(qutim_sdk_0_3::MessageList)
{
	QList<ChatSession*> sessions = ChatLayer::instance()->sessions();
	quint32 unreadConfs = 0, unreadChats = 0;
	foreach(ChatSession* s, sessions){
		if(qobject_cast<Conference*>(s->getUnit()))
			unreadConfs  += s->unread().size();
		else
			unreadChats += s->unread().size();
	}
	QPixmap icon = generateOverlayIcon(unreadConfs, unreadChats);
	warning() << sessions;
	QObject* obj = qobject_cast<QObject*>(ServiceManager::getByName("ChatForm"));
	QWidgetList list;
	QMetaObject::invokeMethod(obj, "chatWidgets", Qt::DirectConnection, Q_RETURN_ARG(QWidgetList, list));
	if(!list.size())
		return;
	if(unreadChats+unreadConfs)
		OverlayIcon::set(list.at(0), icon);
	else
		OverlayIcon::clear(list.at(0));
}

QPixmap Win7Int2::generateOverlayIcon(quint32 unreadConfs, quint32 unreadChats)
{
	bool cfg_displayCount = true; // TODO
	bool cfg_addConfs     = false; // TODO
	quint32 count = unreadChats + (cfg_addConfs ? unreadConfs : 0);
	QPixmap icon;
	if(unreadConfs && !unreadChats)
		icon = Icon("mail-message").pixmap(16, 16);
	else
		icon = Icon("mail-unread-new").pixmap(16, 16);
	QPainter painter(&icon);
	if(cfg_displayCount && count){
		QFont font;
		font.setWeight(QFont::DemiBold);
		font.setFamily("Segoe UI");
		font.setPointSize(7);
		painter.setFont(font);
		painter.setPen(Qt::darkBlue);
		painter.drawText(QRect(0, 1, 15, 15), Qt::AlignCenter, QString::number(count));
	}
	return icon;
}

QUTIM_EXPORT_PLUGIN(Win7Int2)
