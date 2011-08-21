/****************************************************************************
 *  ircaccountnicksettings.cpp
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
