#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <QString>
#include <QDir>
#include <qutim/buddy.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/message.h>
#include <qutim/debug.h>

struct TreeModelItem
{
	TreeModelItem()
	{
		m_item_type = 0xff;
	}

	TreeModelItem(qutim_sdk_0_3::ChatUnit *unit)
	{
		m_item_name = unit->id();
		qutim_sdk_0_3::Account *account = unit->account();
		m_account_name = account->id();
		m_protocol_name = account->protocol()->id();
		m_item_type = 0;
	}
	
	qutim_sdk_0_3::Buddy *unit()
	{
		qutim_sdk_0_3::debug() << "Trying to convert TreeModelItem {"
		                       << m_protocol_name
		                       << ", "
		                       << m_account_name
		                       << ", "
		                       << m_item_name
		                       << "}";
		qutim_sdk_0_3::Protocol *protocol = qutim_sdk_0_3::Protocol::all().value(m_protocol_name);
		qutim_sdk_0_3::Account *account = protocol->account(m_account_name);
		Q_ASSERT(account);
		return qobject_cast<qutim_sdk_0_3::Buddy*>(account->getUnit(m_item_name));
	}

	QString m_protocol_name;
	QString m_account_name;
	QString m_item_name;
	quint8 m_item_type;
};

#ifndef Q_CC_MSVC
extern "C"
{
#include <libotr/proto.h>
#include <libotr/message.h>
#include <libotr/privkey.h>
}
#else
extern "C"
{
#define DLL  __declspec( dllimport )
#include <gcrypt.h>
    /*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __DH_H__
#define __DH_H__

#define DH1536_GROUP_ID 5

typedef struct {
    unsigned int groupid;
    gcry_mpi_t priv, pub;
} DH_keypair;

/* Which half of the secure session id should be shown in bold? */
typedef enum {
    OTRL_SESSIONID_FIRST_HALF_BOLD,
    OTRL_SESSIONID_SECOND_HALF_BOLD
} OtrlSessionIdHalf;

typedef struct {
    unsigned char sendctr[16];
    unsigned char rcvctr[16];
    gcry_cipher_hd_t sendenc;
    gcry_cipher_hd_t rcvenc;
    gcry_md_hd_t sendmac;
    unsigned char sendmackey[20];
    int sendmacused;
    gcry_md_hd_t rcvmac;
    unsigned char rcvmackey[20];
    int rcvmacused;
} DH_sesskeys;

/*
 * Call this once, at plugin load time.  It sets up the modulus and
 * generator MPIs.
 */
DLL void otrl_dh_init(void);

/*
 * Initialize the fields of a DH keypair.
 */
DLL void otrl_dh_keypair_init(DH_keypair *kp);

/*
 * Copy a DH_keypair.
 */
DLL void otrl_dh_keypair_copy(DH_keypair *dst, const DH_keypair *src);

/*
 * Deallocate the contents of a DH_keypair (but not the DH_keypair
 * itself)
 */
DLL void otrl_dh_keypair_free(DH_keypair *kp);

/*
 * Generate a DH keypair for a specified group.
 */
DLL gcry_error_t otrl_dh_gen_keypair(unsigned int groupid, DH_keypair *kp);

/*
 * Construct session keys from a DH keypair and someone else's public
 * key.
 */
DLL gcry_error_t otrl_dh_session(DH_sesskeys *sess, const DH_keypair *kp,
    gcry_mpi_t y);

/*
 * Compute the secure session id, two encryption keys, and four MAC keys
 * given our DH key and their DH public key.
 */
DLL gcry_error_t otrl_dh_compute_v2_auth_keys(const DH_keypair *our_dh,
    gcry_mpi_t their_pub, unsigned char *sessionid, size_t *sessionidlenp,
    gcry_cipher_hd_t *enc_c, gcry_cipher_hd_t *enc_cp,
    gcry_md_hd_t *mac_m1, gcry_md_hd_t *mac_m1p,
    gcry_md_hd_t *mac_m2, gcry_md_hd_t *mac_m2p);

/*
 * Compute the secure session id, given our DH key and their DH public
 * key.
 */
DLL gcry_error_t otrl_dh_compute_v1_session_id(const DH_keypair *our_dh,
    gcry_mpi_t their_pub, unsigned char *sessionid, size_t *sessionidlenp,
    OtrlSessionIdHalf *halfp);

/*
 * Deallocate the contents of a DH_sesskeys (but not the DH_sesskeys
 * itself)
 */
DLL void otrl_dh_session_free(DH_sesskeys *sess);

/*
 * Blank out the contents of a DH_sesskeys (without releasing it)
 */
DLL void otrl_dh_session_blank(DH_sesskeys *sess);

/* Increment the top half of a counter block */
DLL void otrl_dh_incctr(unsigned char *ctr);

/* Compare two counter values (8 bytes each).  Return 0 if ctr1 == ctr2,
 * < 0 if ctr1 < ctr2 (as unsigned 64-bit values), > 0 if ctr1 > ctr2. */
DLL int otrl_dh_cmpctr(const unsigned char *ctr1, const unsigned char *ctr2);

#endif
/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __PRIVKEY_T_H__
#define __PRIVKEY_T_H__

//#include <gcrypt.h>

typedef struct s_OtrlPrivKey {
    struct s_OtrlPrivKey *next;
    struct s_OtrlPrivKey **tous;

    char *accountname;
    char *protocol;
    unsigned short pubkey_type;
    gcry_sexp_t privkey;
    unsigned char *pubkey_data;
    size_t pubkey_datalen;
} OtrlPrivKey;

#define OTRL_PUBKEY_TYPE_DSA 0x0000

#endif

/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __AUTH_H__
#define __AUTH_H__

//#include <gcrypt.h>
//#include "dh.h"

typedef enum {
    OTRL_AUTHSTATE_NONE,
    OTRL_AUTHSTATE_AWAITING_DHKEY,
    OTRL_AUTHSTATE_AWAITING_REVEALSIG,
    OTRL_AUTHSTATE_AWAITING_SIG,
    OTRL_AUTHSTATE_V1_SETUP
} OtrlAuthState;

typedef struct {
    OtrlAuthState authstate;              /* Our state */

    DH_keypair our_dh;                    /* Our D-H key */
    unsigned int our_keyid;               /* ...and its keyid */

    unsigned char *encgx;                 /* The encrypted value of g^x */
    size_t encgx_len;                     /*  ...and its length */
    unsigned char r[16];                  /* The encryption key */

    unsigned char hashgx[32];             /* SHA256(g^x) */

    gcry_mpi_t their_pub;                 /* Their D-H public key */
    unsigned int their_keyid;             /*  ...and its keyid */

    gcry_cipher_hd_t enc_c, enc_cp;       /* c and c' encryption keys */
    gcry_md_hd_t mac_m1, mac_m1p;         /* m1 and m1' MAC keys */
    gcry_md_hd_t mac_m2, mac_m2p;         /* m2 and m2' MAC keys */

    unsigned char their_fingerprint[20];  /* The fingerprint of their
                         long-term signing key */

    int initiated;                        /* Did we initiate this
                         authentication? */

    unsigned int protocol_version;        /* The protocol version number
                         used to authenticate. */

    unsigned char secure_session_id[20];  /* The secure session id */
    size_t secure_session_id_len;         /* And its actual length,
                         which may be either 20 (for
                         v1) or 8 (for v2) */
    OtrlSessionIdHalf session_id_half;    /* Which half of the session
                         id gets shown in bold */

    char *lastauthmsg;                    /* The last auth message
                         (base-64 encoded) we sent,
                         in case we need to
                         retransmit it. */
} OtrlAuthInfo;

//#include "privkey-t.h"

/*
 * Initialize the fields of an OtrlAuthInfo (already allocated).
 */
DLL void otrl_auth_new(OtrlAuthInfo *auth);

/*
 * Clear the fields of an OtrlAuthInfo (but leave it allocated).
 */
DLL void otrl_auth_clear(OtrlAuthInfo *auth);

/*
 * Start a fresh AKE (version 2) using the given OtrlAuthInfo.  Generate
 * a fresh DH keypair to use.  If no error is returned, the message to
 * transmit will be contained in auth->lastauthmsg.
 */
DLL gcry_error_t otrl_auth_start_v2(OtrlAuthInfo *auth);

/*
 * Handle an incoming D-H Commit Message.  If no error is returned, the
 * message to send will be left in auth->lastauthmsg.  Generate a fresh
 * keypair to use.
 */
DLL gcry_error_t otrl_auth_handle_commit(OtrlAuthInfo *auth,
    const char *commitmsg);

/*
 * Handle an incoming D-H Key Message.  If no error is returned, and
 * *havemsgp is 1, the message to sent will be left in auth->lastauthmsg.
 * Use the given private authentication key to sign messages.
 */
DLL gcry_error_t otrl_auth_handle_key(OtrlAuthInfo *auth, const char *keymsg,
    int *havemsgp, OtrlPrivKey *privkey);

/*
 * Handle an incoming Reveal Signature Message.  If no error is
 * returned, and *havemsgp is 1, the message to be sent will be left in
 * auth->lastauthmsg.  Use the given private authentication key to sign
 * messages.  Call the auth_succeeded callback if authentication is
 * successful.
 */
DLL gcry_error_t otrl_auth_handle_revealsig(OtrlAuthInfo *auth,
    const char *revealmsg, int *havemsgp, OtrlPrivKey *privkey,
    gcry_error_t (*auth_succeeded)(const OtrlAuthInfo *auth, void *asdata),
    void *asdata);

/*
 * Handle an incoming Signature Message.  If no error is returned, and
 * *havemsgp is 1, the message to be sent will be left in
 * auth->lastauthmsg.  Call the auth_succeeded callback if
 * authentication is successful.
 */
DLL gcry_error_t otrl_auth_handle_signature(OtrlAuthInfo *auth,
    const char *sigmsg, int *havemsgp,
    gcry_error_t (*auth_succeeded)(const OtrlAuthInfo *auth, void *asdata),
    void *asdata);

/*
 * Start a fresh AKE (version 1) using the given OtrlAuthInfo.  If
 * our_dh is NULL, generate a fresh DH keypair to use.  Otherwise, use a
 * copy of the one passed (with the given keyid).  Use the given private
 * key to sign the message.  If no error is returned, the message to
 * transmit will be contained in auth->lastauthmsg.
 */
DLL gcry_error_t otrl_auth_start_v1(OtrlAuthInfo *auth, DH_keypair *our_dh,
    unsigned int our_keyid, OtrlPrivKey *privkey);

/*
 * Handle an incoming v1 Key Exchange Message.  If no error is returned,
 * and *havemsgp is 1, the message to be sent will be left in
 * auth->lastauthmsg.  Use the given private authentication key to sign
 * messages.  Call the auth_secceeded callback if authentication is
 * successful.  If non-NULL, use a copy of the given D-H keypair, with
 * the given keyid.
 */
DLL gcry_error_t otrl_auth_handle_v1_key_exchange(OtrlAuthInfo *auth,
    const char *keyexchmsg, int *havemsgp, OtrlPrivKey *privkey,
    DH_keypair *our_dh, unsigned int our_keyid,
    gcry_error_t (*auth_succeeded)(const OtrlAuthInfo *auth, void *asdata),
    void *asdata);

#endif
/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __B64_H__
#define __B64_H__

/*
 * base64 encode data.  Insert no linebreaks or whitespace.
 *
 * The buffer base64data must contain at least ((datalen+2)/3)*4 bytes of
 * space.  This function will return the number of bytes actually used.
 */
DLL size_t otrl_base64_encode(char *base64data, const unsigned char *data,
    size_t datalen);

/*
 * base64 decode data.  Skip non-base64 chars, and terminate at the
 * first '=', or the end of the buffer.
 *
 * The buffer data must contain at least (base64len / 4) * 3 bytes of
 * space.  This function will return the number of bytes actually used.
 */
DLL size_t otrl_base64_decode(unsigned char *data, const char *base64data,
    size_t base64len);

/*
 * Base64-encode a block of data, stick "?OTR:" and "." around it, and
 * return the result, or NULL in the event of a memory error.
 */
DLL char *otrl_base64_otr_encode(const unsigned char *buf, size_t buflen);

/*
 * Base64-decode the portion of the given message between "?OTR:" and
 * ".".  Set *bufp to the decoded data, and set *lenp to its length.
 * The caller must free() the result.  Return 0 on success, -1 on a
 * memory error, or -2 on invalid input.
 */
DLL int otrl_base64_otr_decode(const char *msg, unsigned char **bufp,
    size_t *lenp);

#endif
/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __SM_H__
#define __SM_H__

//#include <gcrypt.h>

#define SM_HASH_ALGORITHM GCRY_MD_SHA256
#define SM_DIGEST_SIZE 32

typedef enum {
    OTRL_SMP_EXPECT1,
    OTRL_SMP_EXPECT2,
    OTRL_SMP_EXPECT3,
    OTRL_SMP_EXPECT4,
    OTRL_SMP_EXPECT5
} NextExpectedSMP;

typedef enum {
    OTRL_SMP_PROG_OK = 0,            /* All is going fine so far */
    OTRL_SMP_PROG_CHEATED = -2,      /* Some verification failed */
    OTRL_SMP_PROG_FAILED = -1,       /* The secrets didn't match */
    OTRL_SMP_PROG_SUCCEEDED = 1      /* The SMP completed successfully */
} OtrlSMProgState;

typedef struct {
    gcry_mpi_t secret, x2, x3, g1, g2, g3, g3o, p, q, pab, qab;
    NextExpectedSMP nextExpected;
    int received_question;  /* 1 if we received a question in an SMP1Q TLV */
    OtrlSMProgState sm_prog_state;
} OtrlSMState;

typedef OtrlSMState OtrlSMAliceState;
typedef OtrlSMState OtrlSMBobState;

/*
 * Call this once, at plugin load time.  It sets up the modulus and
 * generator MPIs.
 */
DLL void otrl_sm_init(void);

/*
 * Initialize the fields of a SM state.
 */
DLL void otrl_sm_state_new(OtrlSMState *smst);

/*
 * Initialize the fields of a SM state.  Called the first time that
 * a user begins an SMP session.
 */
DLL void otrl_sm_state_init(OtrlSMState *smst);

/*
 * Deallocate the contents of a OtrlSMState (but not the OtrlSMState
 * itself)
 */
DLL void otrl_sm_state_free(OtrlSMState *smst);

DLL gcry_error_t otrl_sm_step1(OtrlSMAliceState *astate, const unsigned char* secret, int secretlen, unsigned char** output, int* outputlen);
DLL gcry_error_t otrl_sm_step2a(OtrlSMBobState *bstate, const unsigned char* input, const int inputlen, int received_question);
DLL gcry_error_t otrl_sm_step2b(OtrlSMBobState *bstate, const unsigned char* secret, int secretlen, unsigned char **output, int* outputlen);
DLL gcry_error_t otrl_sm_step3(OtrlSMAliceState *astate, const unsigned char* input, const int inputlen, unsigned char **output, int* outputlen);
DLL gcry_error_t otrl_sm_step4(OtrlSMBobState *bstate, const unsigned char* input, const int inputlen, unsigned char **output, int* outputlen);
DLL gcry_error_t otrl_sm_step5(OtrlSMAliceState *astate, const unsigned char* input, const int inputlen);

#endif
/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __USERSTATE_H__
#define __USERSTATE_H__

typedef struct s_OtrlUserState* OtrlUserState;

//#include "context.h"
//#include "privkey-t.h"


/* Create a new OtrlUserState.  Most clients will only need one of
 * these.  A OtrlUserState encapsulates the list of known fingerprints
 * and the list of private keys; if you have separate files for these
 * things for (say) different users, use different OtrlUserStates.  If
 * you've got only one user, with multiple accounts all stored together
 * in the same fingerprint store and privkey store files, use just one
 * OtrlUserState. */
DLL OtrlUserState otrl_userstate_create(void);

/* Free a OtrlUserState */
DLL void otrl_userstate_free(OtrlUserState us);

#endif

/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

//#include <gcrypt.h>

//#include "dh.h"
//#include "auth.h"
//#include "sm.h"

typedef enum {
    OTRL_MSGSTATE_PLAINTEXT,           /* Not yet started an encrypted
                      conversation */
    OTRL_MSGSTATE_ENCRYPTED,           /* Currently in an encrypted
                      conversation */
    OTRL_MSGSTATE_FINISHED             /* The remote side has sent us a
                      notification that he has ended
                      his end of the encrypted
                      conversation; prevent any
                      further messages from being
                      sent to him. */
} OtrlMessageState;

typedef struct s_fingerprint {
    struct s_fingerprint *next;        /* The next fingerprint in the list */
    struct s_fingerprint **tous;       /* A pointer to the pointer to us */
    unsigned char *fingerprint;        /* The fingerprint, or NULL */
    struct context *context;           /* The context to which we belong */
    char *trust;                       /* The trust level of the fingerprint */
} Fingerprint;

typedef struct context {
    struct context * next;             /* Linked list pointer */
    struct context ** tous;            /* A pointer to the pointer to us */

    char * username;                   /* The user this context is for */
    char * accountname;                /* The username is relative to
                      this account... */
    char * protocol;                   /* ... and this protocol */

    char *fragment;                    /* The part of the fragmented message
                      we've seen so far */
    size_t fragment_len;               /* The length of fragment */
    unsigned short fragment_n;         /* The total number of fragments
                      in this message */
    unsigned short fragment_k;         /* The highest fragment number
                      we've seen so far for this
                      message */

    OtrlMessageState msgstate;         /* The state of message disposition
                      with this user */
    OtrlAuthInfo auth;                 /* The state of ongoing
                      authentication with this user */

    Fingerprint fingerprint_root;      /* The root of a linked list of
                      Fingerprints entries */
    Fingerprint *active_fingerprint;   /* Which fingerprint is in use now?
                                          A pointer into the above list */
    unsigned int their_keyid;          /* current keyid used by other side;
                                          this is set to 0 if we get a
                      OTRL_TLV_DISCONNECTED message from
                      them. */
    gcry_mpi_t their_y;                /* Y[their_keyid] (their DH pubkey) */
    gcry_mpi_t their_old_y;            /* Y[their_keyid-1] (their prev DH
                      pubkey) */
    unsigned int our_keyid;            /* current keyid used by us */
    DH_keypair our_dh_key;             /* DH key[our_keyid] */
    DH_keypair our_old_dh_key;         /* DH key[our_keyid-1] */

    DH_sesskeys sesskeys[2][2];        /* sesskeys[i][j] are the session keys
                      derived from DH key[our_keyid-i]
                      and mpi Y[their_keyid-j] */

    unsigned char sessionid[20];       /* The sessionid and bold half */
    size_t sessionid_len;              /* determined when this private */
    OtrlSessionIdHalf sessionid_half;  /* connection was established. */

    unsigned int protocol_version;     /* The version of OTR in use */

    unsigned char *preshared_secret;   /* A secret you share with this
                      user, in order to do
                      authentication. */
    size_t preshared_secret_len;       /* The length of the above secret. */

    /* saved mac keys to be revealed later */
    unsigned int numsavedkeys;
    unsigned char *saved_mac_keys;

    /* generation number: increment every time we go private, and never
     * reset to 0 (unless we remove the context entirely) */
    unsigned int generation;

    time_t lastsent;      /* The last time a Data Message was sent */
    char *lastmessage;    /* The plaintext of the last Data Message sent */
    int may_retransmit;   /* Is the last message eligible for
                 retransmission? */

    enum {
    OFFER_NOT,
    OFFER_SENT,
    OFFER_REJECTED,
    OFFER_ACCEPTED
    } otr_offer;          /* Has this correspondent repsponded to our
                 OTR offers? */

    /* Application data to be associated with this context */
    void *app_data;
    /* A function to free the above data when we forget this context */
    void (*app_data_free)(void *);

    OtrlSMState *smstate;              /* The state of the current
                                          socialist millionaires exchange */
} ConnContext;


struct s_OtrlUserState {
    ConnContext *context_root;
    OtrlPrivKey *privkey_root;
};

//#include "userstate.h"

/* Look up a connection context by name/account/protocol from the given
 * OtrlUserState.  If add_if_missing is true, allocate and return a new
 * context if one does not currently exist.  In that event, call
 * add_app_data(data, context) so that app_data and app_data_free can be
 * filled in by the application, and set *addedp to 1. */
DLL ConnContext * otrl_context_find(OtrlUserState us, const char *user,
    const char *accountname, const char *protocol, int add_if_missing,
    int *addedp,
    void (*add_app_data)(void *data, ConnContext *context), void *data);

/* Find a fingerprint in a given context, perhaps adding it if not
 * present. */
DLL Fingerprint *otrl_context_find_fingerprint(ConnContext *context,
    unsigned char fingerprint[20], int add_if_missing, int *addedp);

/* Set the trust level for a given fingerprint */
DLL void otrl_context_set_trust(Fingerprint *fprint, const char *trust);

/* Set the preshared secret for a given fingerprint.  Note that this
 * currently only stores the secret in the ConnContext structure, but
 * doesn't yet do anything with it. */
DLL void otrl_context_set_preshared_secret(ConnContext *context,
    const unsigned char *secret, size_t secret_len);

/* Force a context into the OTRL_MSGSTATE_FINISHED state. */
DLL void otrl_context_force_finished(ConnContext *context);

/* Force a context into the OTRL_MSGSTATE_PLAINTEXT state. */
DLL void otrl_context_force_plaintext(ConnContext *context);

/* Forget a fingerprint (so long as it's not the active one.  If it's a
 * fingerprint_root, forget the whole context (as long as
 * and_maybe_context is set, and it's PLAINTEXT).  Also, if it's not
 * the fingerprint_root, but it's the only fingerprint, and we're
 * PLAINTEXT, forget the whole context if and_maybe_context is set. */
DLL void otrl_context_forget_fingerprint(Fingerprint *fprint,
    int and_maybe_context);

/* Forget a whole context, so long as it's PLAINTEXT. */
DLL void otrl_context_forget(ConnContext *context);

/* Forget all the contexts in a given OtrlUserState. */
DLL void otrl_context_forget_all(OtrlUserState us);

#endif

/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __TLV_H__
#define __TLV_H__

typedef struct s_OtrlTLV {
    unsigned short type;
    unsigned short len;
    unsigned char *data;
    struct s_OtrlTLV *next;
} OtrlTLV;

/* TLV types */

/* This is just padding for the encrypted message, and should be ignored. */
#define OTRL_TLV_PADDING         0x0000

/* The sender has thrown away his OTR session keys with you */
#define OTRL_TLV_DISCONNECTED    0x0001

/* The message contains a step in the Socialist Millionaires' Protocol. */
#define OTRL_TLV_SMP1            0x0002
#define OTRL_TLV_SMP2            0x0003
#define OTRL_TLV_SMP3            0x0004
#define OTRL_TLV_SMP4            0x0005
#define OTRL_TLV_SMP_ABORT       0x0006
/* Like OTRL_TLV_SMP1, but there's a question for the buddy at the
 * beginning */
#define OTRL_TLV_SMP1Q           0x0007

/* Make a single TLV, copying the supplied data */
DLL OtrlTLV *otrl_tlv_new(unsigned short type, unsigned short len,
    const unsigned char *data);

/* Construct a chain of TLVs from the given data */
DLL OtrlTLV *otrl_tlv_parse(const unsigned char *serialized, size_t seriallen);

/* Deallocate a chain of TLVs */
DLL void otrl_tlv_free(OtrlTLV *tlv);

/* Find the serialized length of a chain of TLVs */
DLL size_t otrl_tlv_seriallen(const OtrlTLV *tlv);

/* Serialize a chain of TLVs.  The supplied buffer must already be large
 * enough. */
DLL void otrl_tlv_serialize(unsigned char *buf, const OtrlTLV *tlv);

/* Return the first TLV with the given type in the chain, or NULL if one
 * isn't found.  (The tlvs argument isn't const because the return type
 * needs to be non-const.) */
DLL OtrlTLV *otrl_tlv_find(OtrlTLV *tlvs, unsigned short type);

#endif


/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __PROTO_H__
#define __PROTO_H__

//#include "context.h"
//#include "version.h"
//#include "tlv.h"

/* If we ever see this sequence in a plaintext message, we'll assume the
 * other side speaks OTR, and try to establish a connection. */
#define OTRL_MESSAGE_TAG_BASE " \t  \t\t\t\t \t \t \t  "
/* The following must each be of length 8 */
#define OTRL_MESSAGE_TAG_V1 " \t \t  \t "
#define OTRL_MESSAGE_TAG_V2 "  \t\t  \t "

/* The possible flags contained in a Data Message */
#define OTRL_MSGFLAGS_IGNORE_UNREADABLE		0x01

typedef unsigned int OtrlPolicy;

#define OTRL_POLICY_ALLOW_V1			0x01
#define OTRL_POLICY_ALLOW_V2			0x02
#define OTRL_POLICY_REQUIRE_ENCRYPTION		0x04
#define OTRL_POLICY_SEND_WHITESPACE_TAG		0x08
#define OTRL_POLICY_WHITESPACE_START_AKE	0x10
#define OTRL_POLICY_ERROR_START_AKE		0x20

#define OTRL_POLICY_VERSION_MASK (OTRL_POLICY_ALLOW_V1 | OTRL_POLICY_ALLOW_V2)

/* For v1 compatibility */
#define OTRL_POLICY_NEVER			0x00
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
#define OTRL_POLICY_DEFAULT OTRL_POLICY_OPPORTUNISTIC

typedef enum {
    OTRL_MSGTYPE_NOTOTR,
    OTRL_MSGTYPE_TAGGEDPLAINTEXT,
    OTRL_MSGTYPE_QUERY,
    OTRL_MSGTYPE_DH_COMMIT,
    OTRL_MSGTYPE_DH_KEY,
    OTRL_MSGTYPE_REVEALSIG,
    OTRL_MSGTYPE_SIGNATURE,
    OTRL_MSGTYPE_V1_KEYEXCH,
    OTRL_MSGTYPE_DATA,
    OTRL_MSGTYPE_ERROR,
    OTRL_MSGTYPE_UNKNOWN
} OtrlMessageType;

typedef enum {
    OTRL_FRAGMENT_UNFRAGMENTED,
    OTRL_FRAGMENT_INCOMPLETE,
    OTRL_FRAGMENT_COMPLETE
} OtrlFragmentResult;

typedef enum {
    OTRL_FRAGMENT_SEND_ALL,
    OTRL_FRAGMENT_SEND_ALL_BUT_FIRST,
    OTRL_FRAGMENT_SEND_ALL_BUT_LAST
} OtrlFragmentPolicy;

/* Initialize the OTR library.  Pass the version of the API you are
 * using. */
DLL void otrl_init(unsigned int ver_major, unsigned int ver_minor,
    unsigned int ver_sub);

/* Shortcut */
#define OTRL_INIT do { \
    otrl_init(OTRL_VERSION_MAJOR, OTRL_VERSION_MINOR, OTRL_VERSION_SUB); \
    } while(0)

