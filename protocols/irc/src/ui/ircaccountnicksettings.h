/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef IRCACCOUNTNICKSETTINGS_H
#define IRCACCOUNTNICKSETTINGS_H

#include <QWizardPage>
#include <qutim/config.h>
#include "ircsettingswidget.h"

namespace Ui {
	class EditNickForm;
}

namespace qutim_sdk_0_3 {

namespace irc {

class IrcAccount;

class IrcAccountNickSettings: public QWizardPage
{
	Q_OBJECT
public:
	IrcAccountNickSettings(QWidget *parent = 0);
	~IrcAccountNickSettings();
	QStringList nicks() const;
	void saveToConfig(Config &cfg);
	void reloadSettings(IrcAccount *account);
	QWidgetList editableWidgets();
	void initSettingsWidget(SettingsWidget *widget) { Q_UNUSED(widget); }
private:
	Ui::EditNickForm *ui;
};

typedef IrcSettingsWidget<IrcAccountNickSettings> IrcAccountNickSettingsWidget;

} } // namespace namespace qutim_sdk_0_3::irc

#endif // IRCACCOUNTNICKSETTINGS_H

