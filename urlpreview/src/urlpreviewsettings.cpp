/*
	UrlPreviewPlugin

  Copyright (c) 2011 by Nicolay Izoderov <nico-izo@yandex.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "urlpreviewsettings.h"
#include <qutim/config.h>

using namespace qutim_sdk_0_3;

UrlPreviewSettings::UrlPreviewSettings()
	: ui(new Ui::UrlPreviewSettingsForm)
{
	ui->setupUi(this);
	lookForWidgetState(ui->maxFileSize);
	lookForWidgetState(ui->maxWidth);
	lookForWidgetState(ui->maxHeight);
	lookForWidgetState(ui->youtubePreview);
	lookForWidgetState(ui->imagesPreview);
}

void UrlPreviewSettings::loadImpl()
{
	Config cfg("urlpreview");
	ui->maxFileSize->setValue(cfg.value("maxFileSize", 100000));
	ui->maxWidth->setValue(cfg.value("maxWidth", 800));
	ui->maxHeight->setValue(cfg.value("maxHeight", 600));
	ui->youtubePreview->setChecked(cfg.value("youtubePreview", true));
	ui->imagesPreview->setChecked(cfg.value("imagesPreview", true));
}

void UrlPreviewSettings::saveImpl()
{
	Config cfg("urlpreview");
	cfg.setValue("maxFileSize", ui->maxFileSize->value());
	cfg.setValue("maxWidth", ui->maxWidth->value());
	cfg.setValue("maxHeight", ui->maxHeight->value());
	cfg.setValue("youtubePreview", ui->youtubePreview->isChecked());
	cfg.setValue("imagesPreview", ui->imagesPreview->isChecked());
	emit reloadSettings();
}

void UrlPreviewSettings::cancelImpl()
{
	loadImpl();
}
