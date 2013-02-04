/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
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

#include "yandexnarodsettings.h"
#include <qutim/configbase.h>

using namespace qutim_sdk_0_3;

YandexNarodSettings::YandexNarodSettings()
{
	m_authorizator = 0;
	ui.setupUi(this);
	ui.labelStatus->setText(QString());

	lookForWidgetState(ui.editLogin);
	lookForWidgetState(ui.editPasswd);
}

YandexNarodSettings::~YandexNarodSettings()
{
}

void YandexNarodSettings::loadImpl()
{
	Config config;
	config.beginGroup("yandex/disk");
	ui.editLogin->setText(config.value("login", QString()));
	ui.editPasswd->setText(config.value("passwd", QString(), Config::Crypted));
}

void YandexNarodSettings::saveImpl()
{
	Config config;
	config.beginGroup("yandex/disk");
	config.setValue("login", ui.editLogin->text());
	config.setValue("passwd", ui.editPasswd->text(), Config::Crypted);
}

void YandexNarodSettings::cancelImpl()
{
}

void YandexNarodSettings::on_testButton_clicked()
{
	if (m_authorizator)
		m_authorizator->deleteLater();

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

