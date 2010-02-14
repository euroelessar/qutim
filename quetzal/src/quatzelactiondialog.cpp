/****************************************************************************
 *  quatzelactiondialog.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "quatzelactiondialog.h"
#include "ui_quatzelactiondialog.h"
#include "quetzalrequest.h"
#include <QPushButton>
#include <QStringBuilder>
#include <QTextDocument>

QuetzalActionDialog::QuetzalActionDialog(const char *title, const char *primary,
										 const char *secondary, int default_action,
										 const QuetzalRequestActionList &actions, gpointer user_data,
										 QWidget *parent) :
	QuetzalRequestDialog(title, primary, secondary, PURPLE_REQUEST_ACTION, user_data, parent)
//	ui(new Ui::QuetzalActionDialog)
{
//    ui->setupUi(this);
	m_default_action = default_action;
//	m_user_data = user_data;
//	setWindowTitle(title);
//	ui->label->setText(quetzal_create_label(primary, secondary));
	m_actions = actions;
	for (int i = 0; i < actions.size(); i++) {
		QPushButton *button = buttonBox()->addButton(actions.at(i).first, QDialogButtonBox::ActionRole);
		button->setProperty("index", i);
	}
}

QuetzalActionDialog::~QuetzalActionDialog()
{
//	delete ui;
}

void QuetzalActionDialog::on_buttonBox_clicked(QAbstractButton *button)
{
	int index = button->property("index").toInt();
	const QuetzalRequestAction &action = m_actions.at(index);
	(*action.second)(userData(), index);
	m_actions.clear();
	closeRequest();
}
//
//void QuetzalActionDialog::changeEvent(QEvent *e)
//{
//    QDialog::changeEvent(e);
//    switch (e->type()) {
//    case QEvent::LanguageChange:
//        ui->retranslateUi(this);
//        break;
//    default:
//        break;
//    }
//}
