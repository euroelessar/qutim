/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

