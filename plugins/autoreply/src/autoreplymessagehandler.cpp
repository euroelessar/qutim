#include "autoreplymessagehandler.h"
#include "autoreplyplugin.h"
#include <qutim/chatunit.h>
#include <qutim/chatsession.h>
#include <qutim/conference.h>
#include <QCoreApplication>
#include <QDebug>

using namespace qutim_sdk_0_3;

struct AutoReplyMessageEvent : public QEvent
{
public:
	AutoReplyMessageEvent(const Message &message)
		: QEvent(eventType()), m_message(message)
	{
	}

	~AutoReplyMessageEvent()
	{
		ChatLayer::get(m_message.chatUnit())->append(m_message);
	}

	static Type eventType()
	{
		static int type = QEvent::registerEventType();
		return static_cast<Type>(type);
	}

private:
	Message m_message;
};

AutoReplyMessageHandler::AutoReplyMessageHandler(AutoReplyPlugin *plugin)
    : m_plugin(plugin)
{
}

static void appendWord(QString &str, int number, const char *word)
{
    if (number > 0) {
        str += QString::number(number);
        str += QLatin1Char(' ');
        str += QLatin1String(word);
        str += QLatin1String(number > 1 ? "s " : " ");
    }
}

QString AutoReplyMessageHandler::fuzzyTimeDelta(const QDateTime &from, const QDateTime &to)
{
    int totalDelta = from.secsTo(to);
    bool negative = totalDelta < 0;

	if (negative)
		return QString();

    totalDelta = qAbs(totalDelta);
    const int seconds = totalDelta % 60;
    totalDelta /= 60;
    const int minutes = totalDelta % 60;
    totalDelta /= 60;
    const int hours = totalDelta % 24;
    totalDelta /= 24;
    const int days = totalDelta;
    const int numbers[] = {
        days,
        hours,
        minutes,
        seconds
    };
    const char *words[] = {
        "day",
        "hour",
        "minute",
        "second"
    };
    QString timeString;
    for (int i = 0; i < 4; ++i) {
        if (numbers[i] > 0) {
            appendWord(timeString, numbers[i], words[i]);
            if (i + 1 < 4)
                appendWord(timeString, numbers[i + 1], words[i + 1]);
            break;
        }
    }
    if (timeString.isEmpty())
        timeString = QLatin1String("right now");
    else if (negative)
        timeString += QLatin1String("ago");
    else
        timeString.prepend(QLatin1String("in "));
    while (timeString.endsWith(QLatin1Char(' ')))
        timeString.chop(1);
	return QString("I'll be back %1").arg(timeString);
}

void AutoReplyMessageHandler::updateText(QString &text, const QDateTime &backTime)
{
	if (backTime.isValid()) {
		const QString timeString = fuzzyTimeDelta(QDateTime::currentDateTime(), backTime);
		if (!timeString.isEmpty())
			text.append(QLatin1Char('\n')).append(timeString);
	}
}

void AutoReplyMessageHandler::doHandle(Message &message, const Handler &handler)
{
	if (message.property("service", false)
			|| message.property("autoreply", false)
			|| qobject_cast<Conference*>(message.chatUnit())) {
        handler(Accept, QString());
		return;
	}
	QMutableListIterator<CacheItem> it(m_cache);
	while (it.hasNext()) {
		CacheItem &item = it.next();
        if (item.time.secsTo(QDateTime::currentDateTime()) > m_plugin->deltaTime()) {
			it.remove();
		} else if (item.unit == message.chatUnit()) {
            handler(Accept, QString());
            return;
        }
	}
    if (message.isIncoming() && m_plugin->isActive() && !m_plugin->replyText().isEmpty()) {
        QString replyText = m_plugin->replyText();
        updateText(replyText, m_plugin->backTime());
        Message replyMessage(replyText);
        replyMessage.setIncoming(false);
        replyMessage.setChatUnit(message.chatUnit());
		replyMessage.setProperty("autoreply", true);
		CacheItem cacheItem = { QDateTime::currentDateTime(), message.chatUnit() };
		m_cache << cacheItem;
		qApp->postEvent(m_plugin, new AutoReplyMessageEvent(replyMessage));
    }
    handler(Accept, QString());
}
