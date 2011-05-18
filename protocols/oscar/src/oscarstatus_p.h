/****************************************************************************
 *  oscarstatus_p.h
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

#ifndef OSCARSTATUS_P_H
#define OSCARSTATUS_P_H

#include "oscarstatus.h"

namespace qutim_sdk_0_3 {

namespace oscar {

typedef QSet<QString> CapsTypes;
CapsTypes &capsTypes();

class OscarStatusList : public QList<OscarStatusData>
{
public:
	void insert(const OscarStatusData &status);
};

} } // namespace qutim_sdk_0_3::oscar

Q_DECLARE_METATYPE(qutim_sdk_0_3::oscar::CapabilityHash);

#endif // OSCARSTATUS_P_H
