/*
 * OtrInternal.cpp - manages the otr connection.
 *
 * Copyright (C) Timo Engel (timo-e@freenet.de), Berlin 2007.
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

#include "otrinternal.h"
#include "otrcrypt.h"

#include <assert.h>
#include <QString>
#include <gcrypt.h>
#include <qutim/systeminfo.h>
#include <qutim/buddy.h>
#include <qutim/chatsession.h>
#include <QtConcurrentRun>

using namespace qutim_sdk_0_3;

//-----------------------------------------------------------------------------

static const QString OTR_FINGERPRINTS_FILE = "otr.fingerprints";
static const QString OTR_KEYS_FILE = "otr.keys";

//-----------------------------------------------------------------------------

QString OtrInternal::otrlMessageTypeToString(const OtrlMessageType& type)
{
    if (type == OTRL_MSGTYPE_NOTOTR)
    {
        return tr("no OTR Message");
    }
    else if (type == OTRL_MSGTYPE_TAGGEDPLAINTEXT)
    {
        return tr("OTR TaggedPlaintexMessage");
    }
    else if (type == OTRL_MSGTYPE_QUERY)
    {
        return tr("OTR QueryMessage");
    }
    else if (type == OTRL_MSGTYPE_DH_COMMIT)
    {
        return tr("OTR DH-Commit Message");
    }
    else if (type == OTRL_MSGTYPE_DH_KEY)
    {
        return tr("OTR DH-Key Message");
    }
    else if (type == OTRL_MSGTYPE_REVEALSIG)
    {
        return tr("OTR Reveal Signature Message");
    }
    else if (type == OTRL_MSGTYPE_SIGNATURE)
    {
        return tr("OTR Signature Message");
    }
    else if (type == OTRL_MSGTYPE_V1_KEYEXCH)
    {
        return tr("OTR Version 1 Key Exchange Message");
    }
    else if (type == OTRL_MSGTYPE_DATA)
    {
        return tr("OTR Data Message");
    }
    else if (type == OTRL_MSGTYPE_ERROR)
    {
        return tr("OTR Error Message");
    }
    else if (type == OTRL_MSGTYPE_UNKNOWN)
    {
        return tr("OTR Unknown Message");
    }
    else
    {
        return tr("Unknown OTR Message Type");
    }
}

// ============================================================================

OtrInternal::OtrInternal(OtrSupport::Policy &policy,
                         OtrlUserState userstate)
    : m_userstate(),
      m_uiOps(),
      m_otrPolicy(policy),
      m_mutex()
{
	QDir shareDir = SystemInfo::getDir(SystemInfo::ConfigDir);
	m_keysFile = shareDir.filePath(OTR_KEYS_FILE);
	m_fingerprintFile = shareDir.filePath(OTR_FINGERPRINTS_FILE);
    //m_userstate = otrl_userstate_create();
    m_userstate = userstate;
    m_uiOps.policy = (OtrInternal::cb_policy);
    m_uiOps.create_privkey = (OtrInternal::cb_create_privkey);
    m_uiOps.is_logged_in = (OtrInternal::cb_is_logged_in);
    m_uiOps.inject_message = (OtrInternal::cb_inject_message);
    m_uiOps.notify = (OtrInternal::cb_notify);
    m_uiOps.display_otr_message = (OtrInternal::cb_display_otr_message);
    m_uiOps.update_context_list = (OtrInternal::cb_update_context_list);
    m_uiOps.protocol_name = (OtrInternal::cb_protocol_name);
    m_uiOps.protocol_name_free = (OtrInternal::cb_protocol_name_free);
    m_uiOps.new_fingerprint = (OtrInternal::cb_new_fingerprint);
    m_uiOps.write_fingerprints = (OtrInternal::cb_write_fingerprints);
    m_uiOps.gone_secure = (OtrInternal::cb_gone_secure);
    m_uiOps.gone_insecure = (OtrInternal::cb_gone_insecure);
    m_uiOps.still_secure = (OtrInternal::cb_still_secure);
    m_uiOps.log_message = (OtrInternal::cb_log_message);
    m_uiOps.max_message_size = (OtrInternal::cb_max_message_size);

#if not (OTRL_VERSION_MAJOR==3 && OTRL_VERSION_MINOR==0)
    m_uiOps.account_name = NULL;
    m_uiOps.account_name_free = NULL;
#endif

    otrl_privkey_read(m_userstate, m_keysFile.toLocal8Bit().data());
    otrl_privkey_read_fingerprints(m_userstate,
                                   m_fingerprintFile.toLocal8Bit().data(),
                                   NULL, NULL);
}

//-----------------------------------------------------------------------------

OtrInternal::~OtrInternal()
{
}

//-----------------------------------------------------------------------------

QString OtrInternal::encryptMessage(const QString& from, const QString& to,
                                      const QString& message, TreeModelItem &item)
{
    QString protocol = item.m_protocol_name;
    char* encMessage = NULL;
    gcry_error_t err;

    err = otrl_message_sending(m_userstate, &m_uiOps, this,
                               from.toStdString().c_str(), protocol.toStdString().c_str(),
                               to.toStdString().c_str(),
                               message.toUtf8().data(),
                               NULL, &encMessage, NULL, NULL);
    if (err != 0)
    {
        QMessageBox mb(QMessageBox::Critical, tr("qutim-otr"),
                       tr("Encrypting message from %1 to %2 failed.").arg(QString(from)).arg(QString(to)) + "\n" +
                       tr("The message was not sent."),
                       QMessageBox::Ok, NULL,
                       Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        mb.exec();
        return QString();
    }

    if (encMessage != NULL)
    {
        QString retMessage(QString::fromUtf8(encMessage));
        otrl_message_free(encMessage);

        return retMessage;
    }
    return message;
}


//-----------------------------------------------------------------------------

QString OtrInternal::decryptMessage(const QString& from, const QString& to,
                                    const QString& cryptedMessage, TreeModelItem &item)
{
    QString protocol = item.m_protocol_name;
    int ignoreMessage = 0;
    char *newMessage = NULL;
    OtrlTLV *tlvs = NULL;
    OtrlTLV *tlv = NULL;
    ConnContext *context = 0;
    NextExpectedSMP nextMsg;

    ignoreMessage = otrl_message_receiving(m_userstate, &m_uiOps, this,
                                           to.toStdString().c_str(),
                                           protocol.toStdString().c_str(),
                                           from.toStdString().c_str(),
                                           cryptedMessage.toUtf8().data(),
                                           &newMessage,
                                           &tlvs, NULL, NULL);

    context = otrl_context_find( m_userstate, from.toStdString().c_str(), to.toStdString().c_str(), protocol.toStdString().c_str(), 0, NULL, NULL, NULL);

//    qDebug() << "[OTR] context fragment: " << QString(context->lastmessage);

    tlv = otrl_tlv_find(tlvs, OTRL_TLV_DISCONNECTED);
    if( tlv ){
            sendCustomNessage(item,tr("%1 has ended the OTR session. You should do the same.")
			                  .arg(Qt::escape(item.m_item_name)));
            gone_insecure(context);
    }

    while (context) {
        OtrlSMState *state = context->smstate;
        if(!state)
            break;
        nextMsg = state->nextExpected;
        tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP1Q);
        if (tlv) {
//            qDebug() << "[OTR] SMP detected. Found SMP1Q";
            //a-la pidgin
            if (nextMsg != OTRL_SMP_EXPECT1)
                abortSMP(context,item);
            else {
                char *question = (char *)tlv->data;
                char *eoq = (char*)memchr(question, '\0', tlv->len);
                if (eoq)
                {
                    QString ans = QInputDialog::getText(NULL,tr("Auth"),tr("Please, answer the question to be authorised by %1.<br>Question: <b>%2</b>").arg(from).arg(QString(question)));
                    if(!ans.isEmpty())
                        respondSMP(context,item,ans,false);
                    else
                        abortSMP(context,item);
                }
            }
        }

        tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP1);
        if (tlv) {
//            qDebug() << "[OTR] SMP detected. Found SMP1";
            if (nextMsg != OTRL_SMP_EXPECT1 ){
                abortSMP( context, item );
            } else {
                QString s = QInputDialog::getText ( NULL, tr("Authorysing"), tr("Please, enter passphrase to authorise %1").arg(context->username), QLineEdit::Normal);
                if(!s.isEmpty())
                    respondSMP(context,item,s,false);
                else
                    abortSMP(context,item);
            }
        }
        tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP2);
        if (tlv) {
//            qDebug() << "[OTR] SMP detected. Found SMP2";
            if (nextMsg != OTRL_SMP_EXPECT2){
                abortSMP( context, item );
            } else {
                context->smstate->nextExpected = OTRL_SMP_EXPECT4;
            }
        }
        tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP3);
        if (tlv) {
//            qDebug() << "[OTR] SMP detected. Found SMP3";
            if (nextMsg != OTRL_SMP_EXPECT3){
                abortSMP( context, item );
            } else {
                if (context->active_fingerprint->trust && context->active_fingerprint->trust[0]) {
                    sendCustomNessage(item, tr("Your buddy has successfully authenticated you. The conversation is now secure!"));
                    gone_secure(context);
                } else {
//                    sendCustomNessage(item, tr("Authentication failed. The conversation is now insecure!"));
                    sendCustomNessage(item,tr("Your buddy has successfully authenticated you. You may want to authenticate your buddy as well by asking your own question."));
                    gone_secure(context);
                }

                context->smstate->nextExpected = OTRL_SMP_EXPECT1;
            }
        }
        tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP4);
        if (tlv) {
//            qDebug() << "[OTR] SMP detected. Found SMP4";
            if (nextMsg != OTRL_SMP_EXPECT4) {
                abortSMP( context, item );
            } else {
                if (context->active_fingerprint->trust && context->active_fingerprint->trust[0]) {
                    sendCustomNessage(item, tr("Authentication successful. The conversation is now secure!"));
                    gone_secure(context);
                } else {
                    sendCustomNessage(item, tr("Authentication failed. The conversation is now insecure!"));
                    gone_secure(context);
                }
                context->smstate->nextExpected = OTRL_SMP_EXPECT1;
            }
        }
        tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP_ABORT);
        if (tlv) {
//            qDebug() << "[OTR] SMP detected. Found SMP_ABORT";
            sendCustomNessage(item,tr("Authentication error!").toLocal8Bit() );
            context->smstate->nextExpected = OTRL_SMP_EXPECT1;
        }

        otrl_tlv_free(tlvs);
        break;
    }

    if (ignoreMessage == 1) // internal protocol message
    {
        OtrlMessageType type = otrl_proto_message_type(
                cryptedMessage.toStdString().c_str());

		if (newMessage)
			otrl_message_free(newMessage);
        QString retMessage("<Internal OTR message>\n"+tr("received %1 \nOTR state now is [%2]").arg(otrlMessageTypeToString(type)).arg(getMessageStateString(to, from, item))) ;

        if (getMessageState(to, from, item) == OtrSupport::MessageStateEncrypted)
        {
            retMessage.append(tr("\nsessionId: ") + getSessionId(to, from, item));
        }

//        TODO: если бы эти сообщения можна было заблокировать...
//        но recivelevel1/2 не дает такой возможности... почему то
//        хотя в вики написано обратное
//        sendCustomNessage(item,retMessage);

        return retMessage;
    }
    else if (ignoreMessage == 0)
    {
        if (newMessage != NULL) // message has been decrypted. replace it
        {
            QString retMessage = QString::fromUtf8(newMessage);
            otrl_message_free(newMessage);

            return retMessage;
        }
        else // received message was not an otr message
        {
            return cryptedMessage;
        }
    }

    assert(false);
    return QString();
}

//-----------------------------------------------------------------------------

QList<OtrSupport::Fingerprint> OtrInternal::getFingerprints()
{
    QList<OtrSupport::Fingerprint> fpList;
    ConnContext* context;
    ::Fingerprint* fingerprint;

    for (context = m_userstate->context_root; context != NULL;
         context = context->next)
    {
        fingerprint = context->fingerprint_root.next;
        while(fingerprint)
        {
            OtrSupport::Fingerprint fp;

            fp.account = QString(context->accountname);

            fp.username = QString(context->username);

            fp.fingerprint = fingerprint->fingerprint;
            char fpHash[45];
            otrl_privkey_hash_to_human(fpHash, fingerprint->fingerprint);
            fp.fingerprintHuman = QString(fpHash);

            fp.trust = QString(fingerprint->trust);

            if (fingerprint == context->active_fingerprint)
            {
                TreeModelItem item;
                item.m_account_name = context->accountname;
                item.m_item_type = 0;
                item.m_protocol_name = context->protocol;
                fp.messageState = QString(
                    getMessageStateString(context->accountname,
                                          context->username,
                                          item));
            }
            else
            {
                fp.messageState.clear();
            }

            fpList.append(fp);
            fingerprint = fingerprint->next;
        }
    }
    return fpList;
}

//-----------------------------------------------------------------------------

void OtrInternal::verifyFingerprint(const OtrSupport::Fingerprint& fingerprint,
                                    bool verified)
{
    ConnContext* context;
    ::Fingerprint* fp;

    for (context = m_userstate->context_root; context != NULL;
         context = context->next)
    {
        fp = otrl_context_find_fingerprint(context, fingerprint.fingerprint,
                                           0, NULL);
        if (verified)
        {
            otrl_context_set_trust(fp, "verified");
        }
        else
        {
            otrl_context_set_trust(fp, "");
        }
    }

    write_fingerprints();
}

//-----------------------------------------------------------------------------

void OtrInternal::deleteKey(QString account, QString protocol)
{
    otrl_privkey_forget(otrl_privkey_find(m_userstate,account.toLocal8Bit().data(),protocol.toLocal8Bit().data()));
}

void OtrInternal::deleteFingerprint(const OtrSupport::Fingerprint& fingerprint)
{
    ConnContext* context;
    ::Fingerprint* fp;

    for (context = m_userstate->context_root; context != NULL;
         context = context->next)
    {
        fp = otrl_context_find_fingerprint(context, fingerprint.fingerprint, 0,
                                           NULL);
        if (fp != NULL)
        {
            otrl_context_forget_fingerprint(fp, true);
            break;
        }
    }
    write_fingerprints();
}

//-----------------------------------------------------------------------------

QHash<QString, QHash<QString, QString> > OtrInternal::getPrivateKeys()
{
    QHash<QString, QHash<QString, QString> > privKeyList;
    ConnContext* context;

    for (context = m_userstate->context_root; context != NULL;
         context = context->next)
    {
        char fingerprintBuf[45];
        char* success = otrl_privkey_fingerprint(m_userstate,
                                                 fingerprintBuf,
                                                 context->accountname,
                                                 context->protocol);
        if (success)
        {
            QHash<QString, QString> hash;
            if(privKeyList.contains(QString(context->accountname)))
                hash = privKeyList.value(QString(context->accountname));
            hash[QString(context->protocol)] = QString(fingerprintBuf);
            privKeyList.insert(QString(context->accountname),hash);
        }
    }

    return privKeyList;
}

//-----------------------------------------------------------------------------
void OtrInternal::startSession(const QString& account, const QString& jid, TreeModelItem &item, int pol)
{
    Q_UNUSED(jid);
    OtrlPolicy policy = (OtrlPolicy)pol;
    char fingerprint[45];
    if (!otrl_privkey_fingerprint(m_userstate, fingerprint,
                                  account.toStdString().c_str(),
                                  item.m_protocol_name.toStdString().c_str()))
    {
        create_privkey(account.toStdString().c_str(), item.m_protocol_name.toStdString().c_str());
    }
    if (!otrl_privkey_fingerprint(m_userstate, fingerprint,
                                  account.toStdString().c_str(),
                                  item.m_protocol_name.toStdString().c_str()))
        return;

    //TODO: make allowed otr versions configureable
    char* msg = otrl_proto_default_query_msg(account.toStdString().c_str(),
                                             policy);

	item.unit()->send(QString::fromUtf8(msg));
	free(msg);
}

void OtrInternal::startSession(const QString& account, const QString& jid, TreeModelItem &item)
{
    startSession(account,jid,item,(int)OTRL_POLICY_DEFAULT);
}

//-----------------------------------------------------------------------------

void OtrInternal::endSession(const QString& account, const QString& jid, TreeModelItem &item)
{
    otrl_message_disconnect(m_userstate, &m_uiOps, this,
                            account.toStdString().c_str(), item.m_protocol_name.toStdString().c_str(),
                            jid.toStdString().c_str());
}

//-----------------------------------------------------------------------------

OtrSupport::MessageState OtrInternal::getMessageState(const QString& thisJid,
                                                     const QString& remoteJid,
                                                     TreeModelItem &item)
{
    ConnContext* context = otrl_context_find(m_userstate,
                                             remoteJid.toStdString().c_str(),
                                             thisJid.toStdString().c_str(),
                                             item.m_protocol_name.toStdString().c_str(), false, NULL, NULL,
                                             NULL);
    if (context != NULL)
    {
        if (context->msgstate == OTRL_MSGSTATE_PLAINTEXT)
        {
            return OtrSupport::MessageStatePlainText;
        }
        else if (context->msgstate == OTRL_MSGSTATE_ENCRYPTED)
        {
            return OtrSupport::MessageStateEncrypted;
        }
        else if (context->msgstate == OTRL_MSGSTATE_FINISHED)
        {
            return OtrSupport::MessageStateFinished;
        }
    }

    return OtrSupport::MessageStateUnknown;
}

//-----------------------------------------------------------------------------

int OtrInternal::getMessageStateIntCode(const QString& thisJid,
                                           const QString& remoteJid,
                                           TreeModelItem &item)
{
    OtrSupport::MessageState state = getMessageState(thisJid, remoteJid, item);

    if (state == OtrSupport::MessageStatePlainText)
    {
        return 0;
    }
    else if (state == OtrSupport::MessageStateEncrypted)
    {
        ConnContext* context = otrl_context_find(m_userstate,
                                                 remoteJid.toStdString().c_str(),
                                                 thisJid.toStdString().c_str(),
                                                 item.m_protocol_name.toStdString().c_str(), false, NULL, NULL,
                                                 NULL);
        if(context->active_fingerprint->trust&&context->active_fingerprint->trust[0])
            return 2;

        return 1;
    }
    else if (state == OtrSupport::MessageStateFinished)
    {
        return 3;
    }

    return 4;
}

QString OtrInternal::getMessageStateString(const QString& thisJid,
                                           const QString& remoteJid,
                                           TreeModelItem &item)
{
    OtrSupport::MessageState state = getMessageState(thisJid, remoteJid, item);

    if (state == OtrSupport::MessageStatePlainText)
    {
        return tr("not private");
    }
    else if (state == OtrSupport::MessageStateEncrypted)
    {
        ConnContext* context = otrl_context_find(m_userstate,
                                                 remoteJid.toStdString().c_str(),
                                                 thisJid.toStdString().c_str(),
                                                 item.m_protocol_name.toStdString().c_str(), false, NULL, NULL,
                                                 NULL);
        if(context->active_fingerprint->trust&&context->active_fingerprint->trust[0])
            return tr("private");

        return tr("unverifed");
    }
    else if (state == OtrSupport::MessageStateFinished)
    {
        return tr("finished");
    }

    return tr("unknown");
}

//-----------------------------------------------------------------------------

QString OtrInternal::getSessionId(const QString& thisJid,
                                  const QString& remoteJid,
                                  TreeModelItem &item)
{
    ConnContext* context;
    context = otrl_context_find(m_userstate, remoteJid.toStdString().c_str(),
                                thisJid.toStdString().c_str(), item.m_protocol_name.toStdString().c_str(),
                                false, NULL, NULL, NULL);
    if (context != NULL)
    {
        QString firstHalf;
        QString secondHalf;

        for (unsigned int i = 0; i < context->sessionid_len / 2; i++)
        {
            firstHalf.append(QString::number(context->sessionid[i], 16));
        }
        for (unsigned int i = context->sessionid_len / 2;
             i < context->sessionid_len; i++)
        {
            secondHalf.append(QString::number(context->sessionid[i], 16));
        }
        if (context->sessionid_half == OTRL_SESSIONID_FIRST_HALF_BOLD)
        {
            return QString("<b>" + firstHalf + "</b>" + secondHalf);
        }
        else
        {
            return QString(firstHalf + "<b>" + secondHalf + "</b>");
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
/***  implemented callback functions for libotr ***/

