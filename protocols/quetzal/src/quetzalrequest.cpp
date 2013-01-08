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
#include <QLibrary>
#include <cstdarg>
#include "quetzalfiledialog.h"
#include "quetzalaccount.h"

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

void *quetzal_request_guard_new(QObject *obj)
{
	void *data = reinterpret_cast<void*>(obj->property("quetzal_guard").value<qptrdiff>());
	if (data)
		return data;
	QPointer<QObject> *pointer = new QPointer<QObject>(obj);
	obj->setProperty("quetzal_guard", reinterpret_cast<qptrdiff>(pointer));
	return pointer;
}

QObject *quetzal_request_guard_value(void *data)
{
	if (!data)
		return 0;
	QPointer<QObject> *pointer = reinterpret_cast<QPointer<QObject>*>(data);
	QObject *value = pointer->data();
	delete pointer;
	return value;
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
	return quetzal_request_guard_new(dialog);
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
	return quetzal_request_guard_new(dialog);
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
	return quetzal_request_guard_new(dialog);
}

typedef void *(*QuetzalRequestHook)(const char *primary, PurpleRequestFields *fields,
									GCallback ok_cb, GCallback cancel_cb,
									PurpleAccount *account,  PurpleConversation *conv,
									void *user_data);

typedef char *(*dgettext_) (const char *domainname, const char *msgid);
static dgettext_ dgettext = 0;
char *dgettext_fallback(const char *domainname, const char *msgid)
{
	Q_UNUSED(domainname);
	return const_cast<char *>(msgid);
}

void *quetzal_request_password_hook(const char *primary, PurpleRequestFields *fields,
									   GCallback ok_cb, GCallback cancel_cb,
									   PurpleAccount *account,  PurpleConversation *conv,
									   void *user_data)
{
	Q_UNUSED(conv);
	char *primary_test = g_strdup_printf(dgettext("libpurple", "Enter password for %s (%s)"),
										 purple_account_get_username(account),
										 purple_account_get_protocol_name(account));
	if (qstrcmp(primary_test, primary)) {
		g_free(primary_test);
		return 0;
	}
	g_free(primary_test);
	
	QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(account->ui_data);
	QObject *obj = acc->requestPassword(fields, reinterpret_cast<PurpleRequestFieldsCb>(ok_cb),
										reinterpret_cast<PurpleRequestFieldsCb>(cancel_cb), user_data);
	return quetzal_request_guard_new(obj);
}

QuetzalRequestHook quetzal_request_hooks[] =
{
	quetzal_request_password_hook
};

void *quetzal_request_fields(const char *title, const char *primary,
							 const char *secondary, PurpleRequestFields *fields,
							 const char *ok_text, GCallback ok_cb,
							 const char *cancel_text, GCallback cancel_cb,
							 PurpleAccount *account, const char *who,
							 PurpleConversation *conv, void *user_data)
{
	if (!dgettext) {
		dgettext = reinterpret_cast<dgettext_>(QLibrary::resolve("nsl", "dgettext"));
		if (!dgettext)
			dgettext = dgettext_fallback;
	}
	debug() << Q_FUNC_INFO << (sizeof(quetzal_request_hooks)/sizeof(QuetzalRequestHook));
	for (unsigned i = 0; i < sizeof(quetzal_request_hooks)/sizeof(QuetzalRequestHook); i++) {
		if (void *data = quetzal_request_hooks[i](primary, fields, ok_cb, cancel_cb, account, conv, user_data))
			return data;
	}
	Q_UNUSED(who);
	QDialog *dialog = new QuetzalFieldsDialog(title, primary, secondary, fields, ok_text,
											  ok_cb, cancel_text, cancel_cb, user_data);
	dialog->show();
	return quetzal_request_guard_new(dialog);
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
	return quetzal_request_guard_new(dialog);
}

void quetzal_request_close(PurpleRequestType type, QObject *dialog)
{
	if (dialog->property("quetzal_closed").toBool())
		return;
	dialog->setProperty("quetzal_closed", true);
	purple_request_close(type, quetzal_request_guard_new(dialog));
}

void quetzal_close_request(PurpleRequestType type, void *ui_handle)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(type);
	QObject *obj = quetzal_request_guard_value(ui_handle);
	if (QWidget *widget = qobject_cast<QWidget *>(obj))
		widget->close();
	if (obj) {
		obj->setProperty("quetzal_closed", true);
		obj->deleteLater();
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
	return quetzal_request_guard_new(dialog);
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

