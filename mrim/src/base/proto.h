//***************************************************************************
// $Id: proto.h,v 1.19 2009/08/01 xtazz Exp $
//***************************************************************************

#ifndef MRIM_PROTO_H
#define MRIM_PROTO_H

//#include <sys/types.h>

#define PROTO_VERSION_MAJOR     1
#define PROTO_VERSION_MINOR     19 //20
#define PROTO_VERSION ((((quint32)(PROTO_VERSION_MAJOR))<<16)|(quint32)(PROTO_VERSION_MINOR))

#define PROTO_MAJOR(p) (((p)&0xFFFF0000)>>16)
#define PROTO_MINOR(p) ((p)&0x0000FFFF)


typedef struct mrim_packet_header_t
{
    quint32       magic;
    quint32       proto;
    quint32       seq;
    quint32       msg;
    quint32       dlen;
    quint32       from;
    quint32	  fromport;
    QByteArray	  reserved;
} mrim_packet_header_t;

#define HEADER_SIZE 44

#define CS_MAGIC    0xDEADBEEF		// ���������� Magic ( C <-> S )


// UNICODE = (UTF-16LE) (>=1.17)

/***************************************************************************

		�������� ����� ������-������

 ***************************************************************************/

#define MRIM_CS_HELLO       	0x1001  // C -> S
// empty

#define MRIM_CS_HELLO_ACK   	0x1002  // S -> C
// mrim_connection_params_t


#define MRIM_CS_LOGIN_ACK   	0x1004  // S -> C
// empty

#define MRIM_CS_LOGIN_REJ   	0x1005  // S -> C
// LPS reason ???

#define MRIM_CS_PING        	0x1006  // C -> S
// empty

#define MRIM_CS_MESSAGE			0x1008  // C -> S
// UL flags
	#define MESSAGE_FLAG_OFFLINE		0x00000001
	#define MESSAGE_FLAG_NORECV			0x00000004
	#define MESSAGE_FLAG_AUTHORIZE		0x00000008 	// X-MRIM-Flags: 00000008
	#define MESSAGE_FLAG_SYSTEM			0x00000040
	#define MESSAGE_FLAG_RTF			0x00000080
	#define MESSAGE_FLAG_CONTACT		0x00000200
	#define MESSAGE_FLAG_NOTIFY			0x00000400
	#define MESSAGE_FLAG_SMS			0x00000800
	#define MESSAGE_FLAG_MULTICAST		0x00001000
	#define MESSAGE_SMS_DELIVERY_REPORT	0x00002000
	#define MESSAGE_FLAG_ALARM			0x00004000
	#define MESSAGE_FLAG_FLASH			0x00008000
	#define MESSAGE_FLAG_SPAMF_SPAM		0x00020000	// ����� ����������� �� ���� - ������� ����� � ���� ������ ;������� ������������, �������� � ������ ��������� ��������� ��� �������� ������ ��������
	#define MESSAGE_FLAG_v1p16			0x00100000	// ��� ������������� �������
	#define MESSAGE_FLAG_CP1251			0x00200000
// LPS to e-mail ANSI
// LPS message ANSI/UNICODE (see flags)
// LPS rtf-formatted message (>=1.1) ???

#define MAX_MULTICAST_RECIPIENTS 50
	#define MESSAGE_USERFLAGS_MASK	0x000036A8	// Flags that user is allowed to set himself


#define MRIM_CS_MESSAGE_ACK		0x1009  // S -> C
// UL msg_id
// UL flags
// LPS from e-mail ANSI
// LPS message UNICODE
// LPS	//rtf-formatted message (>=1.1)	- MESSAGE_FLAG_RTF
//		//BASE64(						- MESSAGE_FLAG_AUTHORIZE
//				UL parts count = 2
//				LPS auth_sender_nick  UNICODE
//				LPS auth_request_text  UNICODE
//	)





#define MRIM_CS_MESSAGE_RECV	0x1011	// C -> S
// LPS from e-mail ANSI
// UL msg_id

#define MRIM_CS_MESSAGE_STATUS	0x1012	// S -> C
// UL status
	#define MESSAGE_DELIVERED				0x0000	// Message delivered directly to user
	#define MESSAGE_REJECTED_NOUSER			0x8001  // Message rejected - no such user
	#define MESSAGE_REJECTED_INTERR			0x8003	// Internal server error
	#define MESSAGE_REJECTED_LIMIT_EXCEEDED	0x8004	// Offline messages limit exceeded
	#define MESSAGE_REJECTED_TOO_LARGE		0x8005	// Message is too large
	#define	MESSAGE_REJECTED_DENY_OFFMSG	0x8006	// User does not accept offline messages
	#define	MESSAGE_REJECTED_DENY_OFFFLSH	0x8007	// User does not accept offline flash animation

