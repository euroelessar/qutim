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
#ifndef ACCOUNTCREATORPROTOCOLS_H
#define ACCOUNTCREATORPROTOCOLS_H

#include <QWizardPage>
#include <qutim/protocol.h>

using namespace qutim_sdk_0_3;

namespace Ui {
class AccountCreatorProtocols;
}

class QListWidgetItem;
class QCommandLinkButton;

namespace Core
{
class AccountCreatorWizard;

class AccountCreatorProtocols : public QWizardPage
{
	Q_OBJECT
public:
	enum { Id = 1 };
	AccountCreatorProtocols(QWizard *parent = 0);
	~AccountCreatorProtocols();
	virtual bool validatePage();
	virtual bool isComplete() const;
	virtual int nextId() const;
public slots:
	void protocolActivated(QListWidgetItem*);
protected:
	QMap<AccountCreationWizard *, int>::iterator ensureCurrentProtocol();
	void changeEvent(QEvent *e);
private:
	Ui::AccountCreatorProtocols *ui;
	QWizard *m_wizard;
	QMultiMap<QString, AccountCreationWizard *> m_wizards;
	QMap<AccountCreationWizard *, int> m_wizardIds;
	int m_lastId;
};
}

#endif // ACCOUNTCREATORPROTOCOLS_H

