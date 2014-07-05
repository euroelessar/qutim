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

#include "abstractchatwidget.h"
#include <qutim/icon.h>
#include <qutim/conference.h>
#include <QAbstractItemModel>
#include <qutim/avatarfilter.h>
#include "chatsessionimpl.h"
#include "chatlayerimpl.h"

#include <QMenuBar>

namespace Core
{
namespace AdiumChat
{

AbstractChatWidget::AbstractChatWidget(QWidget *parent) :
	QMainWindow(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	m_attributes |= UseCustomIcon;

	m_bar = new QMenuBar(this);
}

void AbstractChatWidget::addSessions(const ChatSessionList &sessions)
{
	foreach(ChatSessionImpl *s,sessions)
		addSession(s);
}

void AbstractChatWidget::addActions(const QList<ActionGenerator *> &actions)
{
	foreach(ActionGenerator *gen,actions)
		addAction(gen);
}

void AbstractChatWidget::setTitle(ChatSessionImpl *s)
{
	ChatUnit *u = s->getUnit();
	const bool customIcon = (m_attributes & UseCustomIcon);
	QIcon icon;
	if (customIcon)
		icon = Icon("view-choose");
	QString title;
	if(s->unread().count())
		title = tr("Chat with %1 (have %2 unread messages)").arg(u->title()).arg(s->unread().count());
	else
		title = tr("Chat with %1").arg(u->title());
	if (Conference *c = qobject_cast<Conference *>(u)) {
		if (customIcon)
			icon = Icon("meeting-attending"); //TODO
		title = tr("Conference %1 (%2)").arg(c->title(),c->id());
	} else if (customIcon) {
		if (Buddy *b = qobject_cast<Buddy*>(u)) {
			if (b->avatar().isEmpty())
				icon = Icon("view-choose");
			else {
				QIcon overlay = b->status().icon();
				icon = AvatarFilter::icon(b->avatar(), overlay);
			}
		}
	}

	window()->setWindowTitle(tr("%1 - qutIM").arg(title));
	window()->setWindowIcon(icon);
}

QString AbstractChatWidget::titleForSession(ChatSessionImpl *s)
{
	ChatUnit *u = s->getUnit();
	QString title;

	if(s->unread().count())
		title = tr("Chat with %1 (have %2 unread messages)").arg(u->title()).arg(s->unread().count());
	else
		title = tr("Chat with %1").arg(u->title());

	if (Conference *c = qobject_cast<Conference *>(u))
		title = tr("Conference %1 (%2)").arg(c->title(), c->id());

	return title;
}

QMenuBar* AbstractChatWidget::getMenuBar()
{
	return m_bar;
}

}
}

