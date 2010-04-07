/****************************************************************************
 *  licq.h
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

#ifndef LICQ_H_
#define LICQ_H_
#include "include/qutim/historymanager.h"

namespace HistoryManager {

class licq : public HistoryImporter
{
public:
	licq();
	virtual ~licq();
	virtual void loadMessages(const QString &path);
	virtual bool validate(const QString &path);
	virtual QString name();
	virtual QIcon icon();
	virtual bool needCharset() { return true; }
};

}

#endif /*LICQ_H_*/
