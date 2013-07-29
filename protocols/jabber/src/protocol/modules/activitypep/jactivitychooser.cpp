/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "jactivitychooser.h"
#include "ui_jactivitychooser.h"
#include "jpersonactivityconverter.h"
#include <qutim/icon.h>
#include <qutim/event.h>
#include <qutim/actiongenerator.h>
#include "../../account/jaccount.h"
#include <jreen/activity.h>
#include <qutim/debug.h>

namespace Jabber {

struct ActivityGroup
{
	ActivityGroup(Jreen::Activity::General general_) :
		general(general_)
	{}
	Jreen::Activity::General general;
	QList<Jreen::Activity::Specific> items;
};
typedef QList<ActivityGroup> ActivityGroups;

static ActivityGroups init_activity_groups()
{
    ActivityGroups activities;
	{
		ActivityGroup group(Jreen::Activity::DoingChores);
		group.items.push_back(Jreen::Activity::BuyingGroceries);
		group.items.push_back(Jreen::Activity::Cleaning);
		group.items.push_back(Jreen::Activity::Cooking);
		group.items.push_back(Jreen::Activity::DoingMaintenance);
		group.items.push_back(Jreen::Activity::DoingTheDishes);
		group.items.push_back(Jreen::Activity::DoingTheLaundry);
		group.items.push_back(Jreen::Activity::Gardening);
		group.items.push_back(Jreen::Activity::RunningAnErrand);
		group.items.push_back(Jreen::Activity::WalkingTheDog);
		activities.push_back(group);
	}
	{
		ActivityGroup group(Jreen::Activity::Drinking);
		group.items.push_back(Jreen::Activity::HavingABeer);
		group.items.push_back(Jreen::Activity::HavingCoffee);
		group.items.push_back(Jreen::Activity::HavingTea);
		activities.push_back(group);
	}
	{
		ActivityGroup group(Jreen::Activity::Eating);
		group.items.push_back(Jreen::Activity::HavingASnack);
		group.items.push_back(Jreen::Activity::HavingBreakfast);
		group.items.push_back(Jreen::Activity::HavingDinner);
		group.items.push_back(Jreen::Activity::HavingLunch);
		activities.push_back(group);
	}
	{
		ActivityGroup group(Jreen::Activity::Exercising);
		group.items.push_back(Jreen::Activity::Cycling);
		group.items.push_back(Jreen::Activity::Dancing);
		group.items.push_back(Jreen::Activity::Hiking);
		group.items.push_back(Jreen::Activity::Jogging);
		group.items.push_back(Jreen::Activity::PlayingSports);
		group.items.push_back(Jreen::Activity::Running);
		group.items.push_back(Jreen::Activity::Skiing);
		group.items.push_back(Jreen::Activity::Swimming);
		group.items.push_back(Jreen::Activity::WorkingOut);
		activities.push_back(group);
	}
	{
		ActivityGroup group(Jreen::Activity::Grooming);
		group.items.push_back(Jreen::Activity::AtTheSpa);
		group.items.push_back(Jreen::Activity::BrushingTeeth);
		group.items.push_back(Jreen::Activity::GettingAHaircut);
		group.items.push_back(Jreen::Activity::Shaving);
		group.items.push_back(Jreen::Activity::TakingABath);
		group.items.push_back(Jreen::Activity::TakingAShower);
		activities.push_back(group);
	}
	{
		ActivityGroup group(Jreen::Activity::HavingAppointment);
		activities.push_back(group);
	}
	{
		ActivityGroup group(Jreen::Activity::Inactive);
		group.items.push_back(Jreen::Activity::DayOff);
		group.items.push_back(Jreen::Activity::HangingOut);
		group.items.push_back(Jreen::Activity::Hiding);
		group.items.push_back(Jreen::Activity::OnVacation);
		group.items.push_back(Jreen::Activity::Praying);
		group.items.push_back(Jreen::Activity::ScheduledHoliday);
		group.items.push_back(Jreen::Activity::Sleeping);
		group.items.push_back(Jreen::Activity::Thinking);
		activities.push_back(group);
	}
	{
		ActivityGroup group(Jreen::Activity::Relaxing);
		group.items.push_back(Jreen::Activity::Fishing);
		group.items.push_back(Jreen::Activity::Gaming);
		group.items.push_back(Jreen::Activity::GoingOut);
		group.items.push_back(Jreen::Activity::Partying);
		group.items.push_back(Jreen::Activity::Reading);
		group.items.push_back(Jreen::Activity::Rehearsing);
		group.items.push_back(Jreen::Activity::Shopping);
		group.items.push_back(Jreen::Activity::Smoking);
		group.items.push_back(Jreen::Activity::Socializing);
		group.items.push_back(Jreen::Activity::Sunbathing);
		group.items.push_back(Jreen::Activity::WatchingTv);
		group.items.push_back(Jreen::Activity::WatchingAMovie);
		activities.push_back(group);
	}
	{
		ActivityGroup group(Jreen::Activity::Talking);
		group.items.push_back(Jreen::Activity::InRealLife);
		group.items.push_back(Jreen::Activity::OnThePhone);
		group.items.push_back(Jreen::Activity::OnVideoPhone);
		activities.push_back(group);
	}
	{
		ActivityGroup group(Jreen::Activity::Traveling);
		group.items.push_back(Jreen::Activity::Commuting);
		group.items.push_back(Jreen::Activity::Cycling);
		group.items.push_back(Jreen::Activity::Driving);
		group.items.push_back(Jreen::Activity::InACar);
		group.items.push_back(Jreen::Activity::OnABus);
		group.items.push_back(Jreen::Activity::OnAPlane);
		group.items.push_back(Jreen::Activity::OnATrain);
		group.items.push_back(Jreen::Activity::OnATrip);
		group.items.push_back(Jreen::Activity::Walking);
		activities.push_back(group);
	}
	{
		ActivityGroup group(Jreen::Activity::Working);
		group.items.push_back(Jreen::Activity::Coding);
		group.items.push_back(Jreen::Activity::InAMeeting);
		group.items.push_back(Jreen::Activity::Studying);
		group.items.push_back(Jreen::Activity::Writing);
		activities.push_back(group);
	}
    return activities;
}

Q_GLOBAL_STATIC_WITH_ARGS(ActivityGroups, allActivityGroups, (init_activity_groups()))

JActivityChooserWindow::JActivityChooserWindow(Account *account,
											   const QString &currentText,
											   const QString &currentGeneral,
											   const QString &currentSpecific,
											   QWidget *parent) :
	QDialog(parent),
	ui(new Ui::JActivityChooserWindow),
	m_account(account)
{
	// Init dialog
	ui->setupUi(this);
	setWindowTitle(tr("Choose your activity"));
	ui->buttonBox->addButton(tr("Choose"), QDialogButtonBox::AcceptRole);
	connect(ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(ui->buttonBox, SIGNAL(rejected()), SLOT(reject()));
	connect(this, SIGNAL(accepted()), SLOT(sendActivity()));
	connect(ui->activitiesWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
			SLOT(onCurrentItemChanged(QTreeWidgetItem *)));
	ui->textEdit->setPlainText(currentText);

	// Load available activities
	QTreeWidgetItem *current = 0;
	foreach (const ActivityGroup &activity, *allActivityGroups()) {
		QString generalName = Jreen::Activity::generalName(activity.general);
		QString generalIconName = QLatin1String("user-status-") + generalName;
		QTreeWidgetItem *generalItem = new QTreeWidgetItem(ui->activitiesWidget);
		generalItem->setData(0, Qt::DisplayRole, JPersonActivityConverter::generalTitle(activity.general).toString());
		generalItem->setData(0, Qt::DecorationRole, Icon(generalIconName + QLatin1String("-jabber")));
		generalItem->setData(0, Qt::UserRole+1, generalName);
		bool isGeneralCurrent = (!current && generalName == currentGeneral);

		foreach (Jreen::Activity::Specific specific, activity.items) {
			QString specificName = Jreen::Activity::specificName(specific);
			QString iconName = generalIconName + QLatin1Char('-') + specificName + QLatin1String("-jabber");
			QTreeWidgetItem *item = new QTreeWidgetItem();
			item->setData(0, Qt::DisplayRole, JPersonActivityConverter::specificTitle(specific).toString());
			item->setData(0, Qt::DecorationRole, Icon(iconName));
			item->setData(0, Qt::UserRole+1, generalName);
			item->setData(0, Qt::UserRole+2, specificName);
			generalItem->addChild(item);
			if (!current && isGeneralCurrent && specificName == currentSpecific)
				current = item;
		}

		if (isGeneralCurrent && !current)
			current = generalItem;
	}

	ui->activitiesWidget->sortByColumn(0, Qt::AscendingOrder);
	m_noActivityItem = new QTreeWidgetItem();
	m_noActivityItem->setData(0, Qt::DisplayRole, tr("No activity"));
	m_noActivityItem->setData(0, Qt::DecorationRole, Icon("edit-delete-status"));
	ui->activitiesWidget->insertTopLevelItem(0, m_noActivityItem);
	ui->activitiesWidget->setCurrentItem(current ? current : m_noActivityItem);
}

JActivityChooserWindow::~JActivityChooserWindow()
{
	delete ui;
}

void JActivityChooserWindow::sendActivity()
{
	QVariantHash data;
	QString general = ui->activitiesWidget->currentItem()->data(0, Qt::UserRole + 1).toString();
	if (!general.isEmpty()) {
		data.insert("general", general);
		data.insert("specific", ui->activitiesWidget->currentItem()->data(0, Qt::UserRole + 2).toString());
		data.insert("description", ui->textEdit->toPlainText());
	}
	qutim_sdk_0_3::Event ev("jabber-personal-event", "activity", data, true);
	qApp->sendEvent(m_account, &ev);
}

void JActivityChooserWindow::onCurrentItemChanged(QTreeWidgetItem *item)
{
	ui->textEdit->setEnabled(item != m_noActivityItem);
}

JActivityChooser::JActivityChooser()
{
}

void JActivityChooser::init(qutim_sdk_0_3::Account *account)
{
	m_account = account;
	m_eventId = qutim_sdk_0_3::Event::registerType("jabber-personal-event");
	// Add action to context menu
	m_action.reset(new JActivityChooserAction(QIcon(), tr("Set activity"), this, SLOT(show(QObject*))));
	m_action->setType(0x60000);
	m_action->setPriority(30);
	account->addAction(m_action.data());
	// Register event filter
	account->installEventFilter(this);
}

void JActivityChooser::show(QObject *obj)
{
	Q_ASSERT(qobject_cast<Account*>(obj));
	Account *account = reinterpret_cast<Account*>(obj);
	JActivityChooserWindow *w = new JActivityChooserWindow(account, m_currentText,
														   m_currentGeneral, m_currentSpecific);
#ifdef QUTIM_MOBILE_UI
	w->showMaximized();
#else
	w->show();
#endif
	w->setAttribute(Qt::WA_DeleteOnClose, true);
}

bool JActivityChooser::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == qutim_sdk_0_3::Event::eventType()) {
		qutim_sdk_0_3::Event *customEvent = static_cast<qutim_sdk_0_3::Event*>(ev);
		if (customEvent->id == m_eventId && obj == m_account &&
				customEvent->at<QString>(0) == "activity")
		{
			QVariantHash data = customEvent->at<QVariantHash>(1);
			m_currentGeneral = data.value("general").toString();
			m_currentSpecific = data.value("specific").toString();
			m_currentText = data.value("description").toString();
		}
	}
	return false;
}

JActivityChooserAction::JActivityChooserAction(const QIcon &icon, const LocalizedString &text,
											   const QObject *receiver, const char *member) :
	ActionGenerator(icon, text, receiver, member)
{
}

JActivityChooserAction::JActivityChooserAction(const QIcon &icon, const LocalizedString &text,
											   const char *member) :
	ActionGenerator(icon, text, member)
{
}

void JActivityChooserAction::showImpl(QAction *action, QObject *obj)
{
	JAccount *account = qobject_cast<JAccount*>(obj);
	if (!account)
		return;
	action->setEnabled(account->checkIdentity(QLatin1String("pubsub"), QLatin1String("pep")));
}

} // namespace Jabber

