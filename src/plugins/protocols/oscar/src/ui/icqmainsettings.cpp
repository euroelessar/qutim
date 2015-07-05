/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "icqmainsettings.h"
#include "../icqprotocol.h"
#include "../icqaccount.h"
#include "../util.h"
#include <QTextCodec>
#include <QVBoxLayout>
#include "../settingsextension.h"
#include <qutim/objectgenerator.h>
#include <qutim/debug.h>

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

IcqMainSettings::IcqMainSettings()
{
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

	foreach (QString itr, codecs()) {
		if (codecName.compare(itr, Qt::CaseInsensitive) == 0)
			break;
		bool shouldBreak = false;
		foreach(const QByteArray &alias, codec->aliases()) {
			if (QString::fromLatin1(alias).compare(itr, Qt::CaseInsensitive) == 0) {
				codecName = itr;
				shouldBreak = true;
				break;
			}
		}
		if (shouldBreak)
			break;
	}
	if (codecName == QLatin1String("System"))
		codecName = codecs()[0];
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
	setItem(item);
}

void IcqMainSettings::cancelImpl()
{
	loadImpl();
}

void IcqMainSettings::saveImpl(const qutim_sdk_0_3::DataItem &mainItem)
{
	Config cfg = IcqProtocol::instance()->config();
	foreach (SettingsExtension *extension, settingsExtensions())
		extension->saveSettings(mainItem, cfg);

	cfg.beginGroup("general");
	DataItem item = mainItem.subitem("mainSettings");
	cfg.setValue("avatars", !item.subitem("avatars").data<bool>());
	QString codecName = item.subitem("codec").data<QString>();
	if (codecName == systemCodec)
		codecName = QTextCodec::codecForLocale()->name();
	cfg.setValue("codec", codecName);
	cfg.setValue("aimContacts", item.subitem("aimContacts").data<bool>());
	cfg.endGroup();
	IcqProtocol::instance()->updateSettings();
}


} } // namespace qutim_sdk_0_3::oscar

