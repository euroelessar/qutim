/****************************************************************************
 *  pidgin.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef PIDGIN_H_
#define PIDGIN_H_
#include "../../include/qutim/historymanager.h"

namespace HistoryManager {

class pidgin : public HistoryImporter
{
public:
	pidgin();
	virtual ~pidgin();
	virtual void loadMessages(const QString &path);
	virtual bool validate(const QString &path);
	virtual QString name();
	virtual QIcon icon();
	virtual bool needCharset() { return true; }
};

}

#endif /*PIDGIN_H_*/
