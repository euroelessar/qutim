/****************************************************************************
 *  accountcreator.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef ACCOUNTCREATOR_H
#define ACCOUNTCREATOR_H

#include "qutim/protocol.h"
#include "ircconnection.h"
#include <QWizardPage>

namespace Ui {
	class AddAccountForm;
	class EditNickForm;
}

namespace qutim_sdk_0_3 {

namespace irc {

class IrcAccountCreationWizard;

struct ServerData : public IrcServer
{
	bool savePassword;
};
	
class IrcAccWizardPage: public QWizardPage
{
	Q_OBJECT
public:
	IrcAccWizardPage(IrcAccountCreationWizard *accountWizard, QWidget *parent = 0);
	~IrcAccWizardPage();
	QString networkName() const;
	QList<ServerData> servers() const { return m_servers; }
	bool validatePage();
	bool isComplete() const;
private slots:
	void onAddServer();
	void onEditServer();
	void onRemoveServer();
	void onMoveUpServer();
	void onMoveDownServer();
	void onCurrentServerChanged(int row);
	void onPasswordProtectionChanged(bool checked);
private:
	ServerData currentServer();
	void moveServer(int row, int newRow);
	IrcAccountCreationWizard *m_accountWizard;
	Ui::AddAccountForm *ui;
	QList<ServerData> m_servers;
};

class IrcNickWizardPage: public QWizardPage
{
	Q_OBJECT
public:
	IrcNickWizardPage(IrcAccountCreationWizard *accountWizard, QWidget *parent = 0);
	~IrcNickWizardPage();
	QString fullName() const;
	QStringList nicks() const;
	QString password() const;
	QString encoding() const;
	bool validatePage();
private:
	Ui::EditNickForm *ui;
	IrcAccountCreationWizard *m_accountWizard;
};

class IrcAccountCreationWizard: public AccountCreationWizard
{
	Q_OBJECT
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::irc::IrcProtocol")
public:
	IrcAccountCreationWizard();
	~IrcAccountCreationWizard();
	QList<QWizardPage *> createPages(QWidget *parent);
	void finished();
private:
	IrcAccWizardPage *m_mainPage;
	IrcNickWizardPage *m_nicksPage;
};

} } // namespace qutim_sdk_0_3::irc

#endif // ACCOUNTCREATOR_H