OtrlPolicy OtrInternal::policy(ConnContext*)
{
    if (m_otrPolicy == OtrSupport::PolicyOff)
    {
        return OTRL_POLICY_NEVER; // otr disabled
    }
    else if (m_otrPolicy == OtrSupport::PolicyEnabled)
    {
        return OTRL_POLICY_MANUAL; // otr enabled, session started manual
    }
    else if (m_otrPolicy == OtrSupport::PolicyAuto)
    {
        return OTRL_POLICY_OPPORTUNISTIC; // automatically initiate private messaging
    }
    else if (m_otrPolicy == OtrSupport::PolicyRequire)
    {
        return OTRL_POLICY_ALWAYS; // require private messaging
    }

    return OTRL_POLICY_NEVER;
}

// ---------------------------------------------------------------------------

void OtrInternal::create_privkey(const char *accountname,
                                 const char *protocol)
{
    Q_ASSERT(m_userstate);

    if(!m_mutex.tryLock(10))
        return;

    QMessageBox infoMb(QMessageBox::Information, tr("qutim-otr"),
                       tr("Generating keys for account %1\nThis may take a while.\nPlease, move mouse and use keyoard to decrease generation time.").arg(QString(accountname)),
                       QMessageBox::Ok, NULL,
                       Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    infoMb.button(QMessageBox::Ok)->setEnabled(false);
    infoMb.button(QMessageBox::Ok)->setText(tr("please wait..."));
    infoMb.setWindowModality(Qt::NonModal);
    infoMb.setModal(false);
    infoMb.show();
	Protocol *protocolObject = Protocol::all().value(QString::fromUtf8(protocol));
	Account *account = protocolObject->account(QString::fromUtf8(accountname));
	OTRCrypt::instance()->disableAccount(account);
    {
		QByteArray keysFile = m_keysFile.toLocal8Bit();
		QEventLoop loop;
		QFutureWatcher<gcry_error_t> watcher;
		connect(&watcher, SIGNAL(finished()), &loop, SLOT(quit()));
		QFuture<gcry_error_t> future = QtConcurrent::run(otrl_privkey_generate,
		                                                 m_userstate,
		                                                 keysFile.constData(),
		                                                 accountname,
		                                                 protocol);
		watcher.setFuture(future);
		loop.exec();
	}
	OTRCrypt::instance()->enableAccount(account);
    m_mutex.unlock();
    infoMb.button(QMessageBox::Ok)->setEnabled(true);
    infoMb.button(QMessageBox::Ok)->setText("Ok");

    char fingerprint[45];
    if (otrl_privkey_fingerprint(m_userstate, fingerprint, accountname,
                                 protocol) == NULL)
    {
        QMessageBox failMb(QMessageBox::Critical, tr("qutim-otr"),
                           tr("Failed to generate key for account %1\nThe OTR Plugin will not work.").arg(QString(accountname)),
                           QMessageBox::Ok, NULL,
                           Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        failMb.exec();
    }
    else
    {
        infoMb.setText(tr("The fingerprint for account %1 is\n").arg(QString(accountname)) + QString(fingerprint));
    }
    infoMb.exec();

}

// ---------------------------------------------------------------------------

int OtrInternal::is_logged_in(const char *accountname, const char *protocol,
                              const char *recipient)
{
	TreeModelItem item;
	item.m_protocol_name = QString::fromUtf8(protocol);
	item.m_account_name = QString::fromUtf8(accountname);
	item.m_item_name = QString::fromUtf8(recipient);
	Buddy *buddy = item.unit();
	qDebug() << protocol << accountname << recipient << buddy;
	if (buddy && buddy->status() != Status::Offline)
		return 1;
	else
		return 0;
}

// ---------------------------------------------------------------------------

void OtrInternal::inject_message(const char *accountname,
                                 const char *protocol, const char *recipient,
                                 const char *message)
{
	debug() << "inject_message" << accountname << protocol << recipient << message;
    TreeModelItem item;
	item.m_protocol_name = QString::fromUtf8(protocol);
	item.m_account_name = QString::fromUtf8(accountname);
	item.m_item_name = QString::fromUtf8(recipient);
    item.m_item_type = 0;
    this->inject_message(item,message);
}

void OtrInternal::inject_message(TreeModelItem &item, const char *message)
{
	item.unit()->send(QString::fromUtf8(message));
}

// ---------------------------------------------------------------------------

void OtrInternal::notify(OtrlNotifyLevel level, const char *accountname,
                         const char *protocol, const char *username,
                         const char *title, const char *primary, const char *secondary)
{
    Q_UNUSED(accountname);
    Q_UNUSED(protocol);
    Q_UNUSED(username);

    QMessageBox::Icon messageBoxIcon;
    if (level == OTRL_NOTIFY_ERROR )
    {
        messageBoxIcon = QMessageBox::Critical;
    }
    else if (level == OTRL_NOTIFY_WARNING)
    {
        messageBoxIcon = QMessageBox::Warning;
    }
    else
    {
        messageBoxIcon = QMessageBox::Information;
    }

    QMessageBox mb(messageBoxIcon, "qutim-otr: " + QString(title),
                   QString(primary) + "\n" + QString(secondary),
                   QMessageBox::Ok,
                   NULL, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    mb.exec();
}

// ---------------------------------------------------------------------------

int OtrInternal::display_otr_message(const char *accountname,
                                     const char *protocol,
                                     const char *username,
                                     const char *msg)
{
    Q_UNUSED(accountname);
    Q_UNUSED(protocol);
    Q_UNUSED(username);
    Q_UNUSED(msg);

    return -1; // use notify() or inline message
}

// ---------------------------------------------------------------------------

void OtrInternal::update_context_list()
{
}

// ---------------------------------------------------------------------------

const char* OtrInternal::protocol_name(const char* protocol)
{
    return protocol;
}

// ---------------------------------------------------------------------------

void OtrInternal::protocol_name_free(const char *protocol_name)
{
    Q_UNUSED(protocol_name);
}

// ---------------------------------------------------------------------------

void OtrInternal::new_fingerprint(OtrlUserState us, const char *accountname,
                                  const char *protocol, const char *username,
                                  unsigned char fingerprint[20])
{
    Q_UNUSED(us);
    Q_UNUSED(protocol);

    char fpHuman[45];
    otrl_privkey_hash_to_human(fpHuman, fingerprint);
    TreeModelItem item;
    item.m_account_name = QString(accountname);
    item.m_protocol_name = QString(protocol);
    item.m_item_name = QString(username);
    item.m_item_type = 0;
    sendCustomNessage(item,tr("Account %1 has received new fingerprint from %2 :\n %3")
	                  .arg(Qt::escape(accountname))
	                  .arg(Qt::escape(username))
	                  .arg(Qt::escape(fpHuman)));
//    QMessageBox mb(QMessageBox::Information, tr("qutim-otr"),
//                   tr("Account ") + QString(accountname) + tr(" has received new fingerprint from ")
//                   + QString(username) + ":\n" + QString(fpHuman),
//                   QMessageBox::Ok, NULL, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
//    mb.exec();
}


void OtrInternal::abortSMP(ConnContext *context, TreeModelItem &item)
{
    otrl_message_abort_smp( m_userstate, &m_uiOps, this, context);
    if (context->active_fingerprint->trust && !context->active_fingerprint->trust[0]) {
        gone_secure(context);
        sendCustomNessage(item,tr("Authentication aborded. The conversation is now insecure!"));
    }
}

void OtrInternal::respondSMP(ConnContext *context, TreeModelItem &item, const QString &secret, bool initiate)
{
    if( initiate ){
            context = otrl_context_find( m_userstate, item.m_item_name.toAscii(), item.m_account_name.toAscii(), item.m_protocol_name.toAscii(), 0, NULL, NULL, NULL);
            otrl_message_initiate_smp( m_userstate, &m_uiOps, this, context, (unsigned char*)secret.toAscii().data(), secret.toAscii().count() );
    } else {
            otrl_message_respond_smp( m_userstate, &m_uiOps, this, context, (unsigned char*)secret.toAscii().data(), secret.toAscii().count());
    }

    sendCustomNessage(item,tr("Authenticating contact..."));
}

void OtrInternal::sendCustomNessage(TreeModelItem &item, QString msg)
{
	Message message;
	message.setChatUnit(item.unit());
	message.setProperty("service", true);
	message.setProperty("store", false);
	message.setIncoming(true);
	message.setText(unescape(msg));
	message.setHtml(msg);
	ChatLayer::get(message.chatUnit(), true)->append(message);
	
//    msg.replace("<b>"," ");
//    msg.replace("</b>"," ");
//    msg.replace("<i>"," ");
//    msg.replace("</i>"," ");
//    m_plugin->addServiceMessage(item,msg);
//    m_plugin->systemNotification(item,msg);
}

void OtrInternal::requestAuth(TreeModelItem &item, bool agree, QString answer, QString question)
{
    OtrSupport::Fingerprint fingerprint;
    bool found = false;
    foreach(fingerprint, getFingerprints())
    {
        if (fingerprint.username == item.m_item_name
            && fingerprint.account == item.m_account_name )
        {
            found = true;
            break;
        }
    }
    if(!found)
        return;
    ConnContext *context = otrl_context_find(m_userstate,item.m_item_name.toAscii().data(),item.m_account_name.toAscii().data(),item.m_protocol_name.toAscii().data(),0,NULL,NULL,NULL);
    if(!context)
        return;
    if(!question.isNull())
    {
        otrl_message_initiate_smp_q( m_userstate,&m_uiOps,this,context,question.toAscii(),(unsigned char *)answer.toAscii().data(),answer.toAscii().count());
    } else
        if(!answer.isNull())
        {
        otrl_message_initiate_smp(m_userstate,&m_uiOps,this,context,(unsigned char *)answer.toAscii().data(),answer.toAscii().count());
    } else    
//        if(agree)
        {
        verifyFingerprint(fingerprint, agree);
    }
}

// ---------------------------------------------------------------------------

void OtrInternal::write_fingerprints()
{
    otrl_privkey_write_fingerprints(m_userstate,
                                    m_fingerprintFile.toStdString().c_str());
}

// ---------------------------------------------------------------------------

void OtrInternal::gone_secure(ConnContext *context)
{
    Q_UNUSED(context);
}

// ---------------------------------------------------------------------------

void OtrInternal::gone_insecure(ConnContext *context)
{
    Q_UNUSED(context);
}

// ---------------------------------------------------------------------------

void OtrInternal::still_secure(ConnContext *context, int is_reply)
{
    Q_UNUSED(context);
    Q_UNUSED(is_reply);
}

// ---------------------------------------------------------------------------

void OtrInternal::log_message(const char *message)
{
    debug() << "log_message: " << QString::fromUtf8(message);
}

int OtrInternal::max_message_size(ConnContext *context)
{
    QString protocol(context->protocol);
    int max = 0;
    if(protocol=="Jabber")
    {
        max = 5000;//это на всякий случай, с запасом
    } else
        if(protocol=="ICQ")
    {
        max = 2343;
    } else
        if(protocol=="MSN")
    {
        max = 1409;
    } else
        if(protocol=="YAHOO")
    {
        max = 832;
    }
    return max;
}

// ---------------------------------------------------------------------------
/*** static wrapper functions ***/

OtrlPolicy OtrInternal::cb_policy(void *opdata, ConnContext *context) {
    Q_ASSERT(opdata);
    return static_cast<OtrInternal*>(opdata)->policy(context);
}

void OtrInternal::cb_create_privkey(void *opdata, const char *accountname, const char *protocol) {
    Q_ASSERT(opdata);
    static_cast<OtrInternal*>(opdata)->create_privkey(accountname, protocol);
}

int OtrInternal::cb_is_logged_in(void *opdata, const char *accountname, const char *protocol, const char *recipient) {
    Q_ASSERT(opdata);
    return static_cast<OtrInternal*>(opdata)->is_logged_in(accountname, protocol, recipient);
}

void OtrInternal::cb_inject_message(void *opdata, const char *accountname, const char *protocol, const char *recipient, const char *message) {
    Q_ASSERT(opdata);
    static_cast<OtrInternal*>(opdata)->inject_message(accountname, protocol, recipient, message);
}

void OtrInternal::cb_notify(void *opdata, OtrlNotifyLevel level, const char *accountname, const char *protocol, const char *username, const char *title, const char *primary, const char *secondary) {
    Q_ASSERT(opdata);
    static_cast<OtrInternal*>(opdata)->notify(level, accountname, protocol, username, title, primary, secondary);
}

int OtrInternal::cb_display_otr_message(void *opdata, const char *accountname, const char *protocol, const char *username, const char *msg) {
    Q_ASSERT(opdata);
    return static_cast<OtrInternal*>(opdata)->display_otr_message(accountname, protocol, username, msg);
}

void OtrInternal::cb_update_context_list(void *opdata) {
    Q_ASSERT(opdata);
    static_cast<OtrInternal*>(opdata)->update_context_list();
}

const char* OtrInternal::cb_protocol_name(void *opdata, const char *protocol) {
    Q_ASSERT(opdata);
    return static_cast<OtrInternal*>(opdata)->protocol_name(protocol);
}

void OtrInternal::cb_protocol_name_free(void *opdata, const char *protocol_name) {
    Q_ASSERT(opdata);
    static_cast<OtrInternal*>(opdata)->protocol_name(protocol_name);
}

void OtrInternal::cb_new_fingerprint(void *opdata, OtrlUserState us, const char *accountname, const char *protocol, const char *username, unsigned char fingerprint[20]) {
    Q_ASSERT(opdata);
    static_cast<OtrInternal*>(opdata)->new_fingerprint(us, accountname, protocol, username, fingerprint);
}

void OtrInternal::cb_write_fingerprints(void *opdata) {
    Q_ASSERT(opdata);
    static_cast<OtrInternal*>(opdata)->write_fingerprints();
}

void OtrInternal::cb_gone_secure(void *opdata, ConnContext *context) {
    Q_ASSERT(opdata);
    static_cast<OtrInternal*>(opdata)->gone_secure(context);
}

void OtrInternal::cb_gone_insecure(void *opdata, ConnContext *context) {
    Q_ASSERT(opdata);
    static_cast<OtrInternal*>(opdata)->gone_insecure(context);
}

void OtrInternal::cb_still_secure(void *opdata, ConnContext *context, int is_reply) {
    Q_ASSERT(opdata);
    static_cast<OtrInternal*>(opdata)->still_secure(context, is_reply);
}

void OtrInternal::cb_log_message(void *opdata, const char *message) {
    Q_ASSERT(opdata);
    static_cast<OtrInternal*>(opdata)->log_message(message);
}

int OtrInternal::cb_max_message_size(void *opdata, ConnContext *context)
{
    Q_ASSERT(opdata);
    return static_cast<OtrInternal*>(opdata)->max_message_size(context);
}

// ---------------------------------------------------------------------------
