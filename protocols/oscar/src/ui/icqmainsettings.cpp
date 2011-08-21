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
#include "../icqprotocol.h"
#include "../icqaccount.h"
#include "../util.h"
#include <QTextCodec>
#include <QVBoxLayout>
#include "../settingsextension.h"
#include <qutim/objectgenerator.h>

namespace qutim_sdk_0_3 {

namespace oscar {

static QList<SettingsExtension*> settingsExtensions()
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

static LocalizedString systemCodec = QT_TRANSLATE_NOOP("Codec", "System");

static QStringList codecs()
{
	static QStringList codecs;
	if (codecs.isEmpty()) {
		codecs << systemCodec
			   << "Apple Roman"
			   << "Big5"
			   << "Big5-HKSCS"
			   << "EUC-JP"
			   << "EUC-KR"
			   << "GB18030-0"
			   << "IBM 850"
			   << "IBM 866"
			   << "IBM 874"
			   << "ISO 2022-JP"
			   << "ISO 8859-1"
			   << "ISO 8859-2"
			   << "ISO 8859-3"
			   << "ISO 8859-4"
			   << "ISO 8859-5"
			   << "ISO 8859-6"
			   << "ISO 8859-7"
			   << "ISO 8859-8"
			   << "ISO 8859-9"
			   << "ISO 8859-10"
			   << "ISO 8859-13"
			   << "ISO 8859-14"
			   << "ISO 8859-15"
			   << "ISO 8859-16"
			   << "Iscii-Bng"
			   << "Iscii-Dev"
			   << "Iscii-Gjr"
			   << "Iscii-Knd"
			   << "Iscii-Mlm"
			   << "Iscii-Ori"
			   << "Iscii-Pnj"
			   << "Iscii-Tlg"
			   << "Iscii-Tml"
			   << "JIS X 0201"
			   << "JIS X 0208"
			   << "KOI8-R"
			   << "KOI8-U"
			   << "MuleLao-1"
			   << "ROMAN8"
			   << "Shift-JIS"
			   << "TIS-620"
			   << "TSCII"
				  /*<< "UTF-8"
			  << "UTF-16"
			  << "UTF-16BE"
			  << "UTF-16LE"*/
			   << "Windows-1250"
			   << "Windows-1251"
			   << "Windows-1252"
			   << "Windows-1253"
			   << "Windows-1254"
			   << "Windows-1255"
			   << "Windows-1256"
			   << "Windows-1257"
			   << "Windows-1258"
			   << "WINSAMI2";
	}
	return codecs;
}

IcqMainSettings::IcqMainSettings() :
	m_layout(new QVBoxLayout(this))
{
	m_layout->setMargin(0);
	/*foreach(int codec, QTextCodec::availableMibs())
  ui->codepageBox->addItem(QIcon(), QTextCodec::codecForMib(codec)->name());*/
}

IcqMainSettings::~IcqMainSettings()
{
}

void IcqMainSettings::loadImpl()
{
	Config cfg = IcqProtocol::instance()->config();
	Config general = cfg.group("general");
	QString codecName = general.value("codec", QTextCodec::codecForLocale()->name());
	QTextCodec *codec = QTextCodec::codecForName(codecName.toLatin1());
	if (!codec)
		codec = QTextCodec::codecForLocale();
	QString codecNameLower = codecName.toLower();

	foreach (QString itr, codecs()) {
		itr = itr.toLower();
		if (codecNameLower == itr)
			break;
		foreach(const QByteArray &alias, codec->aliases())
		{
			if (QString::fromLatin1(alias).toLower() == itr) {
				codecName = itr;
				goto codecNameFound;
			}
		}
	}

	codecNameFound:
		DataItem item;
	{
		DataItem subitem("mainSettings", tr("General"), QVariant());
		subitem.addSubitem(StringChooserDataItem("codec", tr("Codec"), codecs(), codecName));
		subitem.addSubitem(DataItem("avatars", tr("Don't send requests for avatarts"),
									!general.value("avatars", true)));
		subitem.addSubitem(DataItem("aimContacts", tr("Support AIM contacts"),
									general.value("aimContacts", false)));
		item.addSubitem(subitem);
	}
	foreach (SettingsExtension *extension, settingsExtensions())
		extension->loadSettings(item, cfg);
	m_extSettings.reset(AbstractDataForm::get(item));
	if (m_extSettings) {
		connect(m_extSettings.data(), SIGNAL(changed()), SLOT(extSettingsChanged()));
		m_layout->insertWidget(0, m_extSettings.data());
	}
}

void IcqMainSettings::cancelImpl()
{
	loadImpl();
}

void IcqMainSettings::saveImpl()
{
	if (m_extSettings) {
		Config cfg = IcqProtocol::instance()->config();
		DataItem item = m_extSettings->item();
		foreach (SettingsExtension *extension, settingsExtensions())
			extension->saveSettings(item, cfg);

		cfg.beginGroup("general");
		item = item.subitem("mainSettings");
		cfg.setValue("avatars", !item.subitem("avatars").data<bool>());
		QString codecName = item.subitem("codec").data<QString>();
		if (codecName == systemCodec)
			codecName = QTextCodec::codecForLocale()->name();
		cfg.setValue("codec", codecName);
		cfg.setValue("aimContacts", item.subitem("aimContacts").data<bool>());
		cfg.endGroup();

		m_extSettings->clearState();
	}
	IcqProtocol::instance()->updateSettings();
}

void IcqMainSettings::extSettingsChanged()
{
	emit modifiedChanged(true);
}


} } // namespace qutim_sdk_0_3::oscar
