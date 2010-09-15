/****************************************************************************
 *  servicechooser.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef SERVICECHOOSER_H
#define SERVICECHOOSER_H

#include <QObject>
#include <qutim/startupmodule.h>

namespace qutim_sdk_0_3
{
	class ExtensionInfo;
}

namespace Core
{
	using namespace qutim_sdk_0_3;

	class ServiceChooser : public QObject
	{
		Q_OBJECT
	public:
		ServiceChooser(QObject* parent = 0);
		static const char *className(const ExtensionInfo &info);
		static QString html(const ExtensionInfo &info);
	};

}
#endif // SERVICECHOOSER_H
