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

#ifndef ABSTRACTCHATFORM_H
#define ABSTRACTCHATFORM_H

#include <QWidget>
#include <QHash>
#include <QPointer>
#include "chatlayer_global.h"

namespace qutim_sdk_0_3 {
class ChatSession;
class ActionGenerator;
}

namespace Core
{
namespace AdiumChat
{
class AbstractChatWidget;
class ChatSessionImpl;

class ADIUMCHAT_EXPORT AbstractChatForm : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ChatForm")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "ChatViewFactory")
public:
	explicit AbstractChatForm();
	~AbstractChatForm();
	Q_INVOKABLE QObject *textEdit(qutim_sdk_0_3::ChatSession *session);
	Q_INVOKABLE QWidgetList chatWidgets();
	Q_INVOKABLE QWidget *chatWidget(qutim_sdk_0_3::ChatSession *session) const;
	Q_INVOKABLE void addAction(qutim_sdk_0_3::ActionGenerator *gen);
	Q_INVOKABLE void removeAction(qutim_sdk_0_3::ActionGenerator *gen);
protected:
	virtual AbstractChatWidget *createWidget(const QString &key) = 0;
	Q_INVOKABLE AbstractChatWidget *widget(const QString &key);
signals:
	void widgetCreated(QWidget *widget);
private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession*);
	void onChatWidgetDestroyed(QObject *object);
	void onSessionActivated(bool active);
	void onSettingsChanged();
	void onServiceChanged(QObject *newObject, QObject *oldObject);
private:
	QHash<QString, AbstractChatWidget*> m_chatWidgets;
	QString getWidgetId(ChatSessionImpl *sess) const;
	AbstractChatWidget *findWidget(qutim_sdk_0_3::ChatSession *sess) const;
	QList<qutim_sdk_0_3::ActionGenerator*> m_actions;
};
}
}

#endif // ABSTRACTCHATFORM_H

