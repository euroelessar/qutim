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

#include "textviewwidget.h"
#include "textviewcontroller.h"
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>
#include <qutim/servicemanager.h>

namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

TextViewWidget::TextViewWidget()
{
//	m_textEdit = new QTextEdit(this);
	setReadOnly(true);
	setOpenLinks(false);
//	setOpenExternalLinks(true);
	viewport()->setAutoFillBackground(true);

	if(QObject *scroller = ServiceManager::getByName("Scroller"))
		QMetaObject::invokeMethod(scroller,
								  "enableScrolling",
								  Q_ARG(QObject*, viewport()));
}

void TextViewWidget::setViewController(QObject *object)
{
	TextViewController *controller = qobject_cast<TextViewController*>(document());
	if (controller) {
		controller->setScrollBarPosition(verticalScrollBar()->value());
		controller->setTextEdit(0);
	}
	controller = qobject_cast<TextViewController*>(object);
	if (controller) {
		controller->setTextEdit(this);
		setDocument(controller);
		verticalScrollBar()->setValue(controller->scrollBarPosition());
		QTimer::singleShot(0, controller, SLOT(ensureScrolling()));
//		controller->ensureScrolling();
	} else {
		setDocument(new QTextDocument(this));
	}
}
}
}

