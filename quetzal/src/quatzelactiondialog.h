/****************************************************************************
 *  quatzelactiondialog.h
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
