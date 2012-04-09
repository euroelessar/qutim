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

#ifndef TEXTVIEWFACTORY_H
#define TEXTVIEWFACTORY_H

#include <chatlayer/chatviewfactory.h>

namespace Core
{
namespace AdiumChat
{
class TextViewFactory : public QObject, public Core::AdiumChat::ChatViewFactory
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewFactory)
	Q_CLASSINFO("Service", "ChatViewFactory")
	Q_CLASSINFO("Uses", "ChatForm")
public:
    TextViewFactory();
	virtual QWidget *createViewWidget();
	virtual QObject *createViewController();
};
}
}

#endif // TEXTVIEWFACTORY_H

