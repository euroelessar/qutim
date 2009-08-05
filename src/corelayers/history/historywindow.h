/*
    HistoryWindow

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

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
#include "ui_historywindow.h"
#include <QTextCodec>
class AbstractHistoryLayer;
class HistoryWindow : public QWidget
{
    Q_OBJECT

public:
    HistoryWindow(const QString &protocol_name,
    		const QString &account_name,
    		const QString &item_name,
    		const QString &profile_name,
		AbstractHistoryLayer *h_layer,
    		QWidget *parent = 0);
    ~HistoryWindow();

private slots:
	void fillContactComboBox(int index);
	void fillDateTreeWidget(int index, const QString &search_word = QString());
	void fillMonth(QTreeWidgetItem *month);
	void on_dateTreeWidget_currentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );
	void on_searchButton_clicked();
    
private:
	void fillAccountComboBox();
	void setIcons();
	QString toHex(const QString &text)
	{
		return QString(text.toUtf8().toHex());
	}
	QString fromHex(const QString &text)
	{
		return codec->toUnicode(QByteArray::fromHex(text.toUtf8()));
	}
    Ui::HistoryWindowClass ui;
    void moveToDesktopCenter();
    QString m_protocol_name;
    QString m_account_name;
    QString m_item_name;
    QString m_profile_name;
    QString m_history_path;
	QString m_search_word;
    QTextCodec *codec;
    AbstractHistoryLayer *m_h_layer;
};

#endif // HISTORYWINDOW_H
