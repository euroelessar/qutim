/****************************************************************************
 * wlistitem.h
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

#ifndef WLISTITEM_H
#define WLISTITEM_H

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>
#include <QWidget>

class WListItem : public QWidget
{
	Q_OBJECT

public:
	WListItem( const QString &title, bool addIcon = true );
	~WListItem();

	QString title();
	void setTitle( const QString &title );

	QListWidgetItem *item();
	void setItem( QListWidgetItem *item );

signals:
	void buttonClicked();

private slots:
	void buttonClicked( bool checked );

private:
	QLabel *m_label;
	QPushButton *m_button;
	QListWidgetItem *m_listWidgetItem;
};

#endif // WLISTITEM_H