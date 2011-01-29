#include "win7int.h"
#include "wsettings.h"
#include <QDir>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QImage>
#include <QImage>
#include <QMap>
#include <QPainter>
#include <QPainter>
#include <QTimer>
#include <QSysInfo>
#include <qutim/conference.h>
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <qutim/extensionicon.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qutim/settingslayer.h>
#include <qt_windows.h>
#include <WinThings/OverlayIcon.h>

using namespace qutim_sdk_0_3;

Win7Int2 *Win7Int2::pluginInstance;

Win7Int2::Win7Int2()
{
	pluginInstance = this;
	previousWindow = 0;
	previousTabId  = 0;
	previews       = 0;
	// being run from Explorer
	HWND console = FindWindowW(L"ConsoleWindowClass", QDir::toNativeSeparators(qApp->applicationFilePath()).toStdWString().data());
	if (!console) // being run from taskbar (pinned icon), assuming default link name (qutim); function is not case-sensitive
		console = FindWindowW(L"ConsoleWindowClass", L"qutim");
	if (console)
		ShowWindow(console, SW_HIDE);
}

void Win7Int2::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Vizir Ivan"),
		QT_TRANSLATE_NOOP("Task", "Author"),
		QLatin1String("define-true-false@yandex.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Windows 7 Integration"),
		QT_TRANSLATE_NOOP("Plugin", "Adds count of unread messages as an icon to taskbar button of qutim, along with some commands list."),
		PLUGIN_VERSION(1, 2, 3, 4),
		ExtensionIcon());
}

bool Win7Int2::load()
{
	if (QSysInfo::windowsVersion() != QSysInfo::WV_WINDOWS7)
		return false; // even if plugin suddenly loads on WinVista, it won't work
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	if (cfg_thumbsEnabled = Config(WI_CONFIG).value("tt_enabled", true)) // so cannot be changed in runtime
		previews = new WPreviews;
	QWidget *cl = ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>();
	TaskbarPreviews::setWindowAttributes(cl, TA_Peek_ExcludeFrom|TA_Flip3D_ExcludeBelow);
	settingsItem = new GeneralSettingsItem<WSettingsWidget>(
		Settings::Plugin,	QIcon(),
		QT_TRANSLATE_NOOP("Plugin", "Windows 7 Integration"));
	Settings::registerItem(settingsItem);
	reloadSettings();
	return true;
}

bool Win7Int2::unload()
{
	QWidget *cl = ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>();
	TaskbarPreviews::setWindowAttributes(cl, TA_NoAttributes);
	if (previews)
		delete previews;
	Settings::removeItem(settingsItem);
	return true;
}

void Win7Int2::onSessionCreated(qutim_sdk_0_3::ChatSession *s)
{
	if (previews)
		connect(s, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), this->previews, SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)), Qt::UniqueConnection);
	connect(s, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)), Qt::UniqueConnection);
	connect(s, SIGNAL(messageReceived(qutim_sdk_0_3::Message*)),  SLOT(onMessageSmthDid(qutim_sdk_0_3::Message*)));
	connect(s, SIGNAL(messageSent(qutim_sdk_0_3::Message*)),      SLOT(onMessageSmthDid(qutim_sdk_0_3::Message*)));
	connect(s, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
}

void Win7Int2::onSessionActivated(bool activated)
{
	if (activated)
		testTab();
	QWidget *w = chatWindow();
	if (w && cfg_thumbsEnabled) {
		TaskbarPreviews::tabSetTitle(previousTabId, w->windowTitle());
		TaskbarPreviews::tabPreviewsRefresh(previousTabId);
	}
}

void Win7Int2::testTab()
{
	if (!cfg_thumbsEnabled)
		return;
	QWidget *w = chatWindow();
	if (w != previousWindow && cfg_thumbsEnabled) {
		TaskbarPreviews::tabRemove(previousTabId);
		previousTabId = 0;
	}
	if (cfg_thumbsEnabled) {
		if (!previousTabId && w) {
			previousTabId  = TaskbarPreviews::tabAddVirtual(previews, w, w->windowTitle());
			previousWindow = w;
			connect(w, SIGNAL(destroyed()), SLOT(onChatwidgetDestroyed()));
		} else
			if (w)
				TaskbarPreviews::tabSetTitle(previousTabId, w->windowTitle());
	}
}

