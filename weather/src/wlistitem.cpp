/****************************************************************************
 * wlistitem.cpp
 *
 *  Copyright (c) 2010 by Belov Nikita <null@deltaz.org>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
