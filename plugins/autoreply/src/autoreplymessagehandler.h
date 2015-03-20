#ifndef AUTOREPLYMESSAGEHANDLER_H
#define AUTOREPLYMESSAGEHANDLER_H

#include <qutim/messagehandler.h>
#include <QDateTime>

class AutoReplyPlugin;

class AutoReplyMessageHandler : public qutim_sdk_0_3::MessageHandler
{
public:
    AutoReplyMessageHandler(AutoReplyPlugin *plugin);

    static QString fuzzyTimeDelta(const QDateTime &from, const QDateTime &to);
    static void updateText(QString &text, const QDateTime &backTime);
	qutim_sdk_0_3::MessageHandlerAsyncResult doHandle(qutim_sdk_0_3::Message &message) override;

private:
	struct CacheItem {
		QDateTime time;
		qutim_sdk_0_3::ChatUnit *unit;
	};
	QList<CacheItem> m_cache;
    AutoReplyPlugin *m_plugin;
};

#endif // AUTOREPLYMESSAGEHANDLER_H
