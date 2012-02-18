/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2008 Rustam Chakin <qutim.develop@gmail.com>
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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


#include "statusdialog.h"
#include <qutim/icon.h>
#include <qutim/configbase.h>
#include "../../src/icqaccount.h"
#include <QPushButton>

namespace qutim_sdk_0_3 {

namespace oscar {

CustomStatusDialog::CustomStatusDialog(IcqAccount *account, QWidget *parent) :
	QDialog(parent),
	m_account(account)
{
	Config config = m_account->config("xstatus");
	ui.setupUi(this);
	setWindowIcon(Icon("user-status-xstatus"));
	ui.birthBox->setVisible(false);
	ui.birthBox->setChecked(config.value("birth", false));

	foreach (const XStatus &status, *xstatusList()) {
		QListWidgetItem *tmp = new QListWidgetItem(ui.iconList);
		tmp->setIcon(status.icon);
		tmp->setToolTip(status.value);
	}

	QVariantHash xstatus = m_account->property("xstatus").toHash();
	int current = xstatusIndexByName(xstatus.value("name").toString());
	ui.iconList->setCurrentRow(current);
	setCurrentRow(current);

	connect(ui.iconList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), SLOT(onChooseClicked()));
	connect(ui.iconList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			SLOT(onCurrentItemChanged(QListWidgetItem*)));
	connect(ui.awayEdit, SIGNAL(textChanged()), SLOT(onAwayTextChanged()));
}

CustomStatusDialog::~CustomStatusDialog()
{
}

void CustomStatusDialog::onCurrentItemChanged(QListWidgetItem *current)
{
	setCurrentRow(ui.iconList->row(current));
}

void CustomStatusDialog::setCurrentRow(int row)
{
	XStatus status = xstatusList()->value(row);
	if (row == 0) {
		ui.captionEdit->clear();
		ui.awayEdit->clear();
		ui.captionEdit->setEnabled(false);
		ui.awayEdit->setEnabled(false);
	} else {
		ui.captionEdit->setEnabled(true);
		ui.awayEdit->setEnabled(true);
		Config config = m_account->config("xstatus");
		config.beginGroup(status.name);
		QString cap = config.value("caption", QString());
		if (cap.isEmpty())
			cap = status.value;
		ui.captionEdit->setText(cap);
		ui.awayEdit->setText(config.value("message", QString()));
		config.endGroup();
	}
}

void CustomStatusDialog::accept()
{
	XStatus status = this->status();
	if (!status.name.isEmpty()) {
		Config config = m_account->config("xstatus");
		config.beginGroup(status.name);
		config.setValue("caption", caption());
		config.setValue("message", message());
		config.endGroup();
		config.setValue("birth", ui.birthBox->isChecked());
	}
	QDialog::accept();
}

void CustomStatusDialog::onAwayTextChanged()
{
	QString xstat = ui.awayEdit->toPlainText();
	if (xstat.length() > 6500)
		ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	else
		ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

} } // namespace qutim_sdk_0_3::oscar