void Win7Int2::onUnreadChanged(qutim_sdk_0_3::MessageList)
{
	QList<ChatSession*> sessions = ChatLayer::instance()->sessions();
	quint32 unreadConfs = 0, unreadChats = 0;
	foreach (ChatSession* s, sessions) {
		if (!s) { // that's strange, but happens // TODO: this block should dissapear sometimes
			debug() << "Zeros still appear in 'ChatLayer::instance()->sessions()'.";
			continue;
		}
		ChatUnit *unit      = s->getUnit();
		unsigned unreadSize = s->unread().size();
		if (qobject_cast<Conference*>(unit))
			unreadConfs += unreadSize;
		else
			unreadChats += unreadSize;
	}
	if (cfg_thumbsEnabled)
		previews->updateNumbers(unreadConfs, unreadChats);
	QWidget *window = chatWindow();
	if (window) {
		if (cfg_overlayIconEnabled) {
			if (unreadChats || unreadConfs)
				OverlayIcon::set(window, generateOverlayIcon(unreadConfs, unreadChats));
			else
				OverlayIcon::clear(window);
		}
		if (cfg_thumbsEnabled) {
			if (previousWindow != window)
				testTab();
			else
				TaskbarPreviews::tabPreviewsRefresh(previousTabId);
		}
	} else {
		if (cfg_thumbsEnabled) {
			TaskbarPreviews::tabRemove(previousTabId);
			previousTabId = 0;
		}
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
		debug() << "Zeros still appear in ChatForm's chatWidgets list.";
	return widget;
}

QPixmap Win7Int2::generateOverlayIcon(quint32 unreadConfs, quint32 unreadChats)
{
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
		if (cfg_displayCount)
			painter.drawText(QRect(0, 1, 15, 15), Qt::AlignCenter, QString::number(count));
	}
	return icon;
}

void Win7Int2::onChatwidgetDestroyed()
{
	if (previousTabId && cfg_thumbsEnabled)
		TaskbarPreviews::tabRemove(previousTabId);
	previousTabId = 0;
	testTab();
}

void Win7Int2::onMessageSmthDid(qutim_sdk_0_3::Message *)
{
	if (cfg_thumbsEnabled)
		TaskbarPreviews::tabPreviewsRefresh(this->previousTabId);
}

void Win7Int2::reloadSettings()
{
	Config cfg(WI_CONFIG);
	cfg_displayCount = cfg.value("oi_showNewMsgCount",    true);
	cfg_addConfs     = cfg.value("oi_addNewConfMsgCount", false);
	cfg_overlayIconEnabled = cfg.value("oi_enabled",      true);
	if (cfg_thumbsEnabled)
		previews->reloadSettings();
	if (!cfg_overlayIconEnabled) {
		QWidget *w = chatWindow();
		if (w)
			OverlayIcon::clear(w);
	}
}

