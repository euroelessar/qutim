/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
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

#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QtGui/QWidget>
#include <QByteArray>
#include <qutim/plugininterface.h>
#include "ui_historywindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>

using namespace qutim_sdk_0_2;

namespace SqlHistoryNamespace {

class SqlEngine;

class HistoryWindow : public QWidget
{
    Q_OBJECT

public:
	HistoryWindow(const TreeModelItem &item, SqlEngine *engine, QWidget *parent = 0);

private slots:
	void fillContactComboBox(int index);
  void fillDateTreeWidget();
  void fillDateTreeWidget(int index);
	void fillMonth(QTreeWidgetItem *month);
  void on_dateTreeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
  void contextMenuRequested ( const QPoint & pos );
  void deleteCurrent();
  void deleteSelectedItems();
  void clearSelectedItems();
  void deleteDay();
  void itemClicked(QTreeWidgetItem * item, int  );
    
private:
	void fillAccountComboBox();
	void setIcons();
	Ui::HistoryWindowClass ui;
	qutim_sdk_0_3::TreeModelItem m_item;
    QString m_history_path;
	QString m_search_word;
	SqlEngine *m_engine;
	QAction *deleteCurrentAction;
	QAction *deleteSelectedAction;
	QAction *deleteDayAction;
	QAction *clearSelectedAction;
	QList<QTreeWidgetItem*> selectedItems;
};

}

#endif // HISTORYWINDOW_H

