/****************************************************************************
 *  capabilty.h
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

#ifndef CAPABILTY_H
#define CAPABILTY_H

#include <QByteArray>

class Capabilty
{
public:
    Capabilty();
private:
	QByteArray m_data;
	bool m_is_short;
};

#endif // CAPABILTY_H
