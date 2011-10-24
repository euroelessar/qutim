/****************************************************************************
 *  ircaccountmainsettings.h
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef IRCACCOUNTMAINSETTINGS_H
#define IRCACCOUNTMAINSETTINGS_H

#include <QWizardPage>
#include "qutim/protocol.h"
#include "../ircconnection.h"
#include "ircsettingswidget.h"

namespace Ui {
	class AddAccountForm;
}

namespace qutim_sdk_0_3 {

namespace irc {

class IrcAccount;

class IrcAccountMainSettings: public QWizardPage
{
	Q_OBJECT
public:
	IrcAccountMainSettings(QWidget *parent = 0);
	~IrcAccountMainSettings();
	QString networkName() const;
	QList<IrcServer> servers() const { return m_servers; }
	void saveToConfig(Config &cfg);
	void reloadSettings(IrcAccount *account);
	QWidgetList editableWidgets();
	void initSettingsWidget(SettingsWidget *widget);
signals:
	void modifiedChanged(bool have_changes);
private slots:
	void onAddServer();
	void onEditServer();
	void onRemoveServer();
	void onMoveUpServer();
	void onMoveDownServer();
	void onCurrentServerChanged(int row);
	void onPasswordProtectionChanged(bool checked);
private:
	void addServer(const IrcServer &server);
private:
	IrcServer currentServer();
	void moveServer(int row, int newRow);
	Ui::AddAccountForm *ui;
	QList<IrcServer> m_servers;
	QWeakPointer<SettingsWidget> m_widget;
};

typedef IrcSettingsWidget<IrcAccountMainSettings> IrcAccountMainSettingsWidget;

} } // namespace namespace qutim_sdk_0_3::irc

#endif // IRCACCOUNTMAINSETTINGS_H
