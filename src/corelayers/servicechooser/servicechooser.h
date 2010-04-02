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

namespace Core
{
	class ServiceChooser : public QObject
	{
		Q_OBJECT
	public:
		ServiceChooser(QObject* parent = 0);
	};

}
#endif // SERVICECHOOSER_H
