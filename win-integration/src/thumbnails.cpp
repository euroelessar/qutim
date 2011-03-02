#include "thumbnails.h"
#include "win7int.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <qutim/configbase.h>

using namespace qutim_sdk_0_3;

WPreviews::WPreviews(Win7Int2 *parent)
{
	lastChatWidget = 0;
	sceneBgImage   = QPixmap(":/res/def-bg.png");
	currentBgSize  = sceneBgImage.size();
	grView         = new QGraphicsView;
	grView->setScene(new QGraphicsScene);
	sceneBgItem    = grView->scene()->addPixmap(sceneBgImage);
	//qutimIconItem  = grView->scene()->addPixmap(qutim_sdk_0_3::Icon("qutim").pixmap(ICON_SIZE, ICON_SIZE));
	//qutimIconItem->setOpacity(0.25);

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
	//qutimIconItem->setPos(size.width()-ICON_SIZE, size.height()-ICON_SIZE);
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
