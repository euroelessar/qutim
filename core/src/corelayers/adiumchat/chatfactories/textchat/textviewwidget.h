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

#ifndef TEXTVIEWWIDGET_H
#define TEXTVIEWWIDGET_H

#include <chatlayer/chatviewfactory.h>
#include <QFrame>
#include <QTextBrowser>
#include <QTextDocument>

namespace Core
{
namespace AdiumChat
{
class TextViewWidget : public QTextBrowser, public Core::AdiumChat::ChatViewWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewWidget)
public:
    TextViewWidget();
	virtual void setViewController(QObject *controller);
private:
//	QTextEdit *m_textEdit;
};
}
}

#endif // TEXTVIEWWIDGET_H