/* Return a pointer to a static string containing the version number of
 * the OTR library. */
DLL const char *otrl_version(void);

/* Return a pointer to a newly-allocated OTR query message, customized
 * with our name.  The caller should free() the result when he's done
 * with it. */
DLL char *otrl_proto_default_query_msg(const char *ourname, OtrlPolicy policy);

/* Return the best version of OTR support by both sides, given an OTR
 * Query Message and the local policy. */
DLL unsigned int otrl_proto_query_bestversion(const char *querymsg,
    OtrlPolicy policy);

/* Locate any whitespace tag in this message, and return the best
 * version of OTR support on both sides.  Set *starttagp and *endtagp to
 * the start and end of the located tag, so that it can be snipped out. */
DLL unsigned int otrl_proto_whitespace_bestversion(const char *msg,
    const char **starttagp, const char **endtagp, OtrlPolicy policy);

/* Return the Message type of the given message. */
DLL OtrlMessageType otrl_proto_message_type(const char *message);

/* Create an OTR Data message.  Pass the plaintext as msg, and an
 * optional chain of TLVs.  A newly-allocated string will be returned in
 * *encmessagep. */
DLL gcry_error_t otrl_proto_create_data(char **encmessagep, ConnContext *context,
    const char *msg, const OtrlTLV *tlvs, unsigned char flags);

