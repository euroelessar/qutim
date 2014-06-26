/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ivan Vizir <define-true-false@yandex.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "thumbnails.h"
#include "thumbnails-renderer.h"
#include "../../src/winint.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <QSettings>
#include <qutim/qtwin.h>

using namespace qutim_sdk_0_3;

QColor GetUserSelectedAeroColor()
{
	const QColor defaultColor(170, 15, 30);
	if (!QtWin::isCompositionEnabled())
		return defaultColor;
	bool ok = false;
	quint32 color = QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\DWM", QSettings::NativeFormat)
			.value("ColorizationColor").toUInt(&ok);
	return ok ? QColor(color) : defaultColor;
}

WThumbnailsProvider::WThumbnailsProvider(WThumbnails *parent)
{
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
	grView->setVerticalScrollBarPolicy  (Qt::ScrollBarAlwaysOff);
	parentThumbs = parent;
	onUnreadChanged(0, 0);
}

WThumbnailsProvider::~WThumbnailsProvider()
{
	delete grView;
}

void WThumbnailsProvider::onUnreadChanged(unsigned chats, unsigned confs)
{
	this->unreadChats = chats;
	this->unreadConfs = confs;
}

void WThumbnailsProvider::onUnreadChanged(qutim_sdk_0_3::MessageList list)
{
	ChatSession *sess = list.isEmpty() ? 0 : ChatLayer::get(list.first().chatUnit(), false);
	int lastSendersCount = 3;
	int sessIndex = sessions.indexOf(sess);
	if (-1 == sessIndex) {
		if (sess) {
			sessions.push_front(sess);
			connect(sess, SIGNAL(destroyed(QObject*)), SLOT(onSessionDestroyed(QObject*)));
		}
	} else
		sessions.move(sessIndex, 0);
	QString result("");
	int listSize  = sessions.size(), index = 0;
	while (index < lastSendersCount && index < listSize) {
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

QPixmap WThumbnailsProvider::IconicPreview(unsigned, QWidget *, QSize size)
{
	sceneBgItem->setPixmap(sceneBgImage.scaled(size, Qt::KeepAspectRatio));
	grView->resize(sceneBgItem->pixmap().size());
	//qutimIconItem->setPos(size.width()-ICON_SIZE, size.height()-ICON_SIZE);
	if (unreadConfs || unreadChats) {
		textUnreadTitle->setHtml     (tr("<b>You have new messages.</b>"));
		textUnreadTitle->setTextWidth(size.width() - UNREAD_TITLE_X*2);
		if (cfg_showMsgCount) {
			textUnreadChats->setHtml     (tr("&middot; %n message(s) from chats.",       "", unreadChats));
			textUnreadConfs->setHtml     (tr("&middot; %n message(s) from conferences.", "", unreadConfs));
			textUnreadChats->setTextWidth(size.width() - CHATUNREAD_X*2);
			textUnreadConfs->setTextWidth(size.width() - CONFUNREAD_X*2);
		} else {
			textUnreadConfs->setPlainText(QString()); // clear "You have no new messages."
		}
		textUnreadAuthorsTitle->setPlainText(tr("Last received from:"));
		textUnreadAuthorsTitle->setTextWidth(size.width() - AUTHORS_TITLE_X*2);
		textUnreadAuthorsList-> setTextWidth(size.width() - AUTHORS_LIST_X *2);
	} else {
		textUnreadConfs->setPlainText(tr("You have no new messages."));
		textUnreadTitle->       setPlainText(QString());
		textUnreadChats->       setPlainText(QString());
		textUnreadAuthorsTitle->setPlainText(QString());
		textUnreadAuthorsList-> setPlainText(QString());
	}
	grView->setBackgroundBrush(QBrush(GetUserSelectedAeroColor()));
	QTimer::singleShot(0, this, SLOT(prepareLivePreview()));
	return QPixmap::grabWidget(grView);
}

QPixmap WThumbnailsProvider::LivePreview(unsigned, QWidget *)
{
	return livePreview;
}

void WThumbnailsProvider::prepareLivePreview()
{
	if (!parentThumbs->currentWindow())
		return;
	livePreview = QPixmap(parentThumbs->currentWindow()->size());
	livePreview.fill(QColor(0, 0, 0, 0)); // hack
	parentThumbs->currentWindow()->render(&livePreview); // TODO: make better rendering quality somehow (just look on semitransparent icons)…
}

void WThumbnailsProvider::onSessionDestroyed(QObject *s)
{
	sessions.removeAll(static_cast<ChatSession*>(s));
}

void WThumbnailsProvider::reloadSettings()
{
	Config cfg(WI_ConfigName);
	cfg_showMsgCount = cfg.value("tt_showNewMsgNumber", true);
	if (!cfg_showMsgCount) {
		textUnreadChats->setPlainText(QString());
		textUnreadConfs->setPlainText(QString());
	}
}

