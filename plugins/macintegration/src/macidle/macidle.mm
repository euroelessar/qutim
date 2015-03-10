#import <Cocoa/Cocoa.h>

#include "macidle.h"
#include "macidle-global.h"
#include "macidlewidget.h"
#include <qutim/account.h>
#include <qutim/status.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <QTimer>

@interface IdleHandler : NSObject {
	@public
	MacIntegration::MacIdle *idleManager;
}
- (void)screenSaverStarted;
- (void)screenSaverStopped;
- (CFTimeInterval)currentIdle;
@end

@implementation IdleHandler
- (id)init
{
	if ((self = [super init])) {
		[[NSDistributedNotificationCenter defaultCenter] addObserver:self
			selector:@selector(screenSaverDidStart)
		  name:@"com.apple.screensaver.didstart"
		  object:nil];
		[[NSDistributedNotificationCenter defaultCenter] addObserver:self
			selector:@selector(screenSaverDidStop)
		  name:@"com.apple.screensaver.didstop"
		  object:nil];
	}
	return self;
}

- (CFTimeInterval)currentIdle
{
	return CGEventSourceSecondsSinceLastEventType(kCGEventSourceStateCombinedSessionState, kCGAnyInputEventType);
}

- (void)screenSaverStarted
{
	idleManager->setIdleOn(MacIntegration::MacIdle::Screensaver);
}

- (void)screenSaverStopped
{
	idleManager->setIdleOff();
}
@end

MacIntegration::MacIdle* pIdleStatusChanger = 0;

namespace MacIntegration
{
class MacIdlePrivate
{
public:
	IdleHandler *idleHandler;
	QTimer *timer;
	bool isAwayEnabled;
	int awayInterval;
	bool isInactiveEnabled;
	int inactiveInterval;
	MacIdle::Reason currentReason;
	QHash<Account *, Status> idleAccounts;
	QHash<Status::Type, QString> idleMessages;
};

MacIdle::MacIdle() : d_ptr(new MacIdlePrivate())
{
	Q_D(MacIdle);
	reloadSettings();
	d->idleHandler = [[IdleHandler alloc] init];
	d->timer = new QTimer();
	connect(d->timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
	d->timer->start(1000);
	SettingsItem* settings = new GeneralSettingsItem<MacIdleWidget>(
				Settings::General,
				Icon("user-away-extended"),
				QT_TRANSLATE_NOOP("AutoAway", "Auto-away"));
	Settings::registerItem(settings);
	pIdleStatusChanger = this;
}

MacIdle::~MacIdle()
{
}

void MacIdle::onTimeout()
{
	Q_D(MacIdle);
	if (d->currentReason == Screensaver)
		return;
	CFTimeInterval currentIdle = [d->idleHandler currentIdle];
	if (currentIdle > d->inactiveInterval) {
		if (d->isInactiveEnabled)
			setIdleOn(Inactive);
	} else if (currentIdle > d->awayInterval) {
		if (d->isAwayEnabled)
			setIdleOn(Away);
	} else if (d->currentReason != NoIdle) {
		setIdleOff();
	}
}

void MacIdle::setIdleOn(MacIdle::Reason reason)
{
	Q_D(MacIdle);
	Status::Type statusType;
	if (reason == Inactive || reason == Screensaver)
		statusType = Status::NA;
	else
		statusType = Status::Away;
	foreach(qutim_sdk_0_3::Protocol *proto, qutim_sdk_0_3::Protocol::all()) {
		foreach(Account *account, proto->accounts()) {
			Status status = account->status();
			if (status.type() != Status::Online && status.type() != Status::FreeChat
					&& !(status.type() == Status::Away && statusType == Status::NA)
					&& !(status.type() == Status::DND && reason == Screensaver))
				continue;
			if (!d->idleAccounts.keys().contains(account))
				d->idleAccounts.insert(account, status);
			status.setType(statusType);
			status.setSubtype(0);
			status.setText(d->idleMessages.value(statusType, ""));
			status.setProperty("changeReason", Status::ByIdle);
			account->setUserStatus(status);
		}
	}
	d->currentReason = reason;
}

void MacIdle::setIdleOff()
{
	Q_D(MacIdle);
	foreach(qutim_sdk_0_3::Protocol *proto, qutim_sdk_0_3::Protocol::all()) {
		foreach(Account *account, proto->accounts()) {
			if (d->idleAccounts.contains(account)) {
				account->setUserStatus(d->idleAccounts.take(account));
			}
		}
	}
	d->currentReason = NoIdle;
}

void MacIdle::reloadSettings()
{
	Q_D(MacIdle);
	Config conf(AA_CONFIG_GROUP);
	d->isAwayEnabled = conf.value("away-enabled", true);
	d->isInactiveEnabled   = conf.value("na-enabled",   true);
	d->awayInterval = conf.value("away-secs", AWAY_DEF_SECS);
	d->inactiveInterval = conf.value("na-secs",   NA_DEF_SECS);
	d->idleMessages.insert(Status::Away, conf.value("away-text", ""));
	d->idleMessages.insert(Status::NA, conf.value("na-text", ""));
}
}
