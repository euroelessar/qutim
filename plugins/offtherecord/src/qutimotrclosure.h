/*
 * psiotrclosure.hpp
 *
 * Copyright (C) Timo Engel (timo-e@freenet.de), Berlin 2007.
 * This program was written as part of a diplom thesis advised by
 * Prof. Dr. Ruediger Weis (PST Labor)
 * at the Technical University of Applied Sciences Berlin.
 * Copyright (C) Stanislav (proDOOMman) Kosolapov <prodoomman@shell.tor.hu>, 2010
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef PSIOTRCLOSURE_H_
#define PSIOTRCLOSURE_H_

#include <QObject>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <qutim/protocol.h>
#include <qutim/chatunit.h>
#include "smpdialog.h"

extern "C"
{
typedef unsigned int OtrlPolicy;
/*
#define OTRL_POLICY_ALLOW_V1			0x01
#define OTRL_POLICY_ALLOW_V2			0x02
#define OTRL_POLICY_REQUIRE_ENCRYPTION		0x04
#define OTRL_POLICY_SEND_WHITESPACE_TAG		0x08
#define OTRL_POLICY_WHITESPACE_START_AKE	0x10
#define OTRL_POLICY_ERROR_START_AKE		0x20

#define OTRL_POLICY_VERSION_MASK (OTRL_POLICY_ALLOW_V1 | OTRL_POLICY_ALLOW_V2)
*/
/* For v1 compatibility */
/*#define OTRL_POLICY_NEVER			0x00
#define OTRL_POLICY_OPPORTUNISTIC \
	( OTRL_POLICY_ALLOW_V1 | \
	OTRL_POLICY_ALLOW_V2 | \
	OTRL_POLICY_SEND_WHITESPACE_TAG | \
	OTRL_POLICY_WHITESPACE_START_AKE | \
	OTRL_POLICY_ERROR_START_AKE )
#define OTRL_POLICY_MANUAL \
	( OTRL_POLICY_ALLOW_V1 | \
	OTRL_POLICY_ALLOW_V2 )
#define OTRL_POLICY_ALWAYS \
	( OTRL_POLICY_ALLOW_V1 | \
	OTRL_POLICY_ALLOW_V2 | \
	OTRL_POLICY_REQUIRE_ENCRYPTION | \
	OTRL_POLICY_WHITESPACE_START_AKE | \
	OTRL_POLICY_ERROR_START_AKE )
#define OTRL_POLICY_DEFAULT OTRL_POLICY_OPPORTUNISTIC*/
}

class QAction;
class QMenu;

namespace OtrSupport
{
class PsiOtrPlugin;
class OtrMessaging;

//-----------------------------------------------------------------------------

class OtrClosure : public QObject
{
	Q_OBJECT
	
public:
	OtrClosure(qutim_sdk_0_3::ChatUnit *unit);
	~OtrClosure();
	qutim_sdk_0_3::ChatUnit *unit() const;
	int getPolicy() const;
	OtrMessaging* getMessaging(){return m_otr;}
	int getCurrentId(){int id = m_id; m_id++; return id;}
	void updateMessageState();
	int getState(){return m_otr->getMessageStateInt(m_myAccount,m_otherJid,m_item);}
public slots:
	void updateState();
	
private:
	int m_id;
	int m_policy;
	OtrMessaging* m_otr;
	qutim_sdk_0_3::ChatUnit *m_unit;
	QString       m_myAccount;
	QString       m_otherJid;
	TreeModelItem m_item;
	qint16 prevState;
	QTimer *m_timer;
	
public slots:
	void initiateSession(bool b);
	void endSession(bool b);
	void verifyFingerprint(bool b);
	void sessionID(bool b);
	void fingerprint(bool b);
	void setPolicy(int pol);
};

//-----------------------------------------------------------------------------

} // namespace qutimotr

#endif
