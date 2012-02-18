/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
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

#include "imagepubsettings.h"

imagepubSettings::imagepubSettings(QString profile_name) 
{
	ui.setupUi(this);
	m_profile_name = profile_name;

	ui.labelAbout->setText(ui.labelAbout->text().replace("%VERSION%", "0.1.4"));

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_imagepub");
	if (settings.value("main/msgtemplate").isValid()) {
		ui.textTpl->setText(settings.value("main/msgtemplate").toString());
	}
	else {
		ui.textTpl->setText("Image sent: %N (%S bytes)\n%U");
	}

	ui.comboBoxServ->addItem(QIcon(":/icons/imagepub-serv-imageshack.png"), "imageshack.us", "imageshack");

	QStringList sids;
	foreach (QString skey, settings.allKeys()) {
		QRegExp rx("^serv\\-([^/]+)");
		if (rx.indexIn(skey)!=-1) {
			if (sids.indexOf(rx.cap(1))==-1) sids.append(rx.cap(1));
		}
	}

	foreach (QString sid, sids) {
		if (settings.value("serv-"+sid+"/name").isValid()) {
			ui.comboBoxServ->addItem(settings.value("serv-"+sid+"/name").toString(), sid);
			QStringList exts; exts<<"ico"<<"png"<<"gif"<<"jpg";
			foreach (QString ext, exts) {
				if (QFile::exists(settings.fileName().section('/', 0, -2) + "/imagepub-icons/serv-"+sid+"."+ext)) {
					ui.comboBoxServ->setItemIcon(ui.comboBoxServ->findData(sid), QIcon(settings.fileName().section('/', 0, -2) + "/imagepub-icons/serv-"+sid+"."+ext));
				}
			}
		}
	}

	if (settings.value("main/service").isValid()) {
		ui.comboBoxServ->setCurrentIndex(ui.comboBoxServ->findData(settings.value("main/service").toString()));
	}
}

imagepubSettings::~imagepubSettings()
{
	
}

void imagepubSettings::saveSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_imagepub");
	settings.setValue("main/service", ui.comboBoxServ->itemData(ui.comboBoxServ->currentIndex()).toString());
	settings.setValue("main/msgtemplate", ui.textTpl->toPlainText());
}

