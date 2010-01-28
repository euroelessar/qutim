/*****************************************************************************
    MRIM protocol definitions

    Copyright (c) 2009 by Rusanov Peter <tazkrut@mail.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef MRIMDEFS_H
#define MRIMDEFS_H

#include <QHash>
#include <QFileInfo>

#define MAX_INT32 4294836225

//to detect corrupted headers
#define MAX_PACKET_BODY_SIZE 500 * 1024 //500 kbytes

struct MRIMUserInfo
{
	QString messagesTotal;
	QString messagesUnread;
	QString userNickname;
	bool userHasMyMail;
	QString userClientEndpoint;
};

enum LogoutReason
{
	EAnotherClientConnected,
	EAuthenticationFailed,
	EUnknownReason
};

enum CLOperationError
{
	ECLUnknownError,
	ECLNoSuchUser,
	ECLInternalServerError,
	ECLInvalidInfo,
	ECLUserAlreadyExists,
	ECLGroupLimitReached
};

struct ContactAdditionalInfo
{
	QString Nick;
	QString AvatarPath;
	QString ClientName;
	QString OtherInfo;
};

struct MRIMSearchParams
{
	QString EmailAddr;
	QString EmailDomain;
	QString Nick;
	QString Name;
	QString Surname;
	qint32 Sex;
	qint32 MinAge;
	qint32 MaxAge;
	qint32 CityId;
	qint32 CountryId;
	qint32 ZodiacId;
	qint32 BirthDay;
	qint32 BirthdayMonth;
	bool OnlineOnly;
	//not for search
        qint32 Status;
	QString LocationText;
	qint32 BirthYear;
};

enum CLItemType
{
        EAccount = 2,
        EGroup = 1,
        EContact = 0
};

struct FileTransferRequest
{
    QString From;
    QString To;
    quint32 UniqueId;
    quint32 SummarySize;
    QHash<QString,quint32> FilesDict;
    QHash<QString,quint32> IPsDict;
    QList<QFileInfo> FilesInfo;
};

#define MRIM_MSGQUEUE_MAX_LEN 10

#endif //MRIMDEFS_H
