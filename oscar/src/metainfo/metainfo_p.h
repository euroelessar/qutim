/****************************************************************************
 *  metainfo_p.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef METAINFO_P_H
#define METAINFO_P_H

#include "icqaccount.h"
#include <QTimer>
#include "abstractmetarequest_p.h"
#include "snachandler.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class MetaInfo: public QObject, public SNACHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SNACHandler)
public:
	MetaInfo();
	static MetaInfo &instance() { Q_ASSERT(self); return *self; }
	void handleSNAC(AbstractConnection *conn, const SNAC &snac);
	void addRequest(AbstractMetaRequest *request);
	bool removeRequest(AbstractMetaRequest *request);
	quint16 nextId() { return ++m_sequence; }
private slots:
	void onNewAccount(qutim_sdk_0_3::Account *account);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
private:
	quint16 m_sequence;
	QHash<quint16, AbstractMetaRequest*> m_requests;
	static MetaInfo *self;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // METAINFO_P_H
