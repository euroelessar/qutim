/*
    urlpreviewSettings

		Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "settings.h"

urlpreviewSettings::urlpreviewSettings(QString profile_name) 
{
	ui.setupUi(this);
	m_profile_name = profile_name;

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_urlpreview");
	ui.cbEnIn->setChecked(settings.value("enable_in", true).toBool());
	ui.cbEnOut->setChecked(settings.value("enable_out", false).toBool());
	ui.cbDisableTextHtml->setChecked(settings.value("disable_text_html", true).toBool());
        ui.cbIgnoreSSLWarn->setChecked(settings.value("ignore_sslwarn", false).toBool());
	ui.editTpl->setPlainText(settings.value("template").toString());
	ui.editImageMaxFileSize->setText(settings.value("image/maxfilesize", 100000).toString());
	ui.editImageMaxW->setText(settings.value("image/maxwidth", 400).toString());
	ui.editImageMaxH->setText(settings.value("image/maxheight", 600).toString());
	ui.editImageTpl->setPlainText(settings.value("image/template").toString());
	ui.cbEnYoutube->setChecked(settings.value("youtube/enable", true).toBool());
	ui.editYoutubeTpl->setPlainText(settings.value("youtube/template").toString());

}

urlpreviewSettings::~urlpreviewSettings()
{
	
}

void urlpreviewSettings::saveSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_urlpreview");
	settings.setValue("enable_in", ui.cbEnIn->isChecked());
	settings.setValue("enable_out", ui.cbEnOut->isChecked());
	settings.setValue("disable_text_html", ui.cbDisableTextHtml->isChecked());
        settings.setValue("ignore_sslwarn", ui.cbIgnoreSSLWarn->isChecked());
	settings.setValue("template", ui.editTpl->toPlainText());

	settings.setValue("image/maxfilesize", ui.editImageMaxFileSize->text().toInt());
	settings.setValue("image/maxwidth", ui.editImageMaxW->text().toInt());
	settings.setValue("image/maxheight", ui.editImageMaxH->text().toInt());
	settings.setValue("image/template", ui.editImageTpl->toPlainText());

	settings.setValue("youtube/enable", ui.cbEnYoutube->isChecked());
	settings.setValue("youtube/template", ui.editYoutubeTpl->toPlainText());
	
	emit settingsChanged();
}

