#ifndef THUMBNAILS_H
#define THUMBNAILS_H

#include <QObject>

namespace qutim_sdk_0_3
{
	class ChatSession;
}

class WThumbnailsProvider;

class WThumbnails : public QObject
{
	Q_OBJECT
public:
	WThumbnails();
	~WThumbnails();
	QWidget *currentWindow();

public slots:
	void onChatwindowDestruction(QObject*);
	void onSessionActivated(bool);
	void onSessionCreated(qutim_sdk_0_3::ChatSession *);
	void onUnreadChanged(unsigned, unsigned);

private:
	QWidget *chatWindow;
	unsigned tabId;
	WThumbnailsProvider *pp;
};

#endif
