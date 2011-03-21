#include "midle.h"
#include "midle-global.h"
#include "midlewidget.h"
#include <qutim/account.h>
#include <qutim/status.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <QTimer>

@interface IdleHandler : NSObject {
	@public
		MacIntegration::MIdle *idleManager;
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
	idleManager->setIdleOn(MacIntegration::MIdle::Screensaver);
}

- (void)screenSaverStopped
{
	idleManager->setIdleOff();
}
@end

MacIntegration::MIdle* pIdleStatusChanger = 0;

namespace MacIntegration
{
	struct MIdlePrivate
	{ 
		IdleHandler *idleHandler;
		QTimer *timer;
		bool isAwayEnabled;
		int awayInterval;
		bool isInactiveEnabled;
		int inactiveInterval;
		MIdle::Reason currentReason;
		QHash<Account *, Status> idleAccounts;
		QHash<Status::Type, QString> idleMessages;
	};

	MIdle::MIdle() : d_ptr(new MIdlePrivate())
	{
		Q_D(MIdle);
		reloadSettings();
		d->idleHandler = [[IdleHandler alloc] init];
		d->timer = new QTimer();
		connect(d->timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
		d->timer->start(1000);
		SettingsItem* settings = new GeneralSettingsItem<MIdleWidget>(
				Settings::General,
				Icon("user-away-extended"),
				QT_TRANSLATE_NOOP("AutoAway", "Auto-away"));
		Settings::registerItem(settings);
		pIdleStatusChanger = this;
	}  

	MIdle::~MIdle()
	{
	}

	void MIdle::onTimeout()
	{
		Q_D(MIdle);
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

	void MIdle::setIdleOn(MIdle::Reason reason)
	{
		Q_D(MIdle);
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
				account->setStatus(status);
			}
		}
		d->currentReason = reason;
	}
 
	void MIdle::setIdleOff()
 	{
		Q_D(MIdle);
		foreach(qutim_sdk_0_3::Protocol *proto, qutim_sdk_0_3::Protocol::all()) {
			foreach(Account *account, proto->accounts()) {
				if (d->idleAccounts.contains(account)) {
					account->setStatus(d->idleAccounts.take(account));
				}
			}
		}
		d->currentReason = NoIdle;
	}

	void MIdle::reloadSettings()
	{
		Q_D(MIdle);
		Config conf(AA_CONFIG_GROUP);
		d->isAwayEnabled = conf.value("away-enabled", true);
		d->isInactiveEnabled   = conf.value("na-enabled",   true);
		d->awayInterval = conf.value("away-secs", AWAY_DEF_SECS);
		d->inactiveInterval = conf.value("na-secs",   NA_DEF_SECS);
		d->idleMessages.insert(Status::Away, conf.value("away-text", ""));
		d->idleMessages.insert(Status::NA, conf.value("na-text", ""));
	}
}
