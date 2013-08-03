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
	
	WListItem(const QString &city, const QString &state, const QString &id, QListWidget *citiesList);
	~WListItem();
	
	QString id() const;
	QString name() const;
	QString state() const;

	QListWidgetItem *item();
	void setItem(QListWidgetItem *item);

signals:
	void buttonClicked();

private:
	QLabel *m_label;
	QPushButton *m_button;
	QListWidgetItem *m_item;
	QString m_id;
	QString m_name;
	QString m_state;
};

#endif // WLISTITEM_H

