#ifndef WIN7INT_H
#define WIN7INT_H

#include <qutim/plugin.h>
#include <qutim/messagesession.h>
#include <WinThings/TaskbarPreviews.h>
#include <QGraphicsView>

class WPreviews;

class Win7Int2 : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Dock")
	Q_CLASSINFO("Uses", "ChatLayer")

	QPixmap generateOverlayIcon(quint32 unreadConfs, quint32 unreadChats);
	QWidget   *chatWindow();
	void       testTab();
	QWidget   *previousWindow;
	unsigned   previousTabId;
	WPreviews *previews;


public:
	Win7Int2();
	void init();
	bool load();
	bool unload();

private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession*);
	void onSessionActivated(bool);
	void onUnreadChanged(qutim_sdk_0_3::MessageList);
	void onChatwidgetDestroyed();

private:
};

class WPreviews : public PreviewProvider
{
	Q_OBJECT

	QGraphicsView *grView;
	QGraphicsTextItem   *textUnreadChats;
	QGraphicsTextItem   *textUnreadConfs;
	QGraphicsPixmapItem *sceneBgItem;
	QGraphicsPixmapItem *qutimIconItem;
	QPixmap sceneBgImage;
	QSize   currentBgSize;
	unsigned unreadChats, unreadConfs;
	Win7Int2 *parent;

public:
	WPreviews(Win7Int2 *parent = 0);
	~WPreviews();
	QPixmap IconicPreview(unsigned tabid, QWidget *owner, QSize);
	QPixmap LivePreview  (unsigned tabid, QWidget *owner);
	void updateNumbers(unsigned confs, unsigned chats);
};

#endif // WIN7INT_H
