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

#ifndef QUATZELACTIONDIALOG_H
#define QUATZELACTIONDIALOG_H

#include <QDialog>
#include <purple.h>
#include "quetzalrequestdialog.h"

class QAbstractButton;

namespace Ui {
	class QuetzalActionDialog;
}

typedef QPair<QString, PurpleRequestActionCb> QuetzalRequestAction;
typedef QList<QuetzalRequestAction> QuetzalRequestActionList;

class QuetzalActionDialog : public QuetzalRequestDialog
{
    Q_OBJECT
public:
	QuetzalActionDialog(const char *title, const char *primary,
						const char *secondary, int default_action,
						const QuetzalRequestActionList &actions, gpointer user_data,
						QWidget *parent = 0);
	~QuetzalActionDialog();

protected slots:
	void onButtonClick(int id);

protected:
//    void changeEvent(QEvent *e);

private:
	QuetzalRequestActionList m_actions;
	int m_default_action;
};

#endif // QUATZELACTIONDIALOG_H

