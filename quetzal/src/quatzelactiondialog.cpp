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
#include <QPushButton>
#include <QStringBuilder>
#include <QTextDocument>

QuetzalActionDialog::QuetzalActionDialog(const QString &title, const QString &primary,
										 const QString &secondary, int default_action,
										 const QuetzalRequestActionList &actions, gpointer user_data,
										 QWidget *parent) :
    QDialog(parent),
	ui(new Ui::QuetzalActionDialog)
{
    ui->setupUi(this);
	m_default_action = default_action;
	m_user_data = user_data;
	setWindowTitle(title);
	QString label;
	if (!primary.isEmpty()) {
		label += QLatin1Literal("<span weight=\"bold\" size=\"larger\">")
				 % Qt::escape(primary)
				 % QLatin1Literal("</span>");
		if (!secondary.isEmpty())
			label += "\n\n";
	}
	if (!secondary.isEmpty())
		label += Qt::escape(secondary);
	ui->label->setText(label);
	m_actions = actions;
	for (int i = 0; i < actions.size(); i++) {
		QPushButton *button = ui->buttonBox->addButton(actions.at(i).first, QDialogButtonBox::ActionRole);
		button->setProperty("index", i);
	}
}

void QuetzalActionDialog::on_buttonBox_clicked(QAbstractButton *button)
{
	int index = button->property("index").toInt();
	const QuetzalRequestAction &action = m_actions.at(index);
	(*action.second)(m_user_data, index);
	deleteLater();
	m_actions.clear();
}

QuetzalActionDialog::~QuetzalActionDialog()
{
    delete ui;
}

void QuetzalActionDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