#define MRIM_CS_USER_STATUS     0x100F  // S -> C
// UL status
	#define STATUS_OFFLINE          0x00000000
	#define STATUS_ONLINE           0x00000001
	#define STATUS_AWAY             0x00000002
	#define STATUS_UNDETERMINATED   0x00000003
	#define STATUS_USER_DEFINED     0x00000004
	#define STATUS_FLAG_INVISIBLE   0x80000000
// LPS spec_status_uri ANSI (>=1.14)
	#define SPEC_STATUS_URI_MAX 256
// LPS status_title UNICODE (>=1.14)
	#define STATUS_TITLE_MAX 16
// LPS status_desc UNICODE (>=1.14)
	#define STATUS_DESC_MAX 64
// LPS user e-mail ANSI
// UL com_support (>=1.14)
// LPS user_agent (>=1.14) ANSI
	#define USER_AGENT_MAX 255
	// Format:
	//  user_agent       = param *(param )
	//  param            = pname "=" pvalue
	//  pname            = token
	//  pvalue           = token / quoted-string
	//
	// Params:
	//  "client" - magent/jagent/???
	//  "name" - sys-name.
	//  "title" - display-name.
	//  "version" - product internal numeration. Examples: "1.2", "1.3 pre".
	//  "build" - product internal numeration (may be positive number or time).
	//  "protocol" - MMP protocol number by format "<major>.<minor>".


#define MRIM_CS_LOGOUT			0x1013	// S -> C
// UL reason
	#define LOGOUT_NO_RELOGIN_FLAG	0x0010		// Logout due to double login

#define MRIM_CS_CONNECTION_PARAMS	0x1014	// S -> C
// mrim_connection_params_t

#define MRIM_CS_USER_INFO			0x1015	// S -> C
// (LPS key, LPS value)* X ???
// MESSAGES.TOTAL - num UNICODE
// MESSAGES.UNREAD - num UNICODE
// MRIM.NICKNAME - nick UNICODE
// client.endpoint - ip:port UNICODE


#define MRIM_CS_ADD_CONTACT			0x1019	// C -> S
// UL flags (group(2) or usual(0) 
// UL group id (unused if contact is group)
// LPS contact e-mail ANSI
// LPS name UNICODE
// LPS custom phones ANSI
// LPS BASE64(
//				UL parts count = 2
//				LPS auth_sender_nick ???
//				LPS auth_request_text ???
//	)
// UL actions ( >= 1.15) 
	#define ADD_CONTACT_ACTION_FLAG_MYMAIL_INVITE   0x00000001 
	//used internal in win32 agent
	#define CONTACT_AWAITING_AUTHORIZATION_USER     0x00000100
	#define CONTACT_FLAG_TEMPORARY                  0x00010000 


#define MRIM_CS_ADD_CONTACT_ACK			0x101A	// S -> C
// UL status
	#define CONTACT_OPER_SUCCESS		0x0000
	#define CONTACT_OPER_ERROR			0x0001
	#define CONTACT_OPER_INTERR			0x0002
	#define CONTACT_OPER_NO_SUCH_USER	0x0003
	#define CONTACT_OPER_INVALID_INFO	0x0004
	#define CONTACT_OPER_USER_EXISTS	0x0005
	#define CONTACT_OPER_GROUP_LIMIT	0x6
// UL contact_id or (u_long)-1 if status is not OK


#define MRIM_CS_MODIFY_CONTACT			0x101B	// C -> S
// UL id
// UL flags - same as for MRIM_CS_ADD_CONTACT
// UL group id (unused if contact is group)
// LPS contact e-mail ANSI
// LPS name UNICODE
// LPS custom phones ANSI

#define MRIM_CS_MODIFY_CONTACT_ACK		0x101C	// S -> C
// UL status, same as for MRIM_CS_ADD_CONTACT_ACK

#define MRIM_CS_OFFLINE_MESSAGE_ACK		0x101D	// S -> C
// UIDL
// LPS offline message ???

#define MRIM_CS_DELETE_OFFLINE_MESSAGE	0x101E	// C -> S
// UIDL


#define MRIM_CS_AUTHORIZE				0x1020	// C -> S
// LPS user e-mail ANSI

#define MRIM_CS_AUTHORIZE_ACK			0x1021	// S -> C
// LPS user e-mail ANSI

