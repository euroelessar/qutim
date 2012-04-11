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
#ifndef JACCOUNTWIZARDPAGE_H
#define JACCOUNTWIZARDPAGE_H

#include <QWizardPage>
#include <QNetworkAccessManager>
#include "jaccountwizard.h"

namespace Ui
{
class JAccountWizardPage;
}

namespace Jabber
{
class JAccountWizardPage : public QWizardPage
{
	Q_OBJECT
public:
	JAccountWizardPage(JAccountWizard *accountWizard,
					   JAccountType type, QWidget *parent = 0);
	~JAccountWizardPage();
	bool validatePage();
	QString jid();
	QString passwd();
	bool isSavePasswd();
	virtual int nextId() const;
	
private slots:
	void on_newAccountButton_clicked();
	void on_oldAccountButton_clicked();
	void onFinished(QNetworkReply *reply);
	
private:
	QNetworkAccessManager m_networkManager;
	JAccountWizard *m_accountWizard;
	JAccountType m_type;
	Ui::JAccountWizardPage *ui;
};
}

#endif // JACCOUNTWIZARDPAGE_H

