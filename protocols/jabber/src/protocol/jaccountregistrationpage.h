/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JACCOUNTREGISTRATIONPAGE_H
#define JACCOUNTREGISTRATIONPAGE_H

#include <QWizardPage>
#include <jreen/registrationmanager.h>
#include <jreen/client.h>
#include "account/dataform/jdataform.h"

namespace Ui {
class JAccountRegistrationPage;
}

namespace Jabber
{
class JAccountRegistrationPage : public QWizardPage
{
	Q_OBJECT
	
public:
	explicit JAccountRegistrationPage(QWidget *parent = 0);
	~JAccountRegistrationPage();
	
	virtual void cleanupPage();
	virtual void initializePage();
	virtual bool validatePage();
	virtual bool isComplete() const;
	virtual int nextId() const;
	qutim_sdk_0_3::LocalizedString fieldText(Jreen::RegistrationData::FieldType type);
	
private slots:
	void onFieldsReceived(const Jreen::RegistrationData &data);
	void onError(const Jreen::Error::Ptr &error);
	void onSuccess();
	void onUnsupported();
	
private:
	Ui::JAccountRegistrationPage *ui;
	Jreen::Client *m_client;
	Jreen::RegistrationManager *m_manager;
	JDataForm *m_jabberForm;
	qutim_sdk_0_3::AbstractDataForm *m_form;
	QString m_error;
	bool m_registered;
};
}

#endif // JACCOUNTREGISTRATIONPAGE_H
