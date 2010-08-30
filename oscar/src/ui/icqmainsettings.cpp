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
	ui(new Ui::IcqMainSettings)
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
	Config config = IcqProtocol::instance()->config().group("general");
	bool avatars = !config.value("avatars", true);
	ui->avatarBox->setChecked(avatars);
	QString codecName = config.value("codec", QTextCodec::codecForLocale()->name());
	QTextCodec *codec = QTextCodec::codecForName(codecName.toLatin1());
	codecName = codec->name().toLower();
	config = IcqProtocol::instance()->config().group("reconnect");
	bool reconnect = config.value("enabled", true);
	ui->reconnectBox->setChecked(reconnect);

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
	Config config = IcqProtocol::instance()->config().group("general");
	config.setValue("avatars", !ui->avatarBox->isChecked());
	if (ui->codepageBox->currentIndex() == 0)
		config.setValue("codec", QTextCodec::codecForLocale()->name());
	else
		config.setValue("codec", ui->codepageBox->currentText());
	config = IcqProtocol::instance()->config().group("reconnect");
	config.setValue("enabled", ui->reconnectBox->isChecked());
	config.sync();
	IcqProtocol::instance()->updateSettings();
}

} } // namespace qutim_sdk_0_3::oscar