WPreviews::WPreviews(Win7Int2 *parent)
{
	lastChatWidget = 0;
	sceneBgImage   = QPixmap(":/res/def-bg.png");
	currentBgSize  = sceneBgImage.size();
	grView         = new QGraphicsView;
	grView->setScene(new QGraphicsScene);
	sceneBgItem    = grView->scene()->addPixmap(sceneBgImage);
	qutimIconItem  = grView->scene()->addPixmap(qutim_sdk_0_3::Icon("qutim").pixmap(ICON_SIZE, ICON_SIZE));
	qutimIconItem->setOpacity(0.25);

	textUnreadTitle        = grView->scene()->addText(QString());
	textUnreadChats        = grView->scene()->addText(QString());
	textUnreadConfs        = grView->scene()->addText(QString());
	textUnreadAuthorsTitle = grView->scene()->addText(QString());
	textUnreadAuthorsList  = grView->scene()->addText(QString());

	grView->resize(200, 100);
	sceneBgItem->           setPos(0, 0);
	textUnreadTitle->       setPos(UNREAD_TITLE_X,  UNREAD_TITLE_Y);
	textUnreadChats->       setPos(CHATUNREAD_X,    CHATUNREAD_Y);
	textUnreadConfs->       setPos(CONFUNREAD_X,    CONFUNREAD_Y);
	textUnreadAuthorsTitle->setPos(AUTHORS_TITLE_X, AUTHORS_TITLE_Y);
	textUnreadAuthorsList-> setPos(AUTHORS_LIST_X,  AUTHORS_LIST_Y);

	grView->setStyleSheet("QGraphicsView { border:0; margin:0; padding:0 }");
	grView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	grView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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

void WPreviews::onUnreadChanged(qutim_sdk_0_3::MessageList list)
{
	ChatSession *sess = list.isEmpty() ? 0 : ChatLayer::get(list.first().chatUnit(), false);
	int lastSendersCount = 3;
	int sessIndex = sessions.indexOf(sess);
	if (-1 == sessIndex) {
		if (sess) {
			sessions.push_front(sess);
			connect(sess, SIGNAL(destroyed(QObject*)), this, SLOT(onSessionDestroyed(QObject*)));
		}
	} else
		sessions.move(sessIndex, 0);
	QString result("");
	int listSize  = sessions.size(), index = 0;
	while (index < lastSendersCount && index < listSize && cfg_showSenders) {
		ChatUnit    *unit;
		ChatSession *session;
		unsigned     unread;
		QString      title;
		session = sessions.at(index++);
		unit    = session->unit();
		title   = unit->   title();
		unread  = session->unread().size();
		if (!unread)
			continue;
		else
			result += "<div>" + title + " (" + QString::number(unread) + ")</div>";
	}
	textUnreadAuthorsList->setHtml(result);
}

QPixmap WPreviews::IconicPreview(unsigned, QWidget *owner, QSize size)
{
	grView->resize(size);
	qutimIconItem->setPos(size.width()-ICON_SIZE, size.height()-ICON_SIZE);
	if (unreadConfs || unreadChats) {
		textUnreadTitle->setHtml     (tr("<b>You have new messages:</b>"));
		textUnreadTitle->setTextWidth(size.width() - UNREAD_TITLE_X*2);
		if (cfg_showMsgCount) {
			textUnreadChats->setHtml     (tr("&middot; %n message(s) from chats.",       "", unreadChats));
			textUnreadConfs->setHtml     (tr("&middot; %n message(s) from conferences.", "", unreadConfs));
			textUnreadChats->setTextWidth(size.width() - CHATUNREAD_X*2);
			textUnreadConfs->setTextWidth(size.width() - CONFUNREAD_X*2);
		} else {
			textUnreadConfs->setPlainText(QString()); // clear "You have no new messages."
		}
		if (cfg_showSenders) {
			textUnreadAuthorsTitle->setPlainText(tr("Last received from:"));
			textUnreadAuthorsTitle->setTextWidth(size.width() - AUTHORS_TITLE_X*2);
			textUnreadAuthorsList-> setTextWidth(size.width() - AUTHORS_LIST_X *2);
		}
	} else {
		if (cfg_showMsgCount || cfg_showSenders)
			textUnreadConfs->setPlainText(tr("You have no new messages."));
		else
			textUnreadConfs->setPlainText(QString());
		textUnreadTitle->       setPlainText(QString());
		textUnreadChats->       setPlainText(QString());
		textUnreadAuthorsTitle->setPlainText(QString());
		textUnreadAuthorsList-> setPlainText(QString());
	}
	if (currentBgSize != size)
		sceneBgItem->setPixmap(sceneBgImage.scaled(size, Qt::KeepAspectRatioByExpanding));
	lastChatWidget = owner;
	QTimer::singleShot(50, this, SLOT(prepareLivePreview()));
	return QPixmap::grabWidget(grView);
}

QPixmap WPreviews::LivePreview(unsigned, QWidget *owner)
{
	if (livePreview.isNull() || !lastChatWidget || lastChatWidget->size() != lastWidgetSize) {
		lastWidgetSize = lastChatWidget->size();
		lastChatWidget = owner;
		prepareLivePreview();
	}
	return livePreview;
}

void WPreviews::prepareLivePreview()
{
	if (!lastChatWidget)
		return;
	livePreview = QPixmap(lastChatWidget->size());
	livePreview.fill(QColor(0, 0, 0, 0)); // hack
	lastChatWidget->render(&livePreview); // TODO: make better rendering quality somehow (just look on semitransparent icons)…
}

void WPreviews::onSessionDestroyed(QObject *s)
{
	sessions.removeAll(static_cast<ChatSession*>(s));
}

void WPreviews::reloadSettings()
{
	Config cfg(WI_CONFIG);
	cfg_showMsgCount = cfg.value("tt_showNewMsgCount", true);
	cfg_showSenders  = cfg.value("tt_showLastSenders", true);
	if (!cfg_showSenders)
		textUnreadAuthorsList-> setPlainText(QString());
		textUnreadAuthorsTitle->setPlainText(QString());
	if (!cfg_showMsgCount) {
		textUnreadChats->setPlainText(QString());
		textUnreadConfs->setPlainText(QString());
	}
	if (!cfg_showMsgCount && !cfg_showSenders)
		textUnreadTitle->setPlainText(QString());
}

QUTIM_EXPORT_PLUGIN(Win7Int2)
