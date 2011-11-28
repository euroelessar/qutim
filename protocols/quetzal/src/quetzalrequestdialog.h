/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef QUETZALREQUESTDIALOG_H
#define QUETZALREQUESTDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <purple.h>
#include <qutim/dataforms.h>

extern void quetzal_request_close(PurpleRequestType type, QObject *dialog);

class QuetzalRequestDialog : public QDialog
{
	Q_OBJECT
public:
	explicit QuetzalRequestDialog(const char *title, const char *primary,
								  const char *secondary, PurpleRequestType type,
								  void *user_data, QWidget *parent = 0);
	explicit QuetzalRequestDialog(PurpleRequestType type, void *user_data, QWidget *parent = 0);
protected slots:
	virtual void closeRequest();
	virtual void onClicked(int);
protected:
	qutim_sdk_0_3::DataItem createItem(const char *title, const char *primary, const char *secondary);
	void createItem(const qutim_sdk_0_3::DataItem &item, const char *okText, const char *cancelText);
	void createItem(const qutim_sdk_0_3::DataItem &item, const qutim_sdk_0_3::AbstractDataForm::Buttons &);
	virtual void closeEvent(QCloseEvent *);
	void *userData() { return m_user_data; }
	QVBoxLayout *boxLayout() { return m_boxLayout; }
	QDialogButtonBox *buttonBox() { return m_buttonBox; }
	qutim_sdk_0_3::AbstractDataForm *form() { return m_form; }
private:
	qutim_sdk_0_3::AbstractDataForm *m_form;
	QVBoxLayout *m_boxLayout;
	QDialogButtonBox *m_buttonBox;
	PurpleRequestType m_type;
	void *m_user_data;
};

#endif // QUETZALREQUESTDIALOG_H