/* Extract the flags from an otherwise unreadable Data Message. */
DLL gcry_error_t otrl_proto_data_read_flags(const char *datamsg,
    unsigned char *flagsp);

/* Accept an OTR Data Message in datamsg.  Decrypt it and put the
 * plaintext into *plaintextp, and any TLVs into tlvsp.  Put any
 * received flags into *flagsp (if non-NULL). */
DLL gcry_error_t otrl_proto_accept_data(char **plaintextp, OtrlTLV **tlvsp,
    ConnContext *context, const char *datamsg, unsigned char *flagsp);

/* Accumulate a potential fragment into the current context. */
DLL OtrlFragmentResult otrl_proto_fragment_accumulate(char **unfragmessagep,
    ConnContext *context, const char *msg);

DLL gcry_error_t otrl_proto_fragment_create(int mms, int fragment_count,
    char ***fragments, const char *message);

DLL void otrl_proto_fragment_free(char ***fragments, unsigned short arraylen);

#endif

/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __MEM_H__
#define __MEM_H__

DLL void otrl_mem_init(void);

#endif
/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

typedef enum {
    OTRL_NOTIFY_ERROR,
    OTRL_NOTIFY_WARNING,
    OTRL_NOTIFY_INFO
} OtrlNotifyLevel;

typedef struct s_OtrlMessageAppOps {
    /* Return the OTR policy for the given context. */
    OtrlPolicy (*policy)(void *opdata, ConnContext *context);

    /* Create a private key for the given accountname/protocol if
     * desired. */
    void (*create_privkey)(void *opdata, const char *accountname,
        const char *protocol);

    /* Report whether you think the given user is online.  Return 1 if
     * you think he is, 0 if you think he isn't, -1 if you're not sure.
     *
     * If you return 1, messages such as heartbeats or other
     * notifications may be sent to the user, which could result in "not
     * logged in" errors if you're wrong. */
    int (*is_logged_in)(void *opdata, const char *accountname,
        const char *protocol, const char *recipient);

    /* Send the given IM to the given recipient from the given
     * accountname/protocol. */
    void (*inject_message)(void *opdata, const char *accountname,
        const char *protocol, const char *recipient, const char *message);

    /* Display a notification message for a particular accountname /
     * protocol / username conversation. */
    void (*notify)(void *opdata, OtrlNotifyLevel level,
        const char *accountname, const char *protocol,
        const char *username, const char *title,
        const char *primary, const char *secondary);

    /* Display an OTR control message for a particular accountname /
     * protocol / username conversation.  Return 0 if you are able to
     * successfully display it.  If you return non-0 (or if this
     * function is NULL), the control message will be displayed inline,
     * as a received message, or else by using the above notify()
     * callback. */
    int (*display_otr_message)(void *opdata, const char *accountname,
        const char *protocol, const char *username, const char *msg);

    /* When the list of ConnContexts changes (including a change in
     * state), this is called so the UI can be updated. */
    void (*update_context_list)(void *opdata);

    /* Return a newly allocated string containing a human-friendly name
     * for the given protocol id */
    const char *(*protocol_name)(void *opdata, const char *protocol);

    /* Deallocate a string allocated by protocol_name */
    void (*protocol_name_free)(void *opdata, const char *protocol_name);

    /* A new fingerprint for the given user has been received. */
    void (*new_fingerprint)(void *opdata, OtrlUserState us,
        const char *accountname, const char *protocol,
        const char *username, unsigned char fingerprint[20]);

    /* The list of known fingerprints has changed.  Write them to disk. */
    void (*write_fingerprints)(void *opdata);

    /* A ConnContext has entered a secure state. */
    void (*gone_secure)(void *opdata, ConnContext *context);

    /* A ConnContext has left a secure state. */
    void (*gone_insecure)(void *opdata, ConnContext *context);

    /* We have completed an authentication, using the D-H keys we
     * already knew.  is_reply indicates whether we initiated the AKE. */
    void (*still_secure)(void *opdata, ConnContext *context, int is_reply);

    /* Log a message.  The passed message will end in "\n". */
    void (*log_message)(void *opdata, const char *message);

    /* Find the maximum message size supported by this protocol. */
    int (*max_message_size)(void *opdata, ConnContext *context);

    /* Return a newly allocated string containing a human-friendly
     * representation for the given account */
    const char *(*account_name)(void *opdata, const char *account,
        const char *protocol);

    /* Deallocate a string returned by account_name */
    void (*account_name_free)(void *opdata, const char *account_name);

} OtrlMessageAppOps;

