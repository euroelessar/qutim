/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nicolay Izoderov <nico-izo@yandex.ru>
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
	lookForWidgetState(ui->yandexRCA);
	lookForWidgetState(ui->exceptionList);
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
	ui->yandexRCA->setChecked(cfg.value("yandexRichContent", true));
	ui->exceptionList->insertPlainText(QStringList(cfg.value("exceptionList", QStringList())).join(";"));
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
	cfg.setValue("yandexRichContent", ui->yandexRCA->isChecked());
	cfg.setValue("exceptionList", ui->exceptionList->toPlainText().split(";", QString::SkipEmptyParts));
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