#define MRIM_CS_CHANGE_STATUS			0x1022	// C -> S
// UL new status
// LPS spec_status_uri ANSI (>=1.14)
// LPS status_title UNICODE (>=1.14)
// LPS status_desc UNICODE (>=1.14)
// UL com_support (>=1.14) (see MRIM_CS_USER_STATUS)


#define MRIM_CS_GET_MPOP_SESSION		0x1024	// C -> S


#define MRIM_CS_MPOP_SESSION			0x1025	// S -> C
// UL status 
	#define MRIM_GET_SESSION_FAIL		0
	#define MRIM_GET_SESSION_SUCCESS	1
// LPS mpop session ???


#define MRIM_CS_FILE_TRANSFER			0x1026  // C->S 
// LPS TO/FROM e-mail ANSI
// DWORD id_request - uniq per connect 
// DWORD FILESIZE 
// LPS:	// LPS Files (FileName;FileSize;FileName;FileSize;) ANSI
		// LPS DESCRIPTION:
							// UL ?
							// Files (FileName;FileSize;FileName;FileSize;) UNICODE
		// LPS Conn (IP:Port;IP:Port;) ANSI

#define MRIM_CS_FILE_TRANSFER_ACK		0x1027 // S->C 
// DWORD status 
	#define FILE_TRANSFER_STATUS_OK                 1 
	#define FILE_TRANSFER_STATUS_DECLINE            0 
	#define FILE_TRANSFER_STATUS_ERROR              2 
	#define FILE_TRANSFER_STATUS_INCOMPATIBLE_VERS  3 
	#define FILE_TRANSFER_MIRROR                    4 
// LPS TO/FROM e-mail ANSI
// DWORD id_request 
// LPS DESCRIPTION [Conn (IP:Port;IP:Port;) ANSI]



//white pages!
#define MRIM_CS_WP_REQUEST			0x1029 //C->S
// DWORD field
// LPS value ???
#define PARAMS_NUMBER_LIMIT			50
#define PARAM_VALUE_LENGTH_LIMIT	64

//if last symbol in value eq '*' it will be replaced by LIKE '%' 
// params define
// must be  in consecutive order (0..N) to quick check in check_anketa_info_request
	enum {
	MRIM_CS_WP_REQUEST_PARAM_USER		= 0,// ANSI
	MRIM_CS_WP_REQUEST_PARAM_DOMAIN,		// ANSI
	MRIM_CS_WP_REQUEST_PARAM_NICKNAME,		// UNICODE
	MRIM_CS_WP_REQUEST_PARAM_FIRSTNAME,		// UNICODE
	MRIM_CS_WP_REQUEST_PARAM_LASTNAME,		// UNICODE
	MRIM_CS_WP_REQUEST_PARAM_SEX	,		// ANSI
	MRIM_CS_WP_REQUEST_PARAM_BIRTHDAY,		// not used for search
	MRIM_CS_WP_REQUEST_PARAM_DATE1	,		// ANSI
	MRIM_CS_WP_REQUEST_PARAM_DATE2	,		// ANSI
	//!!!!!!!!!!!!!!!!!!!online request param must be at end of request!!!!!!!!!!!!!!!
	MRIM_CS_WP_REQUEST_PARAM_ONLINE	,		// ANSI
	MRIM_CS_WP_REQUEST_PARAM_STATUS	,		// we do not used it, yet
	MRIM_CS_WP_REQUEST_PARAM_CITY_ID,		// ANSI
	MRIM_CS_WP_REQUEST_PARAM_ZODIAC,		// ANSI
	MRIM_CS_WP_REQUEST_PARAM_BIRTHDAY_MONTH,// ANSI	
	MRIM_CS_WP_REQUEST_PARAM_BIRTHDAY_DAY,	// ANSI
	MRIM_CS_WP_REQUEST_PARAM_COUNTRY_ID,	// ANSI
	MRIM_CS_WP_REQUEST_PARAM_MAX		
	};

#define MRIM_CS_ANKETA_INFO			0x1028 //S->C
// DWORD status 
	#define MRIM_ANKETA_INFO_STATUS_OK			1
	#define MRIM_ANKETA_INFO_STATUS_NOUSER		0
	#define MRIM_ANKETA_INFO_STATUS_DBERR		2
	#define MRIM_ANKETA_INFO_STATUS_RATELIMERR	3
// DWORD fields_num				
// DWORD max_rows
// DWORD server_time sec since 1970 (unixtime)
	// fields set 				//%fields_num == 0
	// values set 				//%fields_num == 0
// LPS value (numbers too) ???