/* Deallocate a message allocated by other otrl_message_* routines. */
DLL void otrl_message_free(char *message);

/* Handle a message about to be sent to the network.  It is safe to pass
 * all messages about to be sent to this routine.  add_appdata is a
 * function that will be called in the event that a new ConnContext is
 * created.  It will be passed the data that you supplied, as well as a
 * pointer to the new ConnContext.  You can use this to add
 * application-specific information to the ConnContext using the
 * "context->app" field, for example.  If you don't need to do this, you
 * can pass NULL for the last two arguments of otrl_message_sending.
 *
 * tlvs is a chain of OtrlTLVs to append to the private message.  It is
 * usually correct to just pass NULL here.
 *
 * If this routine returns non-zero, then the library tried to encrypt
 * the message, but for some reason failed.  DO NOT send the message in
 * the clear in that case.
 *
 * If *messagep gets set by the call to something non-NULL, then you
 * should replace your message with the contents of *messagep, and
 * send that instead.  Call otrl_message_free(*messagep) when you're
 * done with it. */
DLL gcry_error_t otrl_message_sending(OtrlUserState us,
    const OtrlMessageAppOps *ops,
    void *opdata, const char *accountname, const char *protocol,
    const char *recipient, const char *message, OtrlTLV *tlvs,
    char **messagep,
    void (*add_appdata)(void *data, ConnContext *context),
    void *data);

