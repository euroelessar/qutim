/****************************************************************************
 *  icq_global.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef ICQ_GLOBAL_H
#define ICQ_GLOBAL_H

#include <qutim/libqutim_global.h>
#include <qutim/debug.h>
#include <qutim/status.h>

namespace Icq {

enum SnacFamily
{
	ServiceFamily		    = 0x0001,
	LocationFamily          = 0x0002,
	BuddyFamily             = 0x0003,
	MessageFamily           = 0x0004,
	BosFamily               = 0x0009,
	LookupFamily            = 0x000a,
	StatsFamily			    = 0x000b,
	ChatNavigationFamily    = 0x000d,
	ChatFamily              = 0x000e,
	AvatarFamily            = 0x0010,
	ListsFamily             = 0x0013,
	ExtensionsFamily        = 0x0015,
	AuthorizationFamily     = 0x0017,
	DirectoryFamily         = 0x0025
};

enum ServiceFamilySubtype // 0x0001
{
	ServiceError                = 0x0001,
	ServiceClientReady          = 0x0002,
	ServiceServerReady          = 0x0003,
	ServiceClientNewService     = 0x0004,
	ServerRedirectService		= 0x0005,
	ServiceClientReqRateInfo    = 0x0006,
	ServiceServerAsksServices   = 0x0007,
	ServiceServerRateInfo       = 0x0007,
	ServiceClientRateAck        = 0x0008,
	ServiceServerRateChange     = 0x000a,
	ServiceServerPause          = 0x000b,
	ServiceClientPauseAck       = 0x000c,
	ServiceServerResume         = 0x000d,
	ServiceClientReqinfo        = 0x000e,
	ServiceServerNameInfo       = 0x000f,
	ServiceServerEvilNotice     = 0x0010,
	ServiceClientSetIdle        = 0x0011,
	ServiceServerMigrationreq   = 0x0012,
	ServiceServerMotd           = 0x0013,
	ServiceClientFamilies       = 0x0017,
	ServiceServerFamilies2      = 0x0018,
	ServiceClientSetStatus      = 0x001e,
	ServiceServerExtstatus      = 0x0021
};

enum LocationFamilySubtype // 0x0002
{
	LocationCliReqRights    = 0x0002,
	LocationRightsReply     = 0x0003,
	LocationSetUserInfo     = 0x0004,
	LocationReqUserInfo     = 0x0005,
	LocationUsrInfoReply    = 0x0006,
	LocationQryUserInfo     = 0x0015
};

enum BuddyFamilySubtype // 0x0003
{
	UserCliReqBuddy         = 0x0002,
	UserSrvReplyBuddy       = 0x0003,
	UserAddToList           = 0x0004,  /* Deprecated */
	UserRemoveFromList      = 0x0005,  /* Deprecated */
	UserNotifyRejected      = 0x000a,
	UserOnline              = 0x000b,
	UserOffline             = 0x000c,
	UserAddToList2          = 0x000f,
	UserRemoveFromList2     = 0x0010
};

enum MessageFamilySubtype // 0x0004
{
	MessageSrvError         = 0x0001,
	MessageCliSetParams     = 0x0002,
	MessageCliResetParams   = 0x0003,
	MessageCliReqIcbm       = 0x0004,
	MessageSrvReplyIcbm     = 0x0005,
	MessageSrvSend          = 0x0006,
	MessageSrvRecv          = 0x0007,
	MessageSrvMissedMessage = 0x000a,
	MessageResponse         = 0x000b,
	MessageSrvAck           = 0x000c,
	MessageCliReqOffline    = 0x0010,
	MessageMtn              = 0x0014,
	MessageSrvOfflineReply  = 0x0017
};

enum PrivacyFamilySubtype // 0x0009
{
	PrivacyReqRights            = 0x0002,
	PrivacyRightsReply          = 0x0003,
	PrivacyCliAddVisible        = 0x0005,
	PrivacyCliRemoveVisible     = 0x0006,
	PrivacyCliAddinVisible      = 0x0007,
	PrivacyCliRemoveInvisible   = 0x0008,
	PrivacyServiceError         = 0x0009,
	PrivacyCliAddTempVisible    = 0x000a,
	PrivacyCliRemoveTempVisible = 0x000b
};

enum LookupFamilySubtype // 0x000a
{
	LookupRequest       = 0x0002,
	LookupEmailReply    = 0x0003
};

enum StatsFamilySubtype // 0x000b
{
	StatsMinReportInterval  = 0x0002
};

