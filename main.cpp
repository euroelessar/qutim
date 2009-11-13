/****************************************************************************
 *  main.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <icqaccount.h>
#include <icqprotocol.h>
#include <QSettings>
#include <QDir>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QSettings settings("testicqlogin.ini", QSettings::IniFormat);	
	QString uid = settings.value("uid", "").toString(); 
	if(uid.isEmpty())
		qFatal("testicqlogin is not found");
	IcqProtocol protocol;	
	IcqAccount acc(uid);
	acc.setStatus(Online);
	return a.exec();
}
