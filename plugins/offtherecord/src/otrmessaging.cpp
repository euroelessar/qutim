/*
 * OtrMessaging.cpp - interface to libotr
 * Copyright (C) 2007  Timo Engel (timo-e@freenet.de)
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

#include "otrmessaging.h"
#include "otrinternal.h"

namespace OtrSupport
{

//-----------------------------------------------------------------------------

OtrMessaging::OtrMessaging(Policy policy, OtrlUserState userstate)
    : m_otrPolicy(policy),
      m_impl(new OtrInternal(m_otrPolicy, userstate))
{
}

void OtrMessaging::requestAuth(TreeModelItem &item, bool agree, QString answer, QString question)
{
    m_impl->requestAuth(item,agree,answer,question);
}

//-----------------------------------------------------------------------------

OtrMessaging::~OtrMessaging()
{
    delete m_impl;
}

//-----------------------------------------------------------------------------

QString OtrMessaging::encryptMessage(const QString& from, const QString& to,
                                     const QString& message, TreeModelItem &item)
{
    return m_impl->encryptMessage(from, to, message, item);
}

//-----------------------------------------------------------------------------

QString OtrMessaging::decryptMessage(const QString& from, const QString& to,
                                     const QString& message, TreeModelItem &item)
{
    return m_impl->decryptMessage(from, to, message, item);
}

//-----------------------------------------------------------------------------

QList<Fingerprint> OtrMessaging::getFingerprints()
{
    return m_impl->getFingerprints();
}

//-----------------------------------------------------------------------------

void OtrMessaging::verifyFingerprint(const OtrSupport::Fingerprint& fingerprint,
                                     bool verified)
{
    m_impl->verifyFingerprint(fingerprint, verified);
}

//-----------------------------------------------------------------------------

void OtrMessaging::deleteFingerprint(const OtrSupport::Fingerprint& fingerprint)
{
    m_impl->deleteFingerprint(fingerprint);
}

void OtrMessaging::deleteKey(QString account, QString protocol)
{
    m_impl->deleteKey(account,protocol);
}

//-----------------------------------------------------------------------------

QHash<QString, QHash<QString, QString> > OtrMessaging::getPrivateKeys()
{
    return m_impl->getPrivateKeys();
}

//-----------------------------------------------------------------------------

void OtrMessaging::startSession(const QString& account, const QString& jid, TreeModelItem &item, int pol)
{
    m_impl->startSession(account, jid, item, pol);
}

//-----------------------------------------------------------------------------

void OtrMessaging::endSession(const QString& account, const QString& jid, TreeModelItem &item)
{
    m_impl->endSession(account, jid, item);
}
//-----------------------------------------------------------------------------

MessageState OtrMessaging::getMessageState(const QString& thisJid,
                                              const QString& remoteJid,
                                              TreeModelItem &item)
{
    return m_impl->getMessageState(thisJid, remoteJid, item);
}

//-----------------------------------------------------------------------------

QString OtrMessaging::getMessageStateString(const QString& thisJid,
                                             const QString& remoteJid,
                                             TreeModelItem &item)
{
    return m_impl->getMessageStateString(thisJid, remoteJid, item);
}

int OtrMessaging::getMessageStateInt(const QString& thisJid,
                              const QString& remoteJid,
                              TreeModelItem &item)
{
    return m_impl->getMessageStateIntCode(thisJid, remoteJid, item);
}

//-----------------------------------------------------------------------------

QString OtrMessaging::getSessionId(const QString& thisJid,
                                   const QString& remoteJid,
                                   TreeModelItem &item)
{
    return m_impl->getSessionId(thisJid, remoteJid, item);
}

//-----------------------------------------------------------------------------

void OtrMessaging::setPolicy(OtrSupport::Policy policy)
{
    m_otrPolicy = policy;
}

//-----------------------------------------------------------------------------

Policy OtrMessaging::getPolicy()
{
    return m_otrPolicy;
}

//-----------------------------------------------------------------------------

} // namespace qutimotr