/* Handle a message just received from the network.  It is safe to pass
 * all received messages to this routine.  add_appdata is a function
 * that will be called in the event that a new ConnContext is created.
 * It will be passed the data that you supplied, as well as
 * a pointer to the new ConnContext.  You can use this to add
 * application-specific information to the ConnContext using the
 * "context->app" field, for example.  If you don't need to do this, you
 * can pass NULL for the last two arguments of otrl_message_receiving.
 *
 * If otrl_message_receiving returns 1, then the message you received
 * was an internal protocol message, and no message should be delivered
 * to the user.
 *
 * If it returns 0, then check if *messagep was set to non-NULL.  If
 * so, replace the received message with the contents of *messagep, and
 * deliver that to the user instead.  You must call
 * otrl_message_free(*messagep) when you're done with it.  If tlvsp is
 * non-NULL, *tlvsp will be set to a chain of any TLVs that were
 * transmitted along with this message.  You must call
 * otrl_tlv_free(*tlvsp) when you're done with those.
 *
 * If otrl_message_receiving returns 0 and *messagep is NULL, then this
 * was an ordinary, non-OTR message, which should just be delivered to
 * the user without modification. */
DLL int otrl_message_receiving(OtrlUserState us, const OtrlMessageAppOps *ops,
    void *opdata, const char *accountname, const char *protocol,
    const char *sender, const char *message, char **newmessagep,
    OtrlTLV **tlvsp,
    void (*add_appdata)(void *data, ConnContext *context),
    void *data);

