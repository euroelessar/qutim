/*
	statusdialog.cpp

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


#include "statusdialog.h"
#include <qutim/icon.h>
#include <qutim/configbase.h>
#include "icqaccount.h"

namespace qutim_sdk_0_3 {

namespace oscar {

CustomStatusDialog::CustomStatusDialog(IcqAccount *account, QWidget *parent) :
	QDialog(parent),
	m_account(account)
{
	Config config = m_account->config("xstatus");
	ui.setupUi(this);
	setWindowIcon(Icon("user-xstatus-icon"));
	ui.chooseButton->setIcon(Icon("dialog-ok-apply"));
	ui.cancelButton->setIcon(Icon("dialog-cancel"));
	ui.birthBox->setVisible(false);
	ui.birthBox->setChecked(config.value("birth", false));

	QString last = config.value("last", QString());
	QListWidgetItem *current = 0;
	foreach (const XStatus &status, *xstatusList()) {
		QListWidgetItem *tmp = new QListWidgetItem(ui.iconList);
		tmp->setIcon(status.icon);
		tmp->setToolTip(status.value);
		if (last == status.name)
			current = tmp;
	}
	if (!current)
		current = ui.iconList->item(0);
	ui.iconList->setCurrentItem(current);
	onCurrentItemChanged(current);

	connect(ui.iconList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), SLOT(onChooseClicked()));
	connect(ui.iconList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			SLOT(onCurrentItemChanged(QListWidgetItem*)));
	connect(ui.chooseButton, SIGNAL(clicked()), SLOT(onChooseClicked()));
	connect(ui.awayEdit, SIGNAL(textChanged()), SLOT(onAwayTextChanged()));
}

CustomStatusDialog::~CustomStatusDialog()
{
}

void CustomStatusDialog::onCurrentItemChanged(QListWidgetItem *current)
{
	int row = ui.iconList->row(current);
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

void CustomStatusDialog::onChooseClicked()
{
	XStatus status = this->status();
	if (!status.name.isEmpty()) {
		Config config = m_account->config("xstatus");
		config.setValue("last", status.name);
		config.beginGroup(status.name);
		config.setValue("caption", caption());
		config.setValue("message", message());
		config.endGroup();
		config.setValue("birth", ui.birthBox->isChecked());
	}
	accept();
}

void CustomStatusDialog::onAwayTextChanged()
{
	QString xstat = ui.awayEdit->toPlainText();
	if (xstat.length() > 6500)
		ui.chooseButton->setEnabled(false);
	else
		ui.chooseButton->setEnabled(true);
}

} } // namespace qutim_sdk_0_3::oscar
