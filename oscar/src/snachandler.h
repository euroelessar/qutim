/****************************************************************************
 *  snachandler.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef SNACHANDLER_H
#define SNACHANDLER_H

#include <QObject>
#include <QPair>
#include "snac.h"
#include "util.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class AbstractConnection;

typedef QPair<quint16, quint16> SNACInfo;

class LIBOSCAR_EXPORT SNACHandler
{
public:
	virtual ~SNACHandler();
	const QList<SNACInfo> &infos() { return m_infos; }
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac) = 0;
protected:
	QList<SNACInfo> m_infos;
};

} } // namespace qutim_sdk_0_3::oscar

Q_DECLARE_INTERFACE(qutim_sdk_0_3::oscar::SNACHandler, "org.qutim.oscar.SNACHandler")

#endif // SNACHANDLER_H
