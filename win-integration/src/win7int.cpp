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
#include <QPainter>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <qutim/icon.h>

using qutim_sdk_0_3::ExtensionIcon;
using qutim_sdk_0_3::ChatSession;
using qutim_sdk_0_3::ChatLayer;
using qutim_sdk_0_3::Conference;
using qutim_sdk_0_3::Icon;
using qutim_sdk_0_3::ServiceManager;
using qutim_sdk_0_3::Icon;
using qutim_sdk_0_3::warning;

Win7Int2::Win7Int2()
{
	previousWindow = 0;
	previousTabId  = 0;
	previews       = 0;
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
	if (QSysInfo::windowsVersion() != QSysInfo::WV_WINDOWS7)
		return false; // even if plugin suddenly loads on WinVista, it won't work
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	previews = new WPreviews;
	QWidget *cl = ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>();
	TaskbarPreviews::setWindowAttributes(cl, TA_Peek_ExcludeFrom|TA_Flip3D_ExcludeBelow);
	return true;
}

bool Win7Int2::unload()
{
	QWidget *cl = ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>();
	TaskbarPreviews::setWindowAttributes(cl, TA_NoAttributes);
	delete previews;
	return true;
}

void Win7Int2::onSessionCreated(qutim_sdk_0_3::ChatSession *s)
{
	connect(s, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)), Qt::UniqueConnection);
	connect(s, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
}

void Win7Int2::onSessionActivated(bool activated)
{
	if (activated)
		testTab();
}

void Win7Int2::testTab()
{
	QWidget *w = chatWindow();
	if (w != previousWindow) {
		TaskbarPreviews::tabRemove(previousTabId);
		previousTabId = 0;
	}
	if (!previousTabId && w) {
		previousTabId  = TaskbarPreviews::tabAddVirtual(previews, w, w->windowTitle());
		previousWindow = w;
		connect(w, SIGNAL(destroyed()), SLOT(onChatwidgetDestroyed()));
	} else
		if (w)
			TaskbarPreviews::tabSetTitle(previousTabId, w->windowTitle());
}

void Win7Int2::onUnreadChanged(qutim_sdk_0_3::MessageList)
{
	QList<ChatSession*> sessions = ChatLayer::instance()->sessions();
	quint32 unreadConfs = 0, unreadChats = 0;
	foreach (ChatSession* s, sessions) {
		if (!s) { // that's strange, but happens // TODO: this block should dissapear sometimes
			qDebug() << "Zeros still appear in 'ChatLayer::instance()->sessions()'.";
			continue;
		}
		if(qobject_cast<Conference*>(s->getUnit()))
			unreadConfs  += s->unread().size();
		else
			unreadChats += s->unread().size();
	}
	previews->updateNumbers(unreadConfs, unreadChats);
	QPixmap icon    = generateOverlayIcon(unreadConfs, unreadChats);
	QWidget *window = chatWindow();
	if (window) {
		if (unreadChats + unreadConfs)
			OverlayIcon::set(window, icon);
		else
			OverlayIcon::clear(window);
		if (previousWindow != window)
			testTab();
		else
			TaskbarPreviews::tabPreviewsRefresh(previousTabId);
	} else {
		TaskbarPreviews::tabRemove(previousTabId);
		previousTabId = 0;
	}
	previousWindow = window;
}

QWidget *Win7Int2::chatWindow()
{
	QObject* obj    = qobject_cast<QObject*>(ServiceManager::getByName("ChatForm"));
	QWidget *widget = 0;
	bool metZero    = false;
	QWidgetList list;
	QMetaObject::invokeMethod(obj, "chatWidgets", Qt::DirectConnection, Q_RETURN_ARG(QWidgetList, list));
	if(!list.size())
		return 0; // empty list
	foreach (QWidget *w, list)
		if (w) {
			widget = w->window(); // some exists
			break;
		} else
			metZero = true;
	if (metZero) // TODO: this block should dissapear sometimes as well as variables used here
		qDebug() << "Zeros still appear in ChatForm's chatWidgets list.";
	return widget; // no nonzero values in list
}

QPixmap Win7Int2::generateOverlayIcon(quint32 unreadConfs, quint32 unreadChats)
{
	bool cfg_displayCount = true; // TODO
	bool cfg_addConfs     = false; // TODO
	quint32 count = unreadChats + (cfg_addConfs ? unreadConfs : 0);
	QPixmap icon;
	if(unreadConfs && !unreadChats)
		icon = Icon("mail-message")   .pixmap(16, 16);
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

void Win7Int2::onChatwidgetDestroyed()
{
	if (previousTabId)
		TaskbarPreviews::tabRemove(previousTabId);
	previousTabId = 0;
	testTab();
}

#define CHATUNREAD_X 10
#define CHATUNREAD_Y 10
#define CONFUNREAD_X 10
#define CONFUNREAD_Y 35
#define ICON_SIZE    64

WPreviews::WPreviews(Win7Int2 *parent)
{
	sceneBgImage  = QPixmap(":/res/def-bg.png");
	currentBgSize = sceneBgImage.size();
	grView        = new QGraphicsView;
	grView->setScene(new QGraphicsScene);
	sceneBgItem     = grView->scene()->addPixmap(sceneBgImage);
	qutimIconItem  = grView->scene()->addPixmap(qutim_sdk_0_3::Icon("qutim").pixmap(ICON_SIZE, ICON_SIZE));
	textUnreadChats = grView->scene()->addText("");
	textUnreadConfs = grView->scene()->addText("");
	grView->resize(200, 100);
	sceneBgItem->setPos(0, 0);
	textUnreadChats->setPos(CHATUNREAD_X, CHATUNREAD_Y);
	textUnreadConfs->setPos(CONFUNREAD_X, CONFUNREAD_Y);
	grView->setStyleSheet("QGraphicsView { border:0; margin:0; padding:0 }");
	this->parent = parent;
	updateNumbers(0, 0);
}

WPreviews::~WPreviews()
{
	delete grView;
}

void WPreviews::updateNumbers(unsigned confs, unsigned chats)
{
	this->unreadConfs = confs;
	this->unreadChats = chats;
}

QPixmap WPreviews::IconicPreview(unsigned, QWidget *, QSize size)
{
	grView->resize(size);
	qutimIconItem->setPos(size.width()-ICON_SIZE, size.height()-ICON_SIZE);
	if (unreadConfs || unreadChats) {
		textUnreadChats->setPlainText(tr("You have %n new message(s) from chats.",       "", unreadChats));
		textUnreadConfs->setPlainText(tr("You have %n new message(s) from conferences.", "", unreadConfs));
		textUnreadChats->setTextWidth(size.width() - CHATUNREAD_X*2);
		textUnreadConfs->setTextWidth(size.width() - CONFUNREAD_X*2);
	} else {
		textUnreadChats->setPlainText("");
		textUnreadConfs->setPlainText(tr("You have no new messages."));
	}
	if (currentBgSize != size)
		sceneBgItem->setPixmap(sceneBgImage.scaled(size, Qt::KeepAspectRatioByExpanding));
	return QPixmap::grabWidget(grView);
}

QPixmap WPreviews::LivePreview(unsigned, QWidget *owner)
{
	QPixmap px(owner->size());
	px.fill(QColor(0, 0, 0, 0)); // hack
	owner->render(&px); // TODO: make better rendering quality somehow
	return px;
}

QUTIM_EXPORT_PLUGIN(Win7Int2)
