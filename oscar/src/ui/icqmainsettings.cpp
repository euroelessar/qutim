/****************************************************************************
 *  icqmainsettings.cpp
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

#include "icqmainsettings.h"
#include "icqprotocol.h"
#include "icqaccount.h"
#include "util.h"
#include <QTextCodec>
#include "ui_icqmainsettings.h"

namespace qutim_sdk_0_3 {

namespace oscar {

IcqMainSettings::IcqMainSettings() :
	ui(new Ui::IcqMainSettings), m_config(IcqProtocol::instance()->config())
{
	ui->setupUi(this);
	/*foreach(int codec, QTextCodec::availableMibs())
	 ui->codepageBox->addItem(QIcon(), QTextCodec::codecForMib(codec)->name());*/
	listenChildrenStates();
}

IcqMainSettings::~IcqMainSettings()
{
	delete ui;
}

void IcqMainSettings::loadImpl()
{
	bool avatars = !m_config.group("general").value("avatars", QVariant(true)).toBool();
	ui->avatarBox->setChecked(avatars);
	bool reconnect = m_config.group("reconnect").value("enabled", QVariant(true)).toBool();
	ui->reconnectBox->setChecked(reconnect);
	QString codecName = m_config.group("general").value("codec", "System").toString();
	QTextCodec *codec = QTextCodec::codecForName(codecName.toLatin1());
	codecName = codec->name().toLower();

	for (int i = 0; i < ui->codepageBox->count(); ++i) {
		QString curName = ui->codepageBox->itemText(i).toLower();
		bool found = codecName == curName;
		if (!found) {
			foreach(const QByteArray codecName, codec->aliases())
			{
				if (QString::fromLatin1(codecName).toLower() == curName) {
					found = true;
					break;
				}
			}
		}
		if (found) {
			ui->codepageBox->setCurrentIndex(i);
			break;
		}
	}
}

void IcqMainSettings::cancelImpl()
{
}

void IcqMainSettings::saveImpl()
{
	m_config.group("general").setValue("avatars", !ui->avatarBox->isChecked());
	m_config.group("reconnect").setValue("enabled", ui->reconnectBox->isChecked());
	m_config.group("general").setValue("codec", ui->codepageBox->currentText());
	m_config.sync();
	IcqProtocol::instance()->updateSettings();
}

} } // namespace qutim_sdk_0_3::oscar
