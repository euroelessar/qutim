#include "growlbackend.h"
#include <QTextDocument>
#include <QVariant>
#include <libqutim/message.h>
#include <libqutim/buddy.h>
#include <libqutim/debug.h>
#include <QBuffer>
#import <GrowlApplicationBridge.h>

@interface GrowlInterface :NSObject <GrowlApplicationBridgeDelegate> {
}
-(void) SendGrowlAlert:(NSString*)message title:(NSString*)title image:(NSData*)image;
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
  NSArray* array = [NSArray arrayWithObjects:@"System", nil];  // Valid notification names.
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

-(void) SendGrowlAlert:(NSString*)message title:(NSString*)title image:(NSData*)image {
  [GrowlApplicationBridge notifyWithTitle:title
						  description:message
						  notificationName:@"System"
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
  void showMessage(Notifications::Type,
				   const QString& summary,
				   const QString& message,
				   const QImage& image) {

	NSString* mac_message = [[NSString alloc] initWithUTF8String:message.toUtf8().constData()];
	NSString* mac_summary = [[NSString alloc] initWithUTF8String:summary.toUtf8().constData()];

	NSData* image_data = nil;
	// Growl expects raw TIFF data.
	// This is nasty but it keeps the API nice.
	if (!image.isNull()) {
	  QByteArray tiff_data;
	  QBuffer tiff(&tiff_data);
	  image.save(&tiff, "TIFF");
	  image_data = [NSData dataWithBytes:tiff_data.constData() length:tiff_data.size()];
	}

	[growl_interface_ SendGrowlAlert:mac_message
					  title:mac_summary
					  image:image_data];

	[mac_message release];
	[mac_summary release];
  }

 private:
  GrowlInterface* growl_interface_;
};

GrowlBackend::GrowlBackend() : m_wrapper(new GrowlNotificationWrapper)
{

}

void GrowlBackend::show(qutim_sdk_0_3::Notifications::Type type, QObject* sender, const QString& body,
				   const QVariant& data)
{
	QString text = Qt::escape(body);
	QString sender_id = sender ? sender->property("id").toString() : QString();
	QString sender_name = sender ? sender->property("name").toString() : QString();
	if(sender_name.isEmpty())
		sender_name = sender_id;
	QString title = Notifications::toString(type).arg(sender_name);

	QImage img;
	if (data.canConvert<Message>() && (type & Notifications::MessageSend & Notifications::MessageGet)) {
		const Message &msg = data.value<qutim_sdk_0_3::Message>();
		title = qutim_sdk_0_3::Notifications::toString(type).arg(msg.chatUnit()->title());
		if (const Buddy *b = qobject_cast<const Buddy*>(msg.chatUnit()))
			img = QImage(b->avatar());

	} else if (data.canConvert<QString>()) {
		title = data.toString();
	}
	if (img.isNull() && sender)
		img = QImage(sender->property("avatar").toString());

	m_wrapper->showMessage(type,title, text, img);
}

GrowlBackend::~GrowlBackend()
{

}
