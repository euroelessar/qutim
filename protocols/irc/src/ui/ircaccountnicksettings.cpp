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

#include "ircaccountnicksettings.h"
#include "../ircaccount.h"
#include "ui_editnickform.h"
#include <qutim/config.h>

namespace qutim_sdk_0_3 {

namespace irc {

IrcAccountNickSettings::IrcAccountNickSettings(QWidget *parent) :
	QWizardPage(parent), ui(new Ui::EditNickForm)
{
	ui->setupUi(this);
	registerField("fullName", ui->fullNameEdit);
	registerField("nick*", ui->nickEdit);
	registerField("alternativeNick", ui->alternativeNickEdit);
	registerField("nickPassword", ui->passwordEdit);
	registerField("encoding", ui->encodingBox);
}

IrcAccountNickSettings::~IrcAccountNickSettings()
{
	delete ui;
}

QStringList IrcAccountNickSettings::nicks() const
{
	QStringList list;
	QString s = ui->nickEdit->text();
	if (!s.isEmpty())
		list << s;
	s = ui->alternativeNickEdit->text();
	if (!s.isEmpty())
		list << s;
	return list;
}

void IrcAccountNickSettings::saveToConfig(Config &cfg)
{
	cfg.setValue("fullName", ui->fullNameEdit->text());
	cfg.setValue("nicks", nicks());
	cfg.setValue("nickPassword", ui->passwordEdit->text(), Config::Crypted);
	cfg.setValue("codec", ui->encodingBox->currentText());
}

void IrcAccountNickSettings::reloadSettings(IrcAccount *account)
{
	Config cfg = account->config();
	QStringList nicks = cfg.value("nicks", QStringList());
	ui->fullNameEdit->setText(cfg.value("fullName", QString()));
	ui->nickEdit->setText(nicks.value(0));
	ui->alternativeNickEdit->setText(nicks.value(1));
	ui->passwordEdit->setText(cfg.value("nickPassword", QString(), Config::Crypted));
	int encIndex = ui->encodingBox->findText(cfg.value("codec", QString()));
	ui->encodingBox->setCurrentIndex(encIndex == -1 ? 0 : encIndex);
}

QWidgetList IrcAccountNickSettings::editableWidgets()
{
	QWidgetList list;
	list    << ui->fullNameEdit
			<< ui->nickEdit
			<< ui->alternativeNickEdit
			<< ui->passwordEdit
			<< ui->encodingBox;
	return list;
}

} } // namespace namespace qutim_sdk_0_3::irc

