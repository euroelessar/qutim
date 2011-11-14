/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef MIRANDA_H_
#define MIRANDA_H_
#include "../../include/qutim/historymanager.h"

namespace HistoryManager {

namespace Miranda {

// All this typenames are from Miranda sources

typedef quint32 DWORD;
typedef quint16 WORD;
typedef quint8  BYTE;
typedef quint16 WCHAR;
typedef qint8   TCHAR;

// TODO: Validate database by signatures
extern const char *DBHEADER_SIGNATURE;

struct DBHeader {
  BYTE signature[16];      // 'Miranda ICQ DB',0,26
  DWORD version;		   //as 4 bytes, ie 1.2.3.10=0x0102030a
						   //this version is 0x00000700
  DWORD ofsFileEnd;		   //offset of the end of the database - place to write
						   //new structures
  DWORD slackSpace;		   //a counter of the number of bytes that have been
						   //wasted so far due to deleting structures and/or
						   //re-making them at the end. We should compact when
						   //this gets above a threshold
  DWORD contactCount;	   //number of contacts in the chain,excluding the user
  DWORD ofsFirstContact;   //offset to first struct DBContact in the chain
  DWORD ofsUser;		   //offset to struct DBContact representing the user
  DWORD ofsFirstModuleName;	//offset to first struct DBModuleName in the chain
};

static const DWORD DBCONTACT_SIGNATURE = 0x43DECADEu;

struct DBContact {
  DWORD signature;
  DWORD ofsNext;			 //offset to the next contact in the chain. zero if
							 //this is the 'user' contact or the last contact
							 //in the chain
  DWORD ofsFirstSettings;	 //offset to the first DBContactSettings in the
							 //chain for this contact.
  DWORD eventCount;			 //number of events in the chain for this contact
  DWORD ofsFirstEvent,ofsLastEvent;	 //offsets to the first and last DBEvent in
									 //the chain for this contact
  DWORD ofsFirstUnreadEvent; //offset to the first (chronological) unread event
							 //in the chain, 0 if all are read
  DWORD timestampFirstUnread; //timestamp of the event at ofsFirstUnreadEvent
};

enum DBEF
{
	DBEF_FIRST =  1,		//this is the first event in the chain;
						//internal only: *do not* use this flag
	DBEF_SENT  =  2,		//this event was sent by the user. If not set this
						//event was received.
	DBEF_READ  =  4,		//event has been read by the user. It does not need
						//to be processed any more except for history.
	DBEF_RTL   =  8,		//event contains the right-to-left aligned text
	DBEF_UTF   = 16		//event contains a text in utf-8
};

enum EVENTTYPE
{
	EVENTTYPE_MESSAGE  = 0,
	EVENTTYPE_URL      = 1,
	EVENTTYPE_CONTACTS = 2,	//v0.1.2.2+
	EVENTTYPE_ADDED       = 1000,  //v0.1.1.0+: these used to be module-
	EVENTTYPE_AUTHREQUEST = 1001,  //specific codes, hence the module-
	EVENTTYPE_FILE        = 1002,  //specific limit has been raised to 2000
};

static const DWORD DBEVENT_SIGNATURE = 0x45DECADEu;

struct DBEvent {
  DWORD signature;
  DWORD ofsPrev,ofsNext;	 //offset to the previous and next events in the
							 //chain. Chain is sorted chronologically
  DWORD ofsModuleName;		 //offset to a DBModuleName struct of the name of
							 //the owner of this event
  DWORD timestamp;			 //seconds since 00:00:00 01/01/1970
  DWORD flags;				 //see m_database.h, db/event/add
  WORD eventType;			 //module-defined event type
  DWORD cbBlob;				 //number of bytes in the blob
  QByteArray blob;			 //the blob. module-defined formatting
};

static const DWORD DBMODULENAME_SIGNATURE = 0x4DDECADEu;

struct DBModuleName {
  DWORD signature;
  DWORD ofsNext;		//offset to the next module name in the chain
  BYTE cbName;			//number of characters in this module name
  QByteArray name;		//name, no nul terminator
};

static const DWORD DBCONTACTSETTINGS_SIGNATURE = 0x53DECADEu;

struct DBContactSettings {
  DWORD signature;
  DWORD ofsNext;		 //offset to the next contactsettings in the chain
  DWORD ofsModuleName;	 //offset to the DBModuleName of the owner of these
						 //settings
  DWORD cbBlob;			 //size of the blob in bytes. May be larger than the
						 //actual size for reducing the number of moves
						 //required using granularity in resizing
  QByteArray blob;		 //the blob. a back-to-back sequence of DBSetting
						 //structs, the last has cbName=0
};

//DBVARIANT: used by db/contact/getsetting and db/contact/writesetting
enum DBVT
{
	DBVT_DELETED = 0,    //this setting just got deleted, no other values are valid
	DBVT_BYTE    = 1,	  //bVal and cVal are valid
	DBVT_WORD    = 2,	  //wVal and sVal are valid
	DBVT_DWORD   = 4,	  //dVal and lVal are valid
	DBVT_ASCIIZ  = 255,	  //pszVal is valid
	DBVT_BLOB    = 254,	  //cpbVal and pbVal are valid
	DBVT_UTF8    = 253,   //pszVal is valid
	DBVT_WCHAR   = 252,   //pszVal is valid
	DBVT_TCHAR   = DBVT_WCHAR
};

static const DWORD DBVTF_VARIABLELENGTH = 0x80;
static const DWORD DBVTF_DENYUNICODE    = 0x10000;

typedef struct {
	BYTE type;
	union {
		BYTE bVal; char cVal;
		WORD wVal; short sVal;
		DWORD dVal; long lVal;
		struct {
			union {
				char *pszVal;
				TCHAR *ptszVal;
				WCHAR *pwszVal;
			};
			WORD cchVal;   //only used for db/contact/getsettingstatic
		};
		struct {
			WORD cpbVal;
			BYTE *pbVal;
		};
	};
} DBVARIANT;

class miranda : public HistoryImporter
{
public:
	miranda();
	virtual ~miranda();
	virtual void loadMessages(const QString &path);
	virtual bool validate(const QString &path);
	virtual QString name();
	virtual QIcon icon();
	virtual bool needCharset() { return true; }
	virtual bool chooseFile() { return true; }
};

}

typedef Miranda::miranda miranda;

}

#endif /*MIRANDA_H_*/