/* Send a message to the network, fragmenting first if necessary.
 * All messages to be sent to the network should go through this
 * method immediately before they are sent, ie after encryption. */
DLL gcry_error_t otrl_message_fragment_and_send(const OtrlMessageAppOps *ops,
    void *opdata, ConnContext *context, const char *message,
    OtrlFragmentPolicy fragPolicy, char **returnFragment);

/* Put a connection into the PLAINTEXT state, first sending the
 * other side a notice that we're doing so if we're currently ENCRYPTED,
 * and we think he's logged in. */
DLL void otrl_message_disconnect(OtrlUserState us, const OtrlMessageAppOps *ops,
    void *opdata, const char *accountname, const char *protocol,
    const char *username);

/* Initiate the Socialist Millionaires' Protocol */
DLL void otrl_message_initiate_smp(OtrlUserState us, const OtrlMessageAppOps *ops,
    void *opdata, ConnContext *context, const unsigned char *secret,
    size_t secretlen);

/* Initiate the Socialist Millionaires' Protocol and send a prompt
 * question to the buddy */
DLL void otrl_message_initiate_smp_q(OtrlUserState us,
    const OtrlMessageAppOps *ops, void *opdata, ConnContext *context,
    const char *question, const unsigned char *secret, size_t secretlen);

