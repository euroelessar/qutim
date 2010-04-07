/****************************************************************************
 *  psi.h
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

#ifndef PSI_H
#define PSI_H
#include "include/qutim/historymanager.h"

namespace HistoryManager {

class psi : public HistoryImporter
{
public:
    psi();
	QString decode(const QString &jid);
	QString logdecode(const QString &str);
	virtual void loadMessages(const QString &path);
	virtual bool validate(const QString &path);
	virtual QString name();
	virtual QIcon icon();
	virtual QList<ConfigWidget> config();
	virtual bool useConfig();
	virtual QString additionalInfo();
private:
	ConfigWidget m_config;
	QString m_account;
};

}

#endif // PSI_H
