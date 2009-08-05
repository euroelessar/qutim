/*
    aboutInfo

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/
#include "aboutinfo.h"
#include <QResource>
#include <QCoreApplication>

aboutInfo::aboutInfo(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
	ui.label->setText(ui.label->text().arg(QCoreApplication::applicationVersion(), QLatin1String(qVersion())));
	ui.licenseTextBrowser->setText(QLatin1String((const char *)QResource( ":/GPL" ).data()));
}

aboutInfo::~aboutInfo()
{

}
