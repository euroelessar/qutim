#ifndef WOVERLAYICON_H
#define WOVERLAYICON_H

#include <QObject>

class WOverlayIcon : public QObject
{
	Q_OBJECT

public:
	WOverlayIcon();

public slots:
	void onUnreadChanged(unsigned chats, unsigned confs);
	void reloadSettings();

private:
	bool cfg_displayNumber;
	bool cfg_addConfs;
	bool cfg_enabled;
};

#endif // WOVERLAYICON_H
