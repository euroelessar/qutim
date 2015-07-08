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

#ifndef THUMBNAILSRENDERER_H
#define THUMBNAILSRENDERER_H

#include <WinThings/TaskbarPreviews.h>
#include <QGraphicsView>
#include <QList>
#include <qutim/chatsession.h>

namespace qutim_sdk_0_3
{
	class ChatSession;
}

class WThumbnails;

class WThumbnailsProvider : public PreviewProvider
{
	Q_OBJECT

	typedef QList<qutim_sdk_0_3::ChatSession *> SessionsList;

	QGraphicsPixmapItem *qutimIconItem;
	QGraphicsPixmapItem *sceneBgItem;
	QGraphicsTextItem   *textUnreadChats;
	QGraphicsTextItem   *textUnreadConfs;
	QGraphicsTextItem   *textUnreadTitle;
	QGraphicsTextItem   *textUnreadAuthorsTitle;
	QGraphicsTextItem   *textUnreadAuthorsList;
	QGraphicsView *grView;
	QPixmap  sceneBgImage;
	QPixmap  livePreview;
	QSize    currentBgSize;
	SessionsList sessions;
	unsigned unreadChats, unreadConfs;
	WThumbnails *parentThumbs;

	bool cfg_showMsgCount;

public:
	WThumbnailsProvider(WThumbnails *parent);
	~WThumbnailsProvider();
	QPixmap IconicPreview(unsigned tabid, QWidget *owner, QSize);
	QPixmap LivePreview  (unsigned tabid, QWidget *owner);
	void updateNumbers   (unsigned confs, unsigned chats);

public slots:
	void reloadSettings();
	void onUnreadChanged(unsigned chats, unsigned confs);
	void onUnreadChanged(qutim_sdk_0_3::MessageList);
	void onSessionDestroyed(QObject *);
	void prepareLivePreview();
};

#define UNREAD_TITLE_X  10
#define UNREAD_TITLE_Y  5
#define CHATUNREAD_X    5
#define CHATUNREAD_Y    18
#define CONFUNREAD_X    5
#define CONFUNREAD_Y    31
#define AUTHORS_TITLE_X 10
#define AUTHORS_TITLE_Y 49
#define AUTHORS_LIST_X  10
#define AUTHORS_LIST_Y  62
#define ICON_SIZE       64

#endif // THUMBNAILSRENDERER_H

