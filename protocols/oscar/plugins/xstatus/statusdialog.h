/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2008 Rustam Chakin <qutim.develop@gmail.com>
** Copyright © 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

