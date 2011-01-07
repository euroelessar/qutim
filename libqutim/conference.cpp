#include "conference.h"
#include "chatunit_p.h"
#include "tooltip.h"
#include <QTextDocument>
#include <QLatin1Literal>

namespace qutim_sdk_0_3
{

class ConferencePrivate : public ChatUnitPrivate
{
public:
	ConferencePrivate(Conference *c) : ChatUnitPrivate(c) {}
	bool isJoined;
	void _q_joined()
	{
		isJoined = true;
		emit static_cast<Conference*>(q_func())->joinedChanged(true);
	}
	void _q_leaved()
	{
		isJoined = false;
		emit static_cast<Conference*>(q_func())->joinedChanged(false);
	}
};

Conference::Conference(Account *account) :
	ChatUnit(*new ConferencePrivate(this), account)
{
	connect(this, SIGNAL(joined()),
			this, SLOT(_q_joined()));
	connect(this, SIGNAL(left()),
			this, SLOT(_q_leaved()));
}

Conference::~Conference()
{
}

QString Conference::topic() const
{
	return QString();
}

void Conference::setJoined(bool isJoined)
{
	Q_D(Conference);
	if (d->isJoined == isJoined) return;
	d->isJoined = isJoined;
	emit joinedChanged(isJoined);
	emit (isJoined ? joined : left)();
	d->isJoined = isJoined;
}

void Conference::setTopic(const QString &topic)
{
	Q_UNUSED(topic);
}

void Conference::invite(qutim_sdk_0_3::Contact *contact, const QString &reason)
{
	Q_UNUSED(contact);
	Q_UNUSED(reason);
}

bool Conference::event(QEvent *ev)
{
	if (ev->type() == ToolTipEvent::eventType()) {
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
		QString text = QLatin1Literal("<p><strong>")
				% Qt::escape(title())
				% QLatin1Literal("</strong> &lt;")
				% id()
				% QLatin1Literal("&gt;</p>")
				% QLatin1Literal("<p>")
				% Qt::escape(topic())
				% QLatin1Literal("</p>");
		event->addHtml(text, 90);
	}
	return ChatUnit::event(ev);
}

bool Conference::isJoined() const
{
	return d_func()->isJoined;
}

}

#include "conference.moc"
