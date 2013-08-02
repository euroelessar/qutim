/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "unreadmessageskeeper.h"
#include <qutim/debug.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/history.h>

namespace UnreadMessagesKeeper
{

void UnreadMessagesKeeper::init()
{
	qDebug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "UnreadMessagesKeeper"),
			QT_TRANSLATE_NOOP("Plugin", "Allows you to save a list of unread messages when qutIM exit"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("sauron"));
}

bool UnreadMessagesKeeper::load()
{
	ChatLayer *layer = ChatLayer::instance();
	connect(layer,SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(sessionCreated(qutim_sdk_0_3::ChatSession*))
			);

	Config cfg("unreadmessages");
	foreach (QString id,cfg.childGroups()) {
		Protocol *p =  Protocol::all().value(id);
		if(!p)
			continue;
		cfg.beginGroup(id);
		foreach (id,cfg.childGroups()) {
			Account *a = p->account(id);
			if(!a)
				continue;
			cfg.beginGroup(id);
			foreach(id,cfg.childKeys()) {
				ChatUnit *u = a->unit(id,false);
				if(!u)
					continue;
				int count = cfg.value(id,0);
				if(count) {
					ChatSession *s = ChatLayer::get(u,true);
					MessageList list = History::instance()->read(u,count);
					foreach(Message m,list) {
						m.setProperty("store",false);
						m.setProperty("fake",true); //mega spike
						s->appendMessage(m);
					}
				}
			}
			cfg.endGroup();
		}
		cfg.endGroup();
	}
	return true;
}

bool UnreadMessagesKeeper::unload()
{
	return true;
}

void UnreadMessagesKeeper::sessionCreated(qutim_sdk_0_3::ChatSession* session)
{
	connect(session,SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
}

void UnreadMessagesKeeper::onUnreadChanged(const qutim_sdk_0_3::MessageList &list)
{
	ChatSession *s = qobject_cast<ChatSession*>(sender());
	ChatUnit *u = s->getUnit();
	Account *a = u->account();
	Config cfg("unreadmessages");

	cfg.beginGroup(a->protocol()->id());
	cfg.beginGroup(a->id());

	cfg.setValue(u->id(),list.count());

	cfg.endGroup();
	cfg.endGroup();
}

}

QUTIM_EXPORT_PLUGIN(UnreadMessagesKeeper::UnreadMessagesKeeper);

