/****************************************************************************
 *  abstractmetarequest_p.h
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

#ifndef ABSTRACTMETAREQUEST_P_H
#define ABSTRACTMETAREQUEST_P_H

#include "abstractmetarequest.h"
#include <QTimer>

namespace qutim_sdk_0_3 {

namespace oscar {

class AbstractMetaRequestPrivate : public QSharedData
{
public:
	quint16 id;
	IcqAccount *account;
	bool ok;
	mutable QTimer timer;
	AbstractMetaRequest::ErrorType errorType;
	QString errorString;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ABSTRACTMETAINFOREQUEST_P_H
