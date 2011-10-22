/****************************************************************************
 * ulservice.h
 *  Copyright © 2011, Vsevolod Velichko <torkvema@gmail.com>.
 *  Licence: GPLv2 or later
 *
 ****************************************************************************
 *                                                                          *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/

#ifndef ULSERVICE_EM7KE8Q7
#define ULSERVICE_EM7KE8Q7

#include <qutim/message.h>

class UnityDock;
class QObject;
namespace qutim_sdk_0_3
{
class ChatSession;
}

class UnityLauncherService : public QObject
{
	Q_OBJECT
public:
	UnityLauncherService(QObject *p = NULL);
	~UnityLauncherService();
protected slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionDestroyed(QObject *session);
	void sendAlert();
	void setAlert(bool on);
private:
	int sessionCount;
	UnityDock *dock;
};

#endif /* end of include guard: ULSERVICE_EM7KE8Q7 */
