/****************************************************************************
 *  popupappearance.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "popupappearance.h"
#include <QLayout>
#include <QLayout>
#include "ui_popupappearance.h"

#include "qutim/notification.h"
#include <qutim/configbase.h>
#include <qutim/objectgenerator.h>
#include <qutim/thememanager.h>
#include <qutim/servicemanager.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/chatunit.h>

namespace KineticPopups
{

using namespace qutim_sdk_0_3;

PopupAppearance::PopupAppearance () :
	ui(new Ui::AppearanceSettings)
{
	ui->setupUi(this);
	connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)));
	connect(ui->pushButton, SIGNAL(clicked(bool)), SLOT(onTestButtonClicked()));
}

PopupAppearance::~PopupAppearance()
{
	delete ui;
}



void PopupAppearance::loadImpl()
{
	ui->comboBox->blockSignals(true);

	Config cfg("behavior");
	cfg.beginGroup("popup");
	getThemes();
	QString name = cfg.value("themeName", "default");
	int index = ui->comboBox->findText(name);
	ui->comboBox->setCurrentIndex(index);
	preview();

	ui->comboBox->blockSignals(false);
}


void PopupAppearance::saveImpl()
{
	Config cfg("behavior");
	cfg.beginGroup("popup");
	cfg.setValue("themeName", ui->comboBox->currentText());
	cfg.endGroup();
	cfg.sync();
}

void PopupAppearance::cancelImpl()
{
	loadImpl();
}

void PopupAppearance::getThemes()
{
	ui->comboBox->clear();
	QStringList list = ThemeManager::list("quickpopup");
	foreach (QString theme,list)
		ui->comboBox->addItem(theme, theme);
	ui->comboBox->setCurrentIndex(0);
}

void PopupAppearance::onCurrentIndexChanged(int)
{
	emit modifiedChanged(true);
	preview();
}

void PopupAppearance::onTestButtonClicked()
{
	Config cfg("behavior");
	cfg.beginGroup("popup");
	QString name = cfg.value("themeName", "default");
	cfg.setValue("themeName", ui->comboBox->currentText());
	cfg.sync();

	NotificationRequest request(Notification::System);
	request.send();

	request.setText(tr("Simple popup"));
	request.setTitle(tr("Preview"));

	NotificationAction act(tr("Test again"), this, SLOT(onTestButtonClicked()));
	request.addAction(act);
	request.setType(Notification::IncomingMessage);
	request.send();

	cfg.setValue("themeName", name);
	cfg.sync();
}

void PopupAppearance::preview()
{
}

} //namespace KineticPopups