/* Respond to a buddy initiating the Socialist Millionaires' Protocol */
DLL void otrl_message_respond_smp(OtrlUserState us, const OtrlMessageAppOps *ops,
    void *opdata, ConnContext *context, const unsigned char *secret,
    size_t secretlen);

/* Abort the SMP.  Called when an unexpected SMP message breaks the
 * normal flow. */
DLL void otrl_message_abort_smp(OtrlUserState us, const OtrlMessageAppOps *ops,
    void *opdata, ConnContext *context);

#endif
/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __PRIVKEY_H__
#define __PRIVKEY_H__

#include <stdio.h>
//#include "privkey-t.h"
//#include "userstate.h"

/* Convert a 20-byte hash value to a 45-byte human-readable value */
DLL void otrl_privkey_hash_to_human(char human[45], const unsigned char hash[20]);

/* Calculate a human-readable hash of our DSA public key.  Return it in
 * the passed fingerprint buffer.  Return NULL on error, or a pointer to
 * the given buffer on success. */
DLL char *otrl_privkey_fingerprint(OtrlUserState us, char fingerprint[45],
    const char *accountname, const char *protocol);

/* Calculate a raw hash of our DSA public key.  Return it in the passed
 * fingerprint buffer.  Return NULL on error, or a pointer to the given
 * buffer on success. */
DLL unsigned char *otrl_privkey_fingerprint_raw(OtrlUserState us,
    unsigned char hash[20], const char *accountname, const char *protocol);

/* Read a sets of private DSA keys from a file on disk into the given
 * OtrlUserState. */
DLL gcry_error_t otrl_privkey_read(OtrlUserState us, const char *filename);

