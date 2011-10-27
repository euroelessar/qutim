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
	lookForWidgetState(ui->HTML5Audio);
	lookForWidgetState(ui->HTML5Video);
}

void UrlPreviewSettings::loadImpl()
{
	Config cfg;
	cfg.beginGroup("urlPreview");
	ui->maxFileSize->setValue(cfg.value("maxFileSize", 100000));
	ui->maxWidth->setValue(cfg.value("maxWidth", 800));
	ui->maxHeight->setValue(cfg.value("maxHeight", 600));
	ui->youtubePreview->setChecked(cfg.value("youtubePreview", true));
	ui->imagesPreview->setChecked(cfg.value("imagesPreview", true));
	ui->HTML5Audio->setChecked(cfg.value("HTML5Audio", true));
	ui->HTML5Video->setChecked(cfg.value("HTML5Video", true));
	cfg.endGroup();
}

void UrlPreviewSettings::saveImpl()
{
	Config cfg;
	cfg.beginGroup("urlPreview");
	cfg.setValue("maxFileSize", ui->maxFileSize->value());
	cfg.setValue("maxWidth", ui->maxWidth->value());
	cfg.setValue("maxHeight", ui->maxHeight->value());
	cfg.setValue("youtubePreview", ui->youtubePreview->isChecked());
	cfg.setValue("imagesPreview", ui->imagesPreview->isChecked());
	cfg.setValue("HTML5Audio", ui->HTML5Audio->isChecked());
	cfg.setValue("HTML5Video", ui->HTML5Video->isChecked());
	cfg.endGroup();
}

void UrlPreviewSettings::cancelImpl()
{
	loadImpl();
}

UrlPreviewSettings::~UrlPreviewSettings()
{
	delete ui;
}
