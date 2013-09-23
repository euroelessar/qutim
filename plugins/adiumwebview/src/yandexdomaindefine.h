/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Roman Tretyakov <roman@trett.ru>
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
#pragma once
#include <QLocale>

struct YandexDomainDefine 
{
	YandexDomainDefine(){}
	~YandexDomainDefine(){}
	QString defineDomain(){
		switch(QLocale::system().country()) {
		case QLocale::Kazakhstan:
			return "kz";
			break;
		case QLocale::Ukraine:
			return "ua";
			break;
		case QLocale::Belarus:
			return "bl";
			break;
		case QLocale::Russia:
			return "ru";
			break;
		case QLocale::Turkey:
			return "com.tr";
		default:
			return "com";
		}
	}
	QString getDomain(){
		return "http://yandex." + defineDomain() + "/";
	}
};
