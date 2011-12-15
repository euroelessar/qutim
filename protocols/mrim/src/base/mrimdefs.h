/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Rusanov Peter <tazkrut@mail.ru>
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

#ifndef MRIMDEFS_H
#define MRIMDEFS_H

#include <QHash>
#include <QFileInfo>

#define MAX_INT32 4294836225

//to detect corrupted headers
#define MAX_PACKET_BODY_SIZE 500 * 1024 //500 kbytes

struct MRIMUserInfo
{
	int messagesTotal;
	int messagesUnread;
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

