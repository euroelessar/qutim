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

#ifndef ABSTRACTCHATWIDGET_H
#define ABSTRACTCHATWIDGET_H

#include <QMainWindow>
#include <QTimer>
#include <QTextCursor>
#include "../chatsessionimpl.h"
#include "../chatlayerimpl.h"
#include <qutim/actiongenerator.h>
#include "chatlayer_global.h"

namespace qutim_sdk_0_3 {
}

class QTextEdit;
class QListView;
class QModelIndex;
namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

class ADIUMCHAT_EXPORT AbstractChatWidget : public QMainWindow
{
	Q_OBJECT
public:
	enum Attribute {
		UseCustomIcon = 0x01
	};
	Q_DECLARE_FLAGS(Attributes, Attribute)

	AbstractChatWidget(QWidget *parent = 0);
	virtual void addAction(ActionGenerator *gen) = 0;
	void addActions(const QList<ActionGenerator*> &actions);
	virtual QTextEdit *getInputField() const = 0;
	virtual bool contains(ChatSessionImpl *session) const = 0;
	virtual ~AbstractChatWidget() {}
	virtual ChatSessionImpl *currentSession() const = 0;
	static QString titleForSession(ChatSessionImpl *s);
	virtual void setView(QWidget *) {}

public slots:
	virtual void addSession(ChatSessionImpl *session) = 0;
	void addSessions(const ChatSessionList &sessions);
	virtual void removeSession(ChatSessionImpl *session) = 0;
	virtual void activate(ChatSessionImpl* session) = 0;
	virtual void loadSettings() = 0;

signals:
	void currentSessionChanged(ChatSessionImpl *now,ChatSessionImpl *before);

protected:
	virtual void setTitle(ChatSessionImpl *s);
	
	Attributes m_attributes;
};

}
}
#endif // ABSTRACTCHATWIDGET_H

