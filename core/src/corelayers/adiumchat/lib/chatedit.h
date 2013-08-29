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
#ifndef CHATEDIT_H
#define CHATEDIT_H

#include <QTextEdit>
#include <QPointer>
#include "abstractchatform.h"

namespace qutim_sdk_0_3
{

}

namespace Core
{
namespace AdiumChat
{

enum SendMessageKey
{
	SendEnter = 0,
	SendCtrlEnter,
	SendDoubleEnter
};

using namespace qutim_sdk_0_3;
class ChatSessionImpl;
class ADIUMCHAT_EXPORT ChatEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit ChatEdit(QWidget *parent = 0);
	void setSession(ChatSessionImpl *session);
	void setSendKey(SendMessageKey key);
	void setAutoResize(bool resize);
public slots:
	void send();
protected:
	bool event(QEvent *e);
	QString textEditToPlainText();
protected slots:
	void onTextChanged();
private:
	QPointer<ChatSessionImpl> m_session;
	int m_entersCount;
	int previousTextHeight;
	SendMessageKey m_sendKey;
	bool m_autoResize;
	QTextCursor m_enterPosition;
	qreal m_fontSize;
};

}
}

#endif // CHATEDIT_H

