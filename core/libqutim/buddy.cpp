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
#include "buddy_p.h"
#include "tooltip.h"
#include "account.h"
#include <QTextDocument>
#include <QStringBuilder>
#include "iconloader.h"

namespace qutim_sdk_0_3
{
Buddy::Buddy(Account *account) :
	ChatUnit(*new BuddyPrivate(this), account)
{
	connect(this, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(_q_status_changed(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

Buddy::Buddy(BuddyPrivate &d, Account *account) :
	ChatUnit(d, account)
{
	connect(this, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(_q_status_changed(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

Buddy::~Buddy()
{
}

QString Buddy::avatar() const
{
	return QString();
}

QString Buddy::title() const
{
	QString n = name();
	return n.isEmpty() ? id() : n;
}

QString Buddy::name() const
{
	return id();
}

Status Buddy::status() const
{
	return Status::Online;
}

void Buddy::setName(const QString &name)
{
	Q_UNUSED(name);
}

bool Buddy::event(QEvent *ev)
{
	if (ev->type() == ToolTipEvent::eventType()) {
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
		if (event->generateLayout()) {
			event->addHtml("<table><tr><td>", 98);
			QString ava = avatar();
			if (ava.isEmpty())
				ava = IconLoader::iconPath(QLatin1String("qutim"), 64);
			QString text = QLatin1Literal("</td><td><img width=\"64\" src=\"")
					% ava.toHtmlEscaped()
					% QLatin1Literal("\"/></td></tr></table>");
			event->addHtml(text, 5);
		}
		event->addHtml("<font size=-1>", 50);
		QString text = QLatin1Literal("<b>")
                % name().toHtmlEscaped()
				% QLatin1Literal("</b> &lt;")
                % id().toHtmlEscaped()
				% QLatin1Literal("&gt;");
		event->addHtml("</font>", 10);
		event->addHtml(text, 90);
		event->addField(QT_TRANSLATE_NOOP("ToolTip", "Account"), account()->id(), 90);

		QHash<QString, QVariantHash> extStatuses = status().extendedInfos();
		foreach (const QVariantHash &extStatus, extStatuses) {
			if (extStatus.value("showInTooltip", false).toBool()) {
				int priority = extStatus.value("priorityInTooltip").toInt();
				if (priority < 10 || priority > 90)
					priority = 50;
				event->addField(extStatus.value("title").toString(),
								extStatus.value("description").toString(),
								extStatus.value("icon").value<ExtensionIcon>(),
								extStatus.value("iconPosition").value<ToolTipEvent::IconPosition>(),
								priority);
			}
		}
	}
	return ChatUnit::event(ev);
}

void BuddyPrivate::_q_status_changed(const Status &now, const Status &old)
{
	Q_Q(Buddy);
	if(now.type() != Status::Offline && old.type() == Status::Offline)
		 q->setChatState(ChatUnit::ChatStateInActive);
	else if(now.type() == Status::Offline)
		q->setChatState(ChatUnit::ChatStateGone);
}

//void Buddy::setStatus(const Status &status)
//{
//	Status old = this->status();
//	if(old.type() != status.type())
//		emit statusChanged(status,old);
//	onStatusChanged(status,this->status());
//}

}

#include "moc_buddy.cpp"

