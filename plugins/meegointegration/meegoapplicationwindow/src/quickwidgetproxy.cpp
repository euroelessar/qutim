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
#include "constants.h"
#include <QGraphicsProxyWidget>

namespace MeegoIntegration
{

QuickWidgetProxy::QuickWidgetProxy(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
	setFlag(ItemHasNoContents, false);
	m_proxy = new QGraphicsProxyWidget(this);
	connect(this, SIGNAL(heightChanged()), this, SLOT(onSizeChanged()));
	m_active = true;
}

QObject *QuickWidgetProxy::widget() const
{
	return m_proxy->widget();
}

void QuickWidgetProxy::setWidget(QObject *widget)
{

	QWidget *proxyWidget=qobject_cast<QWidget*>(widget);
	proxyWidget->setStyleSheet(STYLE);
	m_proxy->setWidget(proxyWidget);
	m_proxy->setPos(0.0, 0.0);
	emit widgetChanged(widget);
	connect(m_proxy, SIGNAL(destroyed()), this, SLOT(onClosed()));
}

void QuickWidgetProxy::onSizeChanged()
{
	if (m_active)
	m_proxy->resize(width(), height());
}

void QuickWidgetProxy::onClosed()
{
	m_active = false;
}


}
