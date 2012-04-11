/*
 * OtrInternal.h - manages the otr-connection.
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

#ifndef OTRINTERNAL_HPP_
#define OTRINTERNAL_HPP_

#include <QObject>
#include <QtGui>
#include <QSemaphore>
#include <QMutex>

#include "otrmessaging.h"

#include <QList>
#include <QHash>

#include "global.h"

class QString;

// ---------------------------------------------------------------------------

/**
* Handles all libotr calls and callbacks.
*/
class OtrInternal:public QObject
{
    Q_OBJECT
public:

    OtrInternal(OtrSupport::Policy& policy, OtrlUserState userstate);

    ~OtrInternal();

    void sendCustomNessage(TreeModelItem &item, QString msg);

    QString otrlMessageTypeToString(const OtrlMessageType& type);

    QString encryptMessage(const QString& from, const QString& to,
                           const QString& message, TreeModelItem &item);

    QString decryptMessage(const QString& from, const QString& to,
                           const QString& message, TreeModelItem &item);

    QList<OtrSupport::Fingerprint> getFingerprints();

    void verifyFingerprint(const OtrSupport::Fingerprint& fingerprint, bool verified);

    void deleteFingerprint(const OtrSupport::Fingerprint& fingerprint);

    void deleteKey(QString account, QString protocol);

    QHash<QString, QHash<QString, QString> > getPrivateKeys();

    void startSession(const QString& account, const QString& jid, TreeModelItem &item, int pol);

    void startSession(const QString& account, const QString& jid, TreeModelItem &item);

    void endSession(const QString& account, const QString& jid, TreeModelItem &item);

    OtrSupport::MessageState getMessageState(const QString& thisJid,
                                            const QString& remoteJid,
                                            TreeModelItem &item);

    QString getMessageStateString(const QString& thisJid,
                                  const QString& remoteJid,
                                  TreeModelItem &item);
    
    int getMessageStateIntCode(const QString& thisJid,
                                  const QString& remoteJid,
                                  TreeModelItem &item);

    QString getSessionId(const QString& thisJid, const QString& remoteJid, TreeModelItem &item);

    void abortSMP(ConnContext *context, TreeModelItem &item);
    void respondSMP(ConnContext *context, TreeModelItem &item, const QString &secret, bool initiate);
    void requestAuth(TreeModelItem &item, bool agree, QString answer = 0, QString question = 0);


    /*** otr callback functions ***/
    OtrlPolicy policy(ConnContext *context);
    void create_privkey(const char *accountname, const char *protocol);
    int is_logged_in(const char *accountname, const char *protocol,
                     const char *recipient);
    void inject_message(const char *accountname, const char *protocol,
                        const char *recipient, const char *message);//depreciated
    void inject_message(TreeModelItem &item, const char *message);
    void notify(OtrlNotifyLevel level, const char *accountname,
                const char *protocol, const char *username, const char *title,
                const char *primary, const char *secondary);
    int display_otr_message(const char *accountname, const char *protocol,
                            const char *username, const char *msg);
    void update_context_list();
    const char* protocol_name(const char *protocol);
    void protocol_name_free(const char *protocol_name);
    void new_fingerprint(OtrlUserState us, const char *accountname,
                         const char *protocol, const char *username,
                         unsigned char fingerprint[20]);
    void write_fingerprints();
    void gone_secure(ConnContext *context);
    void gone_insecure(ConnContext *context);
    void still_secure(ConnContext *context, int is_reply);
    void log_message(const char *message);
    int max_message_size(ConnContext *context);


    /*** static otr callback wrapper-functions ***/
    static OtrlPolicy cb_policy(void *opdata, ConnContext *context);
    static void cb_create_privkey(void *opdata, const char *accountname,
                                  const char *protocol);
    static int cb_is_logged_in(void *opdata, const char *accountname,
                               const char *protocol, const char *recipient);
    static void cb_inject_message(void *opdata, const char *accountname,
                                  const char *protocol, const char *recipient,
                                  const char *message);
    static void cb_notify(void *opdata, OtrlNotifyLevel level,
                          const char *accountname, const char *protocol,
                          const char *username, const char *title,
                          const char *primary, const char *secondary);
    static int cb_display_otr_message(void *opdata, const char *accountname,
                                      const char *protocol, const char *username,
                                      const char *msg);
    static void cb_update_context_list(void *opdata);
    static const char* cb_protocol_name(void *opdata, const char *protocol);
    static void cb_protocol_name_free(void *opdata, const char *protocol_name);
    static void cb_new_fingerprint(void *opdata, OtrlUserState us,
                                   const char *accountname, const char *protocol,
                                   const char *username, unsigned char fingerprint[20]);
    static void cb_write_fingerprints(void *opdata);
    static void cb_gone_secure(void *opdata, ConnContext *context);
    static void cb_gone_insecure(void *opdata, ConnContext *context);
    static void cb_still_secure(void *opdata, ConnContext *context, int is_reply);
    static void cb_log_message(void *opdata, const char *message);
    static int cb_max_message_size(void *opdata, ConnContext *context);

private:

    /**
    * The userstate contains keys and known fingerprints.
    */
    OtrlUserState m_userstate;

    /**
    * Pointers to callback functions.
    */
    OtrlMessageAppOps m_uiOps;

    /**
    * Name of the file storing dsa-keys.
    */
    QString m_keysFile;

    /**
    * Name of the file storing known fingerprints.
    */
    QString m_fingerprintFile;

    /**
    * Reference to the default OTR policy
    */
    OtrSupport::Policy& m_otrPolicy;

    QMutex m_mutex;
};

// ---------------------------------------------------------------------------

#endif
