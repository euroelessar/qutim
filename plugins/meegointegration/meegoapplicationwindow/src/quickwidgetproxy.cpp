/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#include "quickwidgetproxy.h"
#include <QGraphicsProxyWidget>

namespace MeegoIntegration
{

QuickWidgetProxy::QuickWidgetProxy(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
	setFlag(ItemHasNoContents, false);
	m_proxy = new QGraphicsProxyWidget(this);
	connect(this, SIGNAL(heightChanged()), this, SLOT(onSizeChanged()));
}

QObject *QuickWidgetProxy::widget() const
{
	return m_proxy->widget();
}

void QuickWidgetProxy::setWidget(QObject *widget)
{
	m_proxy->setWidget(qobject_cast<QWidget*>(widget));
	m_proxy->setPos(0.0, 0.0);
}

void QuickWidgetProxy::onSizeChanged()
{
	m_proxy->resize(width(), height());
}

}
