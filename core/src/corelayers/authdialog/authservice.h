/****************************************************************************
 *  authservice.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H
#include <qutim/plugin.h>

namespace qutim_sdk_0_3
{
namespace Authorization
{
class Reply;
}
}

namespace Core {

using namespace qutim_sdk_0_3;
using namespace Authorization;

class AuthService : public QObject
{
	Q_CLASSINFO("Service","AuthorizationService")
	Q_OBJECT
public:
protected:
	bool event(QEvent *event);
protected:
	void handleReply(Reply *reply);
private slots:
	void onAccepted();
	void onRejected();
};

} // namespace Core

#endif // AUTHSERVICE_H
