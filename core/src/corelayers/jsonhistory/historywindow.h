/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2008 Rustam Chakin <qutim.develop@gmail.com>
**           (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
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

#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QtGui/QWidget>
#include <QByteArray>
#include <qutim/chatunit.h>
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
	void fillDateTreeWidget(int index, const QString &search_word = QString());
	void fillMonth(QTreeWidgetItem *month);
	void on_dateTreeWidget_currentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );
	void on_searchButton_clicked();
    
private:
	void fillAccountComboBox();
	void setIcons();
	Ui::HistoryWindowClass ui;
	const ChatUnit *m_unit;
    QString m_history_path;
	QString m_search_word;
};

}

#endif // HISTORYWINDOW_H
