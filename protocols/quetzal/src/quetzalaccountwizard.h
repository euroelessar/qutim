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
#ifndef QUETZALACCOUNTWIZARD_H
#define QUETZALACCOUNTWIZARD_H

#include <qutim/protocol.h>
#include <qutim/dataforms.h>
#include <QWizardPage>
#include <QLabel>
#include <purple.h>

class QuetzalMetaObject;
class QuetzalProtocol;
class QuetzalAccountSettings;

class QuetzalAccountWizardPage : public QWizardPage
{
	Q_OBJECT
public:
	QuetzalAccountWizardPage(QuetzalProtocol *proto, QWidget *parent = 0);
    void initializePage();
    void cleanupPage();
	bool validatePage();
	bool isComplete() const;
	void handleRegisterResult(PurpleAccount *account, bool result);
protected slots:
	void onRegisterButtonClicked();
	void onDataChanged(const QString &name, const QVariant &data);
private:
	PurpleAccount *createAccount();
	bool m_isUsernameFilled;
	QuetzalProtocol *m_proto;
	PurpleAccount *m_account;
	qutim_sdk_0_3::AbstractDataForm *m_generalWidget;
	QuetzalAccountSettings *m_settingsWidget;
	QAbstractButton *m_registerButton;
	bool m_isRegistering;
};

class QuetzalAccountWizard : public qutim_sdk_0_3::AccountCreationWizard
{
	Q_OBJECT
public:
	QuetzalAccountWizard(const QuetzalMetaObject *proto);
	virtual QList<QWizardPage *> createPages(QWidget *parent);
private:
	QuetzalMetaObject *meta;
	QuetzalProtocol *m_proto;
};

#endif // QUETZALACCOUNTWIZARD_H

