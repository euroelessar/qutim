/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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
	QPointer<SettingsWidget> m_widget;
};

typedef IrcSettingsWidget<IrcAccountMainSettings> IrcAccountMainSettingsWidget;

} } // namespace namespace qutim_sdk_0_3::irc

#endif // IRCACCOUNTMAINSETTINGS_H

