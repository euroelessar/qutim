/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "quatzelactiondialog.h"
#include "ui_quatzelactiondialog.h"
#include "quetzalrequest.h"
#include <QPushButton>
#include <QStringBuilder>
#include <QTextDocument>
#include <QButtonGroup>

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
	QButtonGroup *group = new QButtonGroup(this);
	connect(group, SIGNAL(buttonClicked(int)), this, SLOT(onButtonClick(int)));
	for (int i = 0; i < actions.size(); i++) {
		QPushButton *button = buttonBox()->addButton(actions.at(i).first, QDialogButtonBox::ActionRole);
		group->addButton(button, i);
	}
}

QuetzalActionDialog::~QuetzalActionDialog()
{
//	delete ui;
}

void QuetzalActionDialog::onButtonClick(int index)
{
	const QuetzalRequestAction &action = m_actions.at(index);
	action.second(userData(), index);
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

