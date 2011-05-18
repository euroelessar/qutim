/****************************************************************************
 *  kopete.h
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

#ifndef KOPETE_H_
#define KOPETE_H_
#include "../../include/qutim/historymanager.h"

namespace HistoryManager {

class kopete : public HistoryImporter
{
public:
	kopete();
	virtual ~kopete();
	bool validate(const QString &kopete_id, const QString &id);
	QString guessFromList(const QString &kopete_id, const QStringList &list);
	QString guessID(const QString &kopete_id);
	virtual void loadMessages(const QString &path);
	virtual bool validate(const QString &path);
	virtual QString name();
	virtual QIcon icon();
};

}

#endif /*KOPETE_H_*/