/* Read a sets of private DSA keys from a FILE* into the given
 * OtrlUserState.  The FILE* must be open for reading. */
DLL gcry_error_t otrl_privkey_read_FILEp(OtrlUserState us, FILE *privf);

/* Generate a private DSA key for a given account, storing it into a
 * file on disk, and loading it into the given OtrlUserState.  Overwrite any
 * previously generated keys for that account in that OtrlUserState. */
DLL gcry_error_t otrl_privkey_generate(OtrlUserState us, const char *filename,
    const char *accountname, const char *protocol);

/* Generate a private DSA key for a given account, storing it into a
 * FILE*, and loading it into the given OtrlUserState.  Overwrite any
 * previously generated keys for that account in that OtrlUserState.
 * The FILE* must be open for reading and writing. */
DLL gcry_error_t otrl_privkey_generate_FILEp(OtrlUserState us, FILE *privf,
    const char *accountname, const char *protocol);

/* Read the fingerprint store from a file on disk into the given
 * OtrlUserState.  Use add_app_data to add application data to each
 * ConnContext so created. */
DLL gcry_error_t otrl_privkey_read_fingerprints(OtrlUserState us,
    const char *filename,
    void (*add_app_data)(void *data, ConnContext *context),
    void  *data);

/* Read the fingerprint store from a FILE* into the given
 * OtrlUserState.  Use add_app_data to add application data to each
 * ConnContext so created.  The FILE* must be open for reading. */
DLL gcry_error_t otrl_privkey_read_fingerprints_FILEp(OtrlUserState us,
    FILE *storef,
    void (*add_app_data)(void *data, ConnContext *context),
    void  *data);

/* Write the fingerprint store from a given OtrlUserState to a file on disk. */
DLL gcry_error_t otrl_privkey_write_fingerprints(OtrlUserState us,
    const char *filename);

/* Write the fingerprint store from a given OtrlUserState to a FILE*.
 * The FILE* must be open for writing. */
DLL gcry_error_t otrl_privkey_write_fingerprints_FILEp(OtrlUserState us,
    FILE *storef);

/* Fetch the private key from the given OtrlUserState associated with
 * the given account */
DLL OtrlPrivKey *otrl_privkey_find(OtrlUserState us, const char *accountname,
    const char *protocol);

/* Forget a private key */
DLL void otrl_privkey_forget(OtrlPrivKey *privkey);

/* Forget all private keys in a given OtrlUserState. */
DLL void otrl_privkey_forget_all(OtrlUserState us);

/* Sign data using a private key.  The data must be small enough to be
 * signed (i.e. already hashed, if necessary).  The signature will be
 * returned in *sigp, which the caller must free().  Its length will be
 * returned in *siglenp. */
DLL gcry_error_t otrl_privkey_sign(unsigned char **sigp, size_t *siglenp,
    OtrlPrivKey *privkey, const unsigned char *data, size_t len);

/* Verify a signature on data using a public key.  The data must be
 * small enough to be signed (i.e. already hashed, if necessary). */
DLL gcry_error_t otrl_privkey_verify(const unsigned char *sigbuf, size_t siglen,
    unsigned short pubkey_type, gcry_sexp_t pubs,
    const unsigned char *data, size_t len);

#endif
/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#undef DEBUG

#ifdef DEBUG

#include <stdio.h>

#define debug_data(t,b,l) do { const unsigned char *data = (b); size_t i; \
    fprintf(stderr, "%s: ", (t)); \
    for(i=0;i<(l);++i) { \
        fprintf(stderr, "%02x", data[i]); \
    } \
    fprintf(stderr, "\n"); \
    } while(0)

#define debug_int(t,b) do { const unsigned char *data = (b); \
    unsigned int v = \
        (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]; \
    fprintf(stderr, "%s: %u (0x%x)\n", (t), v, v); \
    } while(0)

#else
#define debug_data(t,b,l)
#define debug_int(t,b)
#endif

#define write_int(x) do { \
    bufp[0] = ((x) >> 24) & 0xff; \
    bufp[1] = ((x) >> 16) & 0xff; \
    bufp[2] = ((x) >> 8) & 0xff; \
    bufp[3] = (x) & 0xff; \
    bufp += 4; lenp -= 4; \
    } while(0)

#define write_mpi(x,nx,dx) do { \
    write_int(nx); \
    gcry_mpi_print(format, bufp, lenp, NULL, (x)); \
    debug_data((dx), bufp, (nx)); \
    bufp += (nx); lenp -= (nx); \
    } while(0)

#define require_len(l) do { \
    if (lenp < (l)) goto invval; \
    } while(0)

#define read_int(x) do { \
    require_len(4); \
    (x) = (bufp[0] << 24) | (bufp[1] << 16) | (bufp[2] << 8) | bufp[3]; \
    bufp += 4; lenp -= 4; \
    } while(0)

#define read_mpi(x) do { \
    size_t mpilen; \
    read_int(mpilen); \
    if (mpilen) { \
        require_len(mpilen); \
        gcry_mpi_scan(&(x), GCRYMPI_FMT_USG, bufp, mpilen, NULL); \
    } else { \
        (x) = gcry_mpi_set_ui(NULL, 0); \
    } \
    bufp += mpilen; lenp -= mpilen; \
    } while(0)

#endif
/*
 *  Off-the-Record Messaging library
 *  Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General
 *  Public License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __VERSION_H__
#define __VERSION_H__

#define OTRL_VERSION "3.2.0"

#define OTRL_VERSION_MAJOR 3
#define OTRL_VERSION_MINOR 2
#define OTRL_VERSION_SUB 0

#endif
}
#endif
#endif
