/*
    Conference Item

    Copyright (c) 2008 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "conferenceitem.h"

ConferenceItem::ConferenceItem(const QVariant & display, ConfContactList *contact_list)
: m_contact_list(contact_list)
{
	m_item_display = display;
        m_item_icons = QVector<QVariant>(13).toList();
        m_item_bottom_rows = QVector<QVariant>(3).toList();
	m_item_type = 0;
	m_item_status_mass = 0;
	m_item_role_mass = 0;
}

ConferenceItem::~ConferenceItem()
{
}

QVariant ConferenceItem::data(int role) const
{
	switch(role)
	{
	case Qt::DecorationRole:
                return reinterpret_cast<qptrdiff>(&m_item_icons);
	case Qt::DisplayRole:
	case Qt::EditRole:
		return m_item_display;
	case Qt::UserRole:
		return m_item_type;
	case Qt::UserRole+2:
                return reinterpret_cast<qptrdiff>(&m_item_bottom_rows);
	case Qt::UserRole+3:
		return m_item_status;
	case Qt::UserRole+4:
		return m_current_status_icon;
	case Qt::ToolTipRole:{
		if(m_item_type!=0)
			return QVariant();
		QString tooltip = m_contact_list->getToolTip(m_item_display.toString());
		if(tooltip.isNull())
			return QVariant();
		return tooltip;}
	default:
		return QVariant();
	}
}

bool ConferenceItem::setData(const QVariant &value, int role)
{
	switch(role)
	{
	case Qt::DecorationRole:
                m_item_icons = value.toList();
		return true;
	case Qt::UserRole:
		m_item_type = value;
		return true;
	case Qt::DisplayRole:
		m_item_display = value;
		return true;
	case Qt::EditRole:
		m_item_display = value;
		return true;
	case Qt::UserRole+4:
		m_current_status_icon = value;
	default:
		return false;
	}
}

void ConferenceItem::setImage(QIcon icon, int column)
{
	if(column<13&&column>0)
	{
		if(column==1)
		{
			static int size=24;
			static QSize ava_size(size,size);
			QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(65535,65535)),QIcon::Normal,QIcon::On);
			if(!pixmap.isNull())
			{
				static QPixmap alpha;
				if( alpha.isNull() )
				{
					alpha = QPixmap( ava_size );
					alpha.fill(QColor(0,0,0));
					QPainter painter(&alpha);
					QPen pen(QColor(127,127,127));
					painter.setRenderHint(QPainter::Antialiasing);
					pen.setWidth(0);
					painter.setPen(pen);
					painter.setBrush(QBrush(QColor(255,255,255)));
					painter.drawRoundedRect(QRectF(QPointF(0,0),QSize(size-1,size-1)),5,5);
					painter.end();
				}
				pixmap = pixmap.scaled(ava_size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
				pixmap.setAlphaChannel(alpha);
				icon=QIcon(pixmap);
			}
			else
				return;
		}
		m_item_icons[column] = icon;
	}
}

QIcon ConferenceItem::getImage(int column)
{
	if(column<13&&column>-1)
	{
		return  qvariant_cast<QIcon>(m_item_icons[column]);
	}
	return QIcon();
}

void ConferenceItem::setRow(QVariant item, int row)
{
	if(row<0||row>2)
		return;
	m_item_bottom_rows[row]=item;
}

void ConferenceItem::setStatus(QString text, QIcon icon, int mass)
{
	m_item_icons[0]=icon;
	m_current_status_icon=icon;
	m_item_status=text;
	m_item_status_mass=mass;
}

void ConferenceItem::setRole(QString text, QIcon icon, int mass)
{
	m_item_icons[11]=icon;
	m_item_role=text;
	m_item_role_mass=mass;
}

int ConferenceItem::getMass()
{
	return 10000*m_item_role_mass/* + m_item_status_mass*/;
}
