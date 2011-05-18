/****************************************************************************
 *  ircaccountnicksettings.h
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