enum AvatarFamilySubtype // 0x0010
{
	AvatarError         = 0x0001,
	AvatarUploadRequest	= 0x0002,
	AvatarUploadAck     = 0x0003,
	AvatarGetRequest    = 0x0006,
	AvatarGetReply      = 0x0007
};

enum ListsFamilySubtype // 0x0013
{
	ListsError              = 0x0001,
	ListsCliReqLists        = 0x0002,
	ListsSrvReplyLists      = 0x0003,
	ListsCliRequest         = 0x0004,
	ListsCliCheck           = 0x0005,
	ListsList               = 0x0006,
	ListsGotList            = 0x0007,
	ListsAddToList          = 0x0008,
	ListsUpdateGroup        = 0x0009,
	ListsRemoveFromList     = 0x000a,
	ListsAck                = 0x000e,
	ListsUpToDate           = 0x000f,
	ListsCliModifyStart     = 0x0011,
	ListsCliModifyEnd       = 0x0012,
	ListsGrantAuth          = 0x0014,
	ListsAuthGranted        = 0x0015,
	ListsRevokeAuth         = 0x0016,
	ListsRequestAuth        = 0x0018,
	ListsAuthRequest        = 0x0019,
	ListsCliAuthResponse    = 0x001a,
	ListsSrvAuthResponse    = 0x001b,
	ListsYouWereAAdded      = 0x001c
};

enum ExtensionsFamilySubtype // 0x0015
{
	ExtensionsMetaError         = 0x0001,
	ExtensionsMetaCliRequest    = 0x0002,
	ExtensionsMetaSrvReply      = 0x0003
};

enum AuthorizationFamilySubtype // 0x0017
{
	SignonError             = 0x0001,
	SignonLoginRequest      = 0x0002,
	SignonLoginReply        = 0x0003,
	SignonRegistrationReq   = 0x0004,
	SignonNewUin            = 0x0005,
	SignonAuthRequest       = 0x0006,
	SignonAuthKey           = 0x0007,
	SignonRequestImage      = 0x000c,
	SignonRegAuthImage      = 0x000d
};

enum SsiItemType
{
	SsiBuddy		= 0x0000,  // Buddy record (name: uin for ICQ and screenname for AIM)
	SsiGroup        = 0x0001,  // Group record
	SsiPermit       = 0x0002,  // Permit record ("Allow" list in AIM, and "Visible" list in ICQ)
	SsiDeny         = 0x0003,  // Deny record ("Block" list in AIM, and "Invisible" list in ICQ)
	SsiVisibility   = 0x0004,  // Permit/deny settings or/and bitmask of the AIM classes
	SsiPresence     = 0x0005,  // Presence info (if others can see your idle status, etc)
	SsiClientData   = 0x0009,  // Client specific, e.g. ICQ2k shortcut bar items
	SsiIgnore       = 0x000e,  // Ignore list record.
	SsiLastUpdate   = 0x000f,  // Item that contain roster update time (name: "LastUpdateDate")
	SsiNonICQ       = 0x0010,  // Non-ICQ contact (to send SMS). Name: 1#EXT, 2#EXT, etc
	SsiUnknown2     = 0x0011,  // Unknown.
	SsiImportTime   = 0x0013,  // Item that contain roster import time (name: "Import time")
	SsiBuddyIcon    = 0x0014,  // Buddy icon info. (names: "1", "8", etc. according ot the icon type)
	SsiMetaInfo     = 0x0020   // Owner Details' token & last update time
};

const quint16 max_message_snac_size = 8000;
const quint16 client_rate_limit     = 0;
const quint16 not_in_list_group     = 2;
const quint8 protocol_version       = 9;
const quint16 client_features       = 0x3;
const quint16 dc_type               = 0x0004;

enum IcqStatus
{
// Status FLAGS (used to determine status of other users)
	IcqOffline        = 0xf000,
	IcqOnline         = 0x0000,
	IcqAway           = 0x0001,
	IcqDND            = 0x0002,
	IcqNA             = 0x0004,
	IcqOccupied       = 0x0010,
	IcqFFC            = 0x0020,
	IcqInvisible      = 0x0100,

// QIP Extended Status flags
	IcqEvil           = 0x3000,
	IcqDepress        = 0x4000,
	IcqHome           = 0x5000,
	IcqWork           = 0x6000,
	IcqLunch          = 0x2001
};

extern quint16 qutimStatusToICQ(const qutim_sdk_0_3::Status &status);
extern qutim_sdk_0_3::Status icqStatusToQutim(IcqStatus status);

} // namespace Icq

#endif // ICQ_GLOBAL_H
