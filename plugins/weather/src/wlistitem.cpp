/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Belov Nikita <null@deltaz.org>
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

WListItem::WListItem( const QString &title, bool addIcon )
{
	m_label = new QLabel( title );

	m_button = new QPushButton();
	m_button->setMinimumSize(QSize(16, 16));
	m_button->setMaximumSize(QSize(16, 16));
	m_button->setIcon( QIcon( addIcon ? ":/icons/add.png" : ":/icons/delete.png" ) );
	m_button->setFlat( true );

	connect( m_button, SIGNAL( clicked( bool ) ), this, SLOT( buttonClicked( bool ) ) );

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setContentsMargins( 0,0,0,0 );
	if ( addIcon )
		layout->addWidget( m_button );
	layout->addWidget( m_label );
	if ( !addIcon )
		layout->addWidget( m_button );

	setLayout( layout );
}

WListItem::~WListItem()
{
}

QString WListItem::title()
{
	return m_label->text();
}

void WListItem::setTitle( const QString &title )
{
	m_label->setText( title );
}

QListWidgetItem *WListItem::item()
{
	return m_listWidgetItem;
}

void WListItem::setItem( QListWidgetItem *item )
{
	m_listWidgetItem = item;
}

void WListItem::buttonClicked( bool checked )
{
	Q_UNUSED( checked );

	emit buttonClicked();
}

