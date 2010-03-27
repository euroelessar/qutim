/*
    HistoryWindow

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
				  2009 by Ruslan Nigmatullin <euroelessar@gmail.com>
				  2010 by Sidorov Aleksey <sauron@citadelspb.com>

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

class QTreeWidgetItem;
namespace Ui
{
	class HistoryWindowClass;
}
namespace qutim_sdk_0_3
{
	class ChatUnit;
	class Message;
	typedef QList<Message> MessageList;
}

namespace Core {
	
	using namespace qutim_sdk_0_3;

	class HistoryWindow : public QWidget
	{
		Q_OBJECT

	public:
		HistoryWindow(ChatUnit *unit,QWidget *parent = 0);
		virtual ~HistoryWindow();
		void setUnit(ChatUnit *unit);
	private slots:
		void fillChatUnitsComboBox();
		void fillDateTreeWidget(const MessageList &messageList);
		//void fillContactComboBox(int index);
		//void fillDateTreeWidget(int index, const QString &search_word = QString());
		//void fillMonth(QTreeWidgetItem *month);
		//void on_dateTreeWidget_currentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );
	//	void on_searchButton_clicked();
		void currentUnitIndexChanged(int);
		void currentDateItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);		
	private:
		QString toHtml(const Message &msg);
		//void fillAccountComboBox();
		QString m_history_path;
		QString m_search_word;
		ChatUnit *m_current_unit;
		QList<ChatUnit *> m_units;
		Ui::HistoryWindowClass *ui;
		MessageList m_msg_list; //all history messages for current unit
	};

}

#endif // HISTORYWINDOW_H
