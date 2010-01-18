/****************************************************************************
 *  yandexnarodsettings.cpp
 *
 *  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
 *                     2010 by Nigmatullin Ruslan <euroelessar@ya.ru>
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

#include "yandexnarodsettings.h"
#include <qutim/configbase.h>

using namespace qutim_sdk_0_3;

YandexNarodSettings::YandexNarodSettings()
{
	m_authorizator = 0;
	ui.setupUi(this);
	ui.labelStatus->setText(NULL);

	ui.labelAbout->setText(ui.labelAbout->text().replace("%VERSION%", VERSION));

	connect(ui.btnTest, SIGNAL(clicked()), this,  SLOT(onTestClick()));

	lookForWidgetState(ui.editLogin);
	lookForWidgetState(ui.editPasswd);
	lookForWidgetState(ui.textTpl);
}

YandexNarodSettings::~YandexNarodSettings()
{
	
}

void YandexNarodSettings::loadImpl()
{
	ConfigGroup group = Config().group("yandex");
	ConfigGroup narod = group.group("narod");
	ui.editLogin->setText(group.value("login", QString()));
	ui.editPasswd->setText(group.value("passwd", QString(), Config::Crypted));
	ui.textTpl->setText(narod.value("template", QString("File sent: %N (%S bytes)\n%U")));
}

void YandexNarodSettings::saveImpl()
{
	ConfigGroup group = Config().group("yandex");
	ConfigGroup narod = group.group("narod");
	group.setValue("login", ui.editLogin->text());
	group.setValue("passwd", ui.editPasswd->text(), Config::Crypted);
	narod.setValue("template", ui.textTpl->toPlainText());
	group.sync();
}

void YandexNarodSettings::cancelImpl()
{
}

void YandexNarodSettings::onTestClick()
{
	if (m_authorizator) {
		m_authorizator->deleteLater();
	}
	m_authorizator = new YandexNarodAuthorizator(this);
	connect(m_authorizator, SIGNAL(result(YandexNarodAuthorizator::Result,QString)),
			this, SLOT(authorizationResult(YandexNarodAuthorizator::Result,QString)));
	m_authorizator->requestAuthorization(ui.editLogin->text(), ui.editPasswd->text());
}

void YandexNarodSettings::authorizationResult(YandexNarodAuthorizator::Result result, const QString &error)
{
	ui.labelStatus->setText(m_authorizator->resultString(result, error));
	m_authorizator->deleteLater();
	m_authorizator = 0;
}
