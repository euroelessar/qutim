/*
    HistoryWindow

	Copyright (c) 2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

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
