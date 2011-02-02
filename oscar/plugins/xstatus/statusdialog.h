/*
	statusdialog.h

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
	Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#ifndef STATUSDIALOG_H
#define STATUSDIALOG_H

#include "ui_statusdialog.h"
#include "xstatus.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class CustomStatusDialog : public QDialog
{
    Q_OBJECT
public:
	CustomStatusDialog(IcqAccount *account, QWidget *parent = 0);
	~CustomStatusDialog();
	IcqAccount *account() { return m_account; }
	QString caption() const { return ui.captionEdit->text(); }
	QString message() const { return ui.awayEdit->toPlainText(); }
	XStatus status() const { return xstatusList()->value(ui.iconList->currentRow()); }
public slots:
	void accept();
private slots:
	void onCurrentItemChanged(QListWidgetItem * current);
	void onAwayTextChanged();
private:
	void setCurrentRow(int row);
	Ui::statusDialogClass ui;
	IcqAccount *m_account;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // STATUSDIALOG_H
