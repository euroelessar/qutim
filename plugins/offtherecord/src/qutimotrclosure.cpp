/*
 * qutimotrclosure.cpp
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

#include "qutimotrclosure.h"
#include "otrmessaging.h"
#include "otrcrypt.h"
#include <qutim/account.h>
#include <qutim/chatsession.h>

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>

namespace OtrSupport
{

using namespace qutim_sdk_0_3;

//-----------------------------------------------------------------------------

OtrClosure::OtrClosure(ChatUnit *unit) : QObject(unit), m_unit(unit), m_item(unit)
{
	m_myAccount = m_item.m_account_name;
	m_otherJid = m_item.m_item_name;
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(updateState()));
    m_timer->start(1000);
    m_id = 0;
	prevState = 666;
	Config config = unit->account()->config("otr");
	m_policy = config.value(unit->id(), -1);
	m_otr = OTRCrypt::instance()->connectionForPolicy(m_policy);
}

//-----------------------------------------------------------------------------

OtrClosure::~OtrClosure()
{
}

//-----------------------------------------------------------------------------

void OtrClosure::initiateSession(bool b)
{
	Q_UNUSED(b);
	m_otr->startSession(m_unit->account()->id(), m_unit->id(), m_item, m_policy);
}

void OtrClosure::setPolicy(int pol)
{
	if (m_policy != pol)
		emit endSession(true);
	m_otr = OTRCrypt::instance()->connectionForPolicy(pol);
	Config config = m_unit->account()->config("otr");
	config.setValue(m_unit->id(), pol);
}

//-----------------------------------------------------------------------------

void OtrClosure::verifyFingerprint(bool)
{
    SMPdialog dialog(m_item,m_otr,NULL);
    dialog.exec();
}

//-----------------------------------------------------------------------------

void OtrClosure::sessionID(bool)
{
    QString sId = m_otr->getSessionId(m_myAccount,
                                      m_otherJid.toStdString().c_str(),m_item);
    QString msg;

    if (sId.isEmpty() ||
        (sId.compare(QString("<b></b>")) == 0) ||
        (sId.compare(QString("<b> </b>")) == 0) ||
        (sId.compare(QString(" <b> </b>")) == 0))
    {
        msg = tr("no active encrypted session");
    }
    else
    {
        msg = tr("Session ID of connection from account %1 to %2 is: ").arg(m_myAccount).arg(m_otherJid)+"<br/>" + sId + ".";
    }

    QMessageBox mb(QMessageBox::Information, "qutim-otr", msg, NULL, NULL,
                   Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    mb.setTextFormat(Qt::RichText);
    mb.exec();
}

//-----------------------------------------------------------------------------

void OtrClosure::endSession(bool b)
{
        Q_UNUSED(b);
        m_otr->endSession(m_myAccount, m_otherJid, m_item);
		OTRCrypt::instance()->updateAction(this);
}

//-----------------------------------------------------------------------------

void OtrClosure::fingerprint(bool)
{
    QString fingerprint =  (m_otr->getPrivateKeys().value(m_myAccount).value(m_item.m_protocol_name,
                                                                             tr("no private key for ") +
                                                                             m_myAccount));

        QString msg(tr("Fingerprint for account %1 is:").arg(m_myAccount)+"\n" + fingerprint + ".");

        QMessageBox mb(QMessageBox::Information, "qutim-otr",
                   msg, NULL, NULL,
                   Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        mb.exec();
}

ChatUnit *OtrClosure::unit() const
{
	return m_unit;
}

int OtrClosure::getPolicy() const
{
	return m_policy;
}

//-----------------------------------------------------------------------------

void OtrClosure::updateState()
{
    qint16 newState = this->getState();
	if (prevState == 666) {
        prevState = newState;
    } else if (prevState != newState) {
        bool enabled = false;
		
		OtrMessaging *otr = OTRCrypt::instance()->connectionForPolicy(-1);
		if (OTRCrypt::instance()->notifyUser()) {
			if (otr->getPolicy() != PolicyOff)
				enabled = true;

			if (m_policy == 0)
				enabled = false;
			else if (m_policy > 0)
				enabled = true;
		}
        if(!enabled)
            return;
        prevState = newState;
        QString state = m_otr->getMessageStateString(m_myAccount,m_otherJid,m_item);
		Message message;
		message.setChatUnit(m_item.unit());
		message.setProperty("service", true);
		message.setProperty("store", false);
		message.setIncoming(true);
		message.setText(tr("OTR state is changed. New state is [%1]").arg(state));
		ChatLayer::get(message.chatUnit(), true)->append(message);
    }
}

void OtrClosure::updateMessageState()
{
}

//-----------------------------------------------------------------------------

} // namespace
