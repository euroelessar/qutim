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

#ifndef SCRIPTMESSAGEPROPERTYITERATOR_H
#define SCRIPTMESSAGEPROPERTYITERATOR_H

#include <QScriptClassPropertyIterator>

namespace qutim_sdk_0_3
{
class Message;

class ScriptMessagePropertyIterator : public QScriptClassPropertyIterator
{
public:
    ScriptMessagePropertyIterator(const QScriptValue &object, Message *msg);
	
    virtual bool hasNext() const;
    virtual void next();

    virtual bool hasPrevious() const;
    virtual void previous();

    virtual void toFront();
    virtual void toBack();

    virtual QScriptString name() const;
private:
	Message *m_msg;
	int m_id;
};
}

#endif // SCRIPTMESSAGEPROPERTYITERATOR_H

