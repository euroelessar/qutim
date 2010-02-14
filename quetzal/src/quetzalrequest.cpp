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
#include "quetzalinputdialog.h"
#include "quetzalchoicedialog.h"
#include "quatzelactiondialog.h"
#include "quetzalfieldsdialog.h"
#include <qutim/debug.h>
#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QStringBuilder>
#include <QTextDocument>
#include <cstdarg>
#include "quetzalfiledialog.h"

using namespace qutim_sdk_0_3;

QString quetzal_create_label(const char *primary, const char *secondary)
{
	// Got from GTK+ interface, named Pidgin
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
	return label;
}

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
	QDialog *dialog = new QuetzalInputDialog(title, primary, secondary, default_value,
											 multiline, masked, hint, ok_text, ok_cb,
											 cancel_text, cancel_cb, user_data);
	dialog->show();
	return dialog;
//
//	QInputDialog dialog;
//	dialog.setWindowTitle(title);
//	dialog.setLabelText(quetzal_create_label(primary, secondary));
//	dialog.setTextValue(default_value);
//	dialog.setTextEchoMode(masked ? QLineEdit::Password : QLineEdit::Normal);
//	if (ok_text)
//		dialog.setOkButtonText(ok_text);
//	if (cancel_text)
//		dialog.setCancelButtonText(cancel_text);
//
//	bool ok = !!dialog.exec();
//
//	PurpleRequestInputCb func = reinterpret_cast<PurpleRequestInputCb>(ok ? ok_cb : cancel_cb);
//	if (func)
//		(*func)(user_data, ok ? dialog.textValue().toUtf8().constData() : NULL);
//	return NULL;
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
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	QDialog *dialog = new QuetzalChoiceDialog(title, primary, secondary, default_value, ok_text, ok_cb,
											  cancel_text, cancel_cb, user_data, choices);
	dialog->show();
	return dialog;
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
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	QDialog *dialog = new QuetzalFieldsDialog(title, primary, secondary, fields, ok_text,
											  ok_cb, cancel_text, cancel_cb, user_data);
	dialog->show();
	return dialog;
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
	QFileInfo info = QString(filename);
	QFileDialog *dialog = new QFileDialog;
	new QuetzalFileDialog(title, info.absolutePath(), ok_cb, cancel_cb, user_data, dialog);
	dialog->setAcceptMode(savedialog ? QFileDialog::AcceptSave : QFileDialog::AcceptSave);
	dialog->show();
	return dialog;
}

void quetzal_close_request(PurpleRequestType type, void *ui_handle)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(type);
	QObject *obj = reinterpret_cast<QObject *>(ui_handle);
	if (QWidget *widget = qobject_cast<QWidget *>(obj)) {
		widget->close();
		widget->deleteLater();
	}
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
	QFileDialog *dialog = new QFileDialog;
	new QuetzalFileDialog(title, dirname, ok_cb, cancel_cb, user_data, dialog);
	dialog->setFileMode(QFileDialog::Directory);
	dialog->setOption(QFileDialog::ShowDirsOnly, true);
	dialog->show();
	return dialog;
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
