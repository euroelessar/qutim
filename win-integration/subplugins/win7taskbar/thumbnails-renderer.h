#ifndef THUMBNAILSRENDERER_H
#define THUMBNAILSRENDERER_H

#include <WinThings/TaskbarPreviews.h>
#include <QGraphicsView>
#include <QList>
#include <qutim/messagesession.h>

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
