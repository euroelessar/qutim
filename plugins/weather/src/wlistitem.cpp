/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nikita Belov <null@deltaz.org>
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

#include "wlistitem.h"
#include <QTextDocument>
#include <qutim/icon.h>


WListItem::WListItem(const QString &city, const QString &state, const QString &id, QListWidget *citiesList)
    : m_id(id), m_name(city), m_state(state)
{
	QString title = QString::fromLatin1("%1<br>%2")
			.arg(city.toHtmlEscaped(), state.toHtmlEscaped());
	m_label = new QLabel(title, this);
	QSizePolicy policy = m_label->sizePolicy();
	policy.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	m_label->setSizePolicy(policy);
	m_button = new QPushButton(tr("Remove"), this);
	m_button->setIcon(qutim_sdk_0_3::Icon(QLatin1String("list-remove")));
	connect(m_button, SIGNAL(clicked(bool)), this, SIGNAL(buttonClicked()));
	
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(m_label);
	layout->addWidget(m_button);

	m_item = new QListWidgetItem(citiesList);
	m_item->setData(Qt::SizeHintRole, sizeHint());
	citiesList->setItemWidget(m_item, this);
}

WListItem::~WListItem()
{
}

QString WListItem::id() const
{
	return m_id;
}

QString WListItem::name() const
{
	return m_name;
}

QString WListItem::state() const
{
	return m_state;
}

QListWidgetItem *WListItem::item()
{
	return m_item;
}

void WListItem::setItem(QListWidgetItem *item)
{
	m_item = item;
}

