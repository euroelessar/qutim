/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#include "growlbackend.h"
#include <QTextDocument>
#include <QVariant>
#include <qutim/message.h>
#include <qutim/buddy.h>
#include <qutim/debug.h>
#include <QBuffer>
#import <GrowlApplicationBridge.h>

@interface GrowlInterface :NSObject <GrowlApplicationBridgeDelegate> {
}
-(void) SendGrowlAlert:(NSString*)message type:(NSString*)type title:(NSString*)title image:(NSData*)image;
-(void) ClickCallback;  // Called when user clicks on notification.
@end

using namespace qutim_sdk_0_3;

@implementation GrowlInterface

-(id) init {
	if ((self = [super init])) {
		[GrowlApplicationBridge setGrowlDelegate:self];
	}
	return self;
}

-(void) dealloc {
	[super dealloc];
}

-(NSDictionary*) registrationDictionaryForGrowl {
	NSArray* array = [NSArray arrayWithObjects:@"Incoming Message", @"Outgoing Message",  @"qutIM Startup",
		@"Message blocked", @"User joined chat", @"User leaved chat", @"User leaved chat", @"Incoming message in chat",
		@"Outgoing message in chat", @"Filetransfer completed",  @"User online",  @"User offline",
		@"User offline",  @"User changed status", @"User has birhtday", @"User typing",  @"System", nil];  // Valid notification names.

	NSDictionary* dict = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithInt:1],
		@"TicketVersion",
		array,
		@"AllNotifications",
		array,
		@"DefaultNotifications",
		@"com.qutim.growlnotification",
		@"ApplicationId",
		nil];
	return dict;
}

-(void) growlNotificationWasClicked:(id)clickContext {
	if (clickContext) {
		[self ClickCallback];
	}
	return;
}

-(void) SendGrowlAlert:(NSString*)message type:(NSString*)type title:(NSString*)title image:(NSData*)image {
	[GrowlApplicationBridge notifyWithTitle:title
		description:message
		notificationName:type
		iconData:image
		priority:0
		isSticky:NO
		clickContext:@"click_callback"];  // String sent to our callback.
}

-(void) ClickCallback {
	qutim_sdk_0_3::debug() << "Growl notification clicked!";
	return;
}

@end

class GrowlNotificationWrapper {
public:
	GrowlNotificationWrapper() {
		growl_interface_ = [[GrowlInterface alloc] init];
	}

	~GrowlNotificationWrapper() {
		[growl_interface_ release];
	}
	void showMessage(Notification::Type type, const QString& summary, const QString& message, const QImage& image) {
		if (!type)
			return;
		NSString* mac_message = [[NSString alloc] initWithUTF8String:message.toUtf8().constData()];
		NSString* mac_summary = [[NSString alloc] initWithUTF8String:summary.toUtf8().constData()];
		NSString* mac_type = @"Unknown";
		switch (type) {
		case Notification::IncomingMessage:
			mac_type = @"Incoming Message";
			break;
		case Notification::OutgoingMessage:
			mac_type = @"Outgoing Message";
			break;
		case Notification::AppStartup:
			mac_type = @"qutIM Startup";
			break;
		case Notification::BlockedMessage:
			mac_type = @"Message blocked";
			break;
		case Notification::ChatUserJoined:
			mac_type = @"User joined chat";
			break;
		case Notification::ChatUserLeft:
			mac_type = @"User left chat";
			break;
		case Notification::ChatIncomingMessage:
			mac_type = @"Incoming message in chat";
			break;
		case Notification::ChatOutgoingMessage:
			mac_type = @"Outgoing message in chat";
			break;
		case Notification::FileTransferCompleted:
			mac_type = @"Filetransfer completed";
			break;
		case Notification::UserOnline:
			mac_type = @"User online";
			break;
		case Notification::UserOffline:
			mac_type = @"User offline";
			break;
		case Notification::UserChangedStatus:
			mac_type = @"User changed status";
			break;
		case Notification::UserHasBirthday:
			mac_type = @"User has birhtday";
			break;
		case Notification::UserTyping:
			mac_type = @"User typing";
			break;
		case Notification::System:
			mac_type = @"System";
			break;
		}

		NSData* image_data = nil;
		// Growl expects raw TIFF data.
		// This is nasty but it keeps the API nice.
		if (!image.isNull()) {
			QByteArray tiff_data;
			QBuffer tiff(&tiff_data);
			image.save(&tiff, "TIFF");
			image_data = [NSData dataWithBytes:tiff_data.constData() length:tiff_data.size()];
		}

		[growl_interface_ SendGrowlAlert:mac_message type:mac_type title:mac_summary image:image_data];

		[mac_message release];
		[mac_summary release];
		[mac_type release];
	}

private:
	GrowlInterface* growl_interface_;
};

GrowlBackend::GrowlBackend() : 
    NotificationBackend("Popup"),
    m_wrapper(new GrowlNotificationWrapper)
{

}

void GrowlBackend::handleNotification(Notification *notification)
{
	ref(notification);

	NotificationRequest request = notification->request();
	QImage img = request.image().toImage();
	m_wrapper->showMessage(request.type(), request.title(), request.text(), img);
	deref(notification);
}

GrowlBackend::~GrowlBackend()
{

}
