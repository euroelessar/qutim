/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2008 Rustam Chakin <qutim.develop@gmail.com>
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

#ifndef HISTORYWINDOW2_H
#define HISTORYWINDOW2_H

#include <QWidget>
#include <QByteArray>
#include <QRegularExpression>
#include <qutim/chatunit.h>
#include <qutim/history.h>
#include "ui_historywindow.h"

using namespace qutim_sdk_0_3;

namespace Core
{
class JsonEngine;

class HistoryWindow : public QWidget
{
	Q_OBJECT

public:
	HistoryWindow(const ChatUnit *unit);
	void setUnit(const ChatUnit *unit);

private slots:
	void fillContactComboBox(int index);
	void fillDateTreeWidget(int index);
	void fillMonth(QTreeWidgetItem *month);
	void on_dateTreeWidget_currentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );
	void on_searchButton_clicked();
	void findPrevious();

private:
	void fillAccountComboBox();
	void setIcons();
	Ui::HistoryWindowClass ui;
	QMetaObject::Connection m_contactConnection;
	History::ContactInfo m_unitInfo;
	QString m_search_word;
};

}

#endif // HISTORYWINDOW2_H

