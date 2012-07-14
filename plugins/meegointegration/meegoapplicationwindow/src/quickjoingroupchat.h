/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef QUICKJOINGROUPCHAT_H
#define QUICKJOINGROUPCHAT_H
#include <QObject>
#include <qutim/actiongenerator.h>

namespace MeegoIntegration
{

	class JoinGroupChatWrapper;
	using namespace qutim_sdk_0_3;
	
	class JoinGroupChatGenerator : public ActionGenerator
	{
	public:
		JoinGroupChatGenerator(QObject *module);
		void showImpl(QAction *action, QObject *obj);
	};	
	
	class QuickJoinGroupChat : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "JoinGroupChat")
		Q_CLASSINFO("Uses", "IconLoader")
		Q_CLASSINFO("Uses", "ContactList")
	public:
		explicit QuickJoinGroupChat();
		virtual ~QuickJoinGroupChat();
	public slots:
		void onJoinGroupChatTriggered();
	private:
		QScopedPointer<ActionGenerator> m_button;
		QScopedPointer<JoinGroupChatWrapper> m_chat;
	};
	
}
#endif // QUICKJOINGROUPCHAT_H