#define MRIM_CS_MAILBOX_STATUS			0x1033	
// DWORD new messages in mailbox


#define MRIM_CS_GAME                    0x1035
// LPS to/from e-mail ANSI
// DWORD session unique per game
// DWORD msg internal game message
	enum {
	GAME_BASE, 
	GAME_CONNECTION_INVITE,
	GAME_CONNECTION_ACCEPT,
	GAME_DECLINE,
	GAME_INC_VERSION,
	GAME_NO_SUCH_GAME,
	GAME_JOIN,
	GAME_CLOSE,
	GAME_SPEED,
	GAME_SYNCHRONIZATION,
	GAME_USER_NOT_FOUND,
	GAME_ACCEPT_ACK,
	GAME_PING,
	GAME_RESULT,
	GAME_MESSAGES_NUMBER
	};
// DWORD msg_id id for ack
// DWORD time_send time of client
// LPS data ???



#define MRIM_CS_CONTACT_LIST2		0x1037 //S->C
// UL status
	#define GET_CONTACTS_OK		0x0000
	#define GET_CONTACTS_ERROR	0x0001
	#define GET_CONTACTS_INTERR	0x0002
// DWORD status  - if ...OK than this staff:
// DWORD groups number
	// mask symbols table:
	// 's' - lps
	// 'u' - unsigned long
	// 'z' - zero terminated string 
	// LPS groups fields mask ANSI
	// LPS contacts fields mask ANSI
	// group fields
	// contacts fields
	// groups mask 'us' == flags, name UNICODE
	// contact mask 'uussuussssus' flags, group id, e-mail ANSI, nick UNICODE, server flags, status, custom phone numbers ANSI, spec_status_uri ANSI, status_title UNICODE, status_desc UNICODE, com_support (future flags), user_agent (formated string) ANSI
	//               uussuussssusuuus
	#define CONTACT_INTFLAG_NOT_AUTHORIZED	0x0001

#define MRIM_DEFAULT_GROUP_ID 0x0
#define MRIM_PHONE_GROUP_ID 0x67
#define MRIM_MAX_GROUPS 20

//old packet cs_login with cs_statistic
#define MRIM_CS_LOGIN2		0x1038  // C -> S
// LPS login e-mail ANSI
// LPS password ANSI
// DWORD status
// LPS spec_status_uri ANSI (>=1.14)
// LPS status_title UNICODE (>=1.14)
// LPS status_desc UNICODE (>=1.14)
// UL com_support (>=1.14) (see MRIM_CS_USER_STATUS)
// LPS user_agent ANSI (>=1.14) (see MRIM_CS_USER_STATUS)
	// + statistic packet data:
// LPS client description ANSI
	#define MAX_CLIENT_DESCRIPTION 1024




#define MRIM_CS_SMS       	0x1039  // C -> S
// UL flags
// LPS to Phone ???
// LPS message ???

#define MRIM_CS_SMS_ACK		0x1040 // S->C 
// UL status



#define MRIM_CS_PROXY		0x1044
// LPS          to e-mail ANSI
// DWORD        id_request
// DWORD        data_type
	#define MRIM_PROXY_TYPE_VOICE	1
	#define MRIM_PROXY_TYPE_FILES	2
	#define MRIM_PROXY_TYPE_CALLOUT	3
// LPS          user_data ???
// LPS          lps_ip_port ???
// DWORD        session_id[4]

#define MRIM_CS_PROXY_ACK	0x1045
//DWORD         status
	#define PROXY_STATUS_OK					1
	#define PROXY_STATUS_DECLINE			0
	#define PROXY_STATUS_ERROR				2
	#define PROXY_STATUS_INCOMPATIBLE_VERS	3
	#define PROXY_STATUS_NOHARDWARE			4
	#define PROXY_STATUS_MIRROR				5
	#define PROXY_STATUS_CLOSED				6
// LPS           to e-mail ANSI
// DWORD         id_request
// DWORD         data_type
// LPS           user_data ???
// LPS:          lps_ip_port ???
// DWORD[4]      Session_id

#define MRIM_CS_PROXY_HELLO			0x1046
// DWORD[4]      Session_id

#define MRIM_CS_PROXY_HELLO_ACK		0x1047






#define MRIM_CS_NEW_MAIL	0x1048 // S->C
// UL unread count
// LPS from e-mail ANSI
// LPS subject ???
// UL date
// UL uidl


#define MRIM_PHONE_GROUP_ID 0x67

typedef struct mrim_connection_params_t
{
	quint32 	ping_period;
}
mrim_connection_params_t;



#endif // MRIM_PROTO_H

