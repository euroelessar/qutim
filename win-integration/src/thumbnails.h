#ifndef THUMBNAILS_H
#define THUMBNAILS_H

#include <WinThings/TaskbarPreviews.h>
#include <QGraphicsView>
#include <QList>
#include <qutim/messagesession.h>

namespace qutim_sdk_0_3
{
	class ChatSession;
}

class Win7Int2;

class WPreviews : public PreviewProvider
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
	QSize    lastWidgetSize;
	QWidget *lastChatWidget;
	SessionsList sessions;
	unsigned unreadChats, unreadConfs;
	Win7Int2 *parent;

	bool cfg_showSenders;
	bool cfg_showMsgCount;

public:
	WPreviews(Win7Int2 *parent = 0);
	~WPreviews();
	QPixmap IconicPreview(unsigned tabid, QWidget *owner, QSize);
	QPixmap LivePreview  (unsigned tabid, QWidget *owner);
	void updateNumbers   (unsigned confs, unsigned chats);
	void reloadSettings();

public slots:
	void onUnreadChanged(qutim_sdk_0_3::MessageList);
	void onSessionDestroyed(QObject *);
	void prepareLivePreview();
};

#endif THUMBNAILS_H
