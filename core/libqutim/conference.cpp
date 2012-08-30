/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
	ConferencePrivate(Conference *c) : ChatUnitPrivate(c), isJoined(false) {}
	bool isJoined;

	void setJoined(bool set)
	{
		isJoined = set;
		emit static_cast<Conference*>(q_func())->joinedChanged(set);
	}
};

Conference::Conference(Account *account) :
	ChatUnit(*new ConferencePrivate(this), account)
{
}

Conference::~Conference()
{
}

QString Conference::topic() const
{
	return QString();
}

void Conference::setJoined(bool set)
{
	Q_D(Conference);
	if (d->isJoined == set)
		return;
	d->isJoined = set;
	emit joinedChanged(set);

	emit (this->*(set ? &Conference::joined : &Conference::left))();
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

void Conference::join()
{
	if (!d_func()->isJoined)
		doJoin();
}

void Conference::leave()
{
	if (d_func()->isJoined)
		doLeave();

	//TODO set offline all participants
}

}

#include "moc_conference.cpp"

