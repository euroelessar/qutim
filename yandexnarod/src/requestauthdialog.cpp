/****************************************************************************
 *  requestauthdialog.cpp
 *
 *  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
 *                     2010 by Nigmatullin Ruslan <euroelessar@ya.ru>
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

#include <qutim/libqutim_global.h>
#include "requestauthdialog.h"

YandexNarodRequestAuthDialog::YandexNarodRequestAuthDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
	ui.frameCaptcha->hide();
	this->setFixedHeight(180);
	this->setMaximumHeight(180);
	qutim_sdk_0_3::centerizeWidget(this);
}


YandexNarodRequestAuthDialog::~YandexNarodRequestAuthDialog()
{
	
}

void YandexNarodRequestAuthDialog::setCaptcha(QString imgurl) {
	this->setFixedHeight(305);
	this->setMaximumHeight(305);
	ui.frameCaptcha->show();
	ui.webCaptcha->setHtml("<img src='"+imgurl+"'>");
	ui.labelCaptcha->show();
}
