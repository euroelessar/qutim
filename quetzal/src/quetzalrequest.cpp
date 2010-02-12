/****************************************************************************
 *  quetzalrequest.cpp
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

#include "quetzalrequest.h"
#include "quatzelactiondialog.h"
#include <qutim/debug.h>
#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QStringBuilder>
#include <QTextDocument>

using namespace qutim_sdk_0_3;

void *quetzal_request_input(const char *title, const char *primary,
							const char *secondary, const char *default_value,
							gboolean multiline, gboolean masked, gchar *hint,
							const char *ok_text, GCallback ok_cb,
							const char *cancel_text, GCallback cancel_cb,
							PurpleAccount *account, const char *who,
							PurpleConversation *conv, void *user_data)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	QString label;
	if (primary) {
		label += QLatin1Literal("<span weight=\"bold\" size=\"larger\">")
				 % Qt::escape(primary)
				 % QLatin1Literal("</span>");
		if (secondary)
			label += "\n\n";
	}
	if (secondary)
		label += Qt::escape(secondary);

	QInputDialog dialog;
	dialog.setWindowTitle(title);
	dialog.setLabelText(label);
	dialog.setTextValue(default_value);
	dialog.setTextEchoMode(masked ? QLineEdit::Password : QLineEdit::Normal);
	if (ok_text)
		dialog.setOkButtonText(ok_text);
	if (cancel_text)
		dialog.setCancelButtonText(cancel_text);

	bool ok = !!dialog.exec();

	PurpleRequestInputCb func = reinterpret_cast<PurpleRequestInputCb>(ok ? ok_cb : cancel_cb);
	if (func)
		(*func)(user_data, ok ? dialog.textValue().toUtf8().constData() : NULL);
	return NULL;
}

void *quetzal_request_choice(const char *title, const char *primary,
							 const char *secondary, int default_value,
							 const char *ok_text, GCallback ok_cb,
							 const char *cancel_text, GCallback cancel_cb,
							 PurpleAccount *account, const char *who,
							 PurpleConversation *conv, void *user_data,
							 va_list choices)
{
	debug() << Q_FUNC_INFO;
}

void *quetzal_request_action(const char *title, const char *primary,
							 const char *secondary, int default_action,
							 PurpleAccount *account, const char *who,
							 PurpleConversation *conv, void *user_data,
							 size_t action_count, va_list actions)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	QuetzalRequestActionList uiActions;
	while (action_count --> 0) {
		QString str = va_arg(actions, gchararray);
		PurpleRequestActionCb cb = va_arg(actions, PurpleRequestActionCb);
		uiActions << qMakePair(str, cb);
	}
	QDialog *dialog = new QuetzalActionDialog(title, primary, secondary,
											  default_action, uiActions, user_data, NULL);
	dialog->show();
	return dialog;
}

void *quetzal_request_fields(const char *title, const char *primary,
							 const char *secondary, PurpleRequestFields *fields,
							 const char *ok_text, GCallback ok_cb,
							 const char *cancel_text, GCallback cancel_cb,
							 PurpleAccount *account, const char *who,
							 PurpleConversation *conv, void *user_data)
{
	debug() << Q_FUNC_INFO;
}

void *quetzal_request_file(const char *title, const char *filename,
						   gboolean savedialog, GCallback ok_cb,
						   GCallback cancel_cb, PurpleAccount *account,
						   const char *who, PurpleConversation *conv,
						   void *user_data)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	QString file = (savedialog ? QFileDialog::getSaveFileName : QFileDialog::getOpenFileName)
				   (NULL, title, filename, QString(), NULL, 0);
	PurpleRequestFileCb func;
	if (file.isEmpty())
		func = reinterpret_cast<PurpleRequestFileCb>(cancel_cb);
	else
		func = reinterpret_cast<PurpleRequestFileCb>(ok_cb);

	if (func)
		(*func)(user_data, file.isEmpty() ? NULL : file.toUtf8().constData());
	return NULL;
}

void quetzal_close_request(PurpleRequestType type, void *ui_handle)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(type);
	QWidget *widget = reinterpret_cast<QWidget *>(ui_handle);
	widget->deleteLater();
}

void *quetzal_request_folder(const char *title, const char *dirname,
							 GCallback ok_cb, GCallback cancel_cb,
							 PurpleAccount *account, const char *who,
							 PurpleConversation *conv, void *user_data)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	QString dir = QFileDialog::getExistingDirectory(NULL, title, dirname);
	PurpleRequestFileCb func;
	if (dir.isEmpty())
		func = reinterpret_cast<PurpleRequestFileCb>(cancel_cb);
	else
		func = reinterpret_cast<PurpleRequestFileCb>(ok_cb);

	if (func)
		(*func)(user_data, dir.toUtf8().constData());
	return NULL;
}

PurpleRequestUiOps quetzal_request_uiops =
{
	quetzal_request_input,
	quetzal_request_choice,
	quetzal_request_action,
	quetzal_request_fields,
	quetzal_request_file,
	quetzal_close_request,
	quetzal_request_folder,
	NULL,
	NULL,
	NULL,
	NULL
};
