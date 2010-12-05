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
#include "settingsextension.h"
#include <qutim/objectgenerator.h>

namespace qutim_sdk_0_3 {

namespace oscar {

QList<SettingsExtension*> settingsExtensions()
{
	static QList<SettingsExtension*> list;
	static bool inited = false;
	if (!inited && ObjectGenerator::isInited()) {
		foreach(const ObjectGenerator *gen, ObjectGenerator::module<SettingsExtension>())
			list << gen->generate<SettingsExtension>();
		inited = true;
	}
	return list;
}

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
	Config cfg = IcqProtocol::instance()->config();
	Config general = cfg.group("general");
	QString codecName = general.value("codec", QTextCodec::codecForLocale()->name());
	QTextCodec *codec = QTextCodec::codecForName(codecName.toLatin1());
	codecName = codec->name().toLower();

	for (int i = 0; i < ui->codepageBox->count(); ++i) {
		QString curName = ui->codepageBox->itemText(i).toLower();
		bool found = codecName == curName;
		if (!found) {
			foreach(const QByteArray &codecName, codec->aliases())
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

	DataItem item;
	item.addSubitem(DataItem("avatars", tr("Don't send requests for avatarts"),
							 !general.value("avatars", true)));
	foreach (SettingsExtension *extension, settingsExtensions())
		extension->loadSettings(item, cfg);
	m_extSettings.reset(AbstractDataForm::get(item));
	if (m_extSettings) {
		connect(m_extSettings.data(), SIGNAL(changed()), SLOT(extSettingsChanged()));
		ui->verticalLayout->insertWidget(2, m_extSettings.data());
	}
}

void IcqMainSettings::cancelImpl()
{
	loadImpl();
}

void IcqMainSettings::saveImpl()
{
	Config cfg = IcqProtocol::instance()->config();
	Config general = cfg.group("general");
	if (ui->codepageBox->currentIndex() == 0)
		general.setValue("codec", QTextCodec::codecForLocale()->name());
	else
		general.setValue("codec", ui->codepageBox->currentText());

	if (m_extSettings) {
		DataItem item = m_extSettings->item();
		foreach (SettingsExtension *extension, settingsExtensions())
			extension->saveSettings(item, cfg);
		general.setValue("avatars", !item.subitem("avatars").data<bool>());
		m_extSettings->clearState();
	}
	IcqProtocol::instance()->updateSettings();
}

void IcqMainSettings::extSettingsChanged()
{
	emit modifiedChanged(true);
}


} } // namespace qutim_sdk_0_3::oscar
