#include "jactivitychooser.h"
#include "ui_jactivitychooser.h"
#include "jpersonactivityconverter.h"
#include <qutim/icon.h>
#include <qutim/event.h>
#include <qutim/actiongenerator.h>
#include "protocol/account/jaccount.h"
#include "jreen/activity.h"
#include <qutim/debug.h>

namespace Jabber {

struct ActivityGroup
{
	ActivityGroup(jreen::Activity::General general_) :
		general(general_)
	{}
	jreen::Activity::General general;
	QList<jreen::Activity::Specific> items;
};
typedef QList<ActivityGroup> ActivityGroups;

static void init_activity_groups(ActivityGroups &activities)
{
	{
		ActivityGroup group(jreen::Activity::DoingChores);
		group.items.push_back(jreen::Activity::BuyingGroceries);
		group.items.push_back(jreen::Activity::Cleaning);
		group.items.push_back(jreen::Activity::Cooking);
		group.items.push_back(jreen::Activity::DoingMaintenance);
		group.items.push_back(jreen::Activity::DoingTheDishes);
		group.items.push_back(jreen::Activity::DoingTheLaundry);
		group.items.push_back(jreen::Activity::Gardening);
		group.items.push_back(jreen::Activity::RunningAnErrand);
		group.items.push_back(jreen::Activity::WalkingTheDog);
		activities.push_back(group);
	}
	{
		ActivityGroup group(jreen::Activity::Drinking);
		group.items.push_back(jreen::Activity::HavingABeer);
		group.items.push_back(jreen::Activity::HavingCoffee);
		group.items.push_back(jreen::Activity::HavingTea);
		activities.push_back(group);
	}
	{
		ActivityGroup group(jreen::Activity::Eating);
		group.items.push_back(jreen::Activity::HavingASnack);
		group.items.push_back(jreen::Activity::HavingBreakfast);
		group.items.push_back(jreen::Activity::HavingDinner);
		group.items.push_back(jreen::Activity::HavingLunch);
		activities.push_back(group);
	}
	{
		ActivityGroup group(jreen::Activity::Exercising);
		group.items.push_back(jreen::Activity::Cycling);
		group.items.push_back(jreen::Activity::Dancing);
		group.items.push_back(jreen::Activity::Hiking);
		group.items.push_back(jreen::Activity::Jogging);
		group.items.push_back(jreen::Activity::PlayingSports);
		group.items.push_back(jreen::Activity::Running);
		group.items.push_back(jreen::Activity::Skiing);
		group.items.push_back(jreen::Activity::Swimming);
		group.items.push_back(jreen::Activity::WorkingOut);
		activities.push_back(group);
	}
	{
		ActivityGroup group(jreen::Activity::Grooming);
		group.items.push_back(jreen::Activity::AtTheSpa);
		group.items.push_back(jreen::Activity::BrushingTeeth);
		group.items.push_back(jreen::Activity::GettingAHaircut);
		group.items.push_back(jreen::Activity::Shaving);
		group.items.push_back(jreen::Activity::TakingABath);
		group.items.push_back(jreen::Activity::TakingAShower);
		activities.push_back(group);
	}
	{
		ActivityGroup group(jreen::Activity::HavingAppointment);
		activities.push_back(group);
	}
	{
		ActivityGroup group(jreen::Activity::Inactive);
		group.items.push_back(jreen::Activity::DayOff);
		group.items.push_back(jreen::Activity::HangingOut);
		group.items.push_back(jreen::Activity::Hiding);
		group.items.push_back(jreen::Activity::OnVacation);
		group.items.push_back(jreen::Activity::Praying);
		group.items.push_back(jreen::Activity::ScheduledHoliday);
		group.items.push_back(jreen::Activity::Sleeping);
		group.items.push_back(jreen::Activity::Thinking);
		activities.push_back(group);
	}
	{
		ActivityGroup group(jreen::Activity::Relaxing);
		group.items.push_back(jreen::Activity::Fishing);
		group.items.push_back(jreen::Activity::Gaming);
		group.items.push_back(jreen::Activity::GoingOut);
		group.items.push_back(jreen::Activity::Partying);
		group.items.push_back(jreen::Activity::Reading);
		group.items.push_back(jreen::Activity::Rehearsing);
		group.items.push_back(jreen::Activity::Shopping);
		group.items.push_back(jreen::Activity::Smoking);
		group.items.push_back(jreen::Activity::Socializing);
		group.items.push_back(jreen::Activity::Sunbathing);
		group.items.push_back(jreen::Activity::WatchingTv);
		group.items.push_back(jreen::Activity::WatchingAMovie);
		activities.push_back(group);
	}
	{
		ActivityGroup group(jreen::Activity::Talking);
		group.items.push_back(jreen::Activity::InRealLife);
		group.items.push_back(jreen::Activity::OnThePhone);
		group.items.push_back(jreen::Activity::OnVideoPhone);
		activities.push_back(group);
	}
	{
		ActivityGroup group(jreen::Activity::Traveling);
		group.items.push_back(jreen::Activity::Commuting);
		group.items.push_back(jreen::Activity::Cycling);
		group.items.push_back(jreen::Activity::Driving);
		group.items.push_back(jreen::Activity::InACar);
		group.items.push_back(jreen::Activity::OnABus);
		group.items.push_back(jreen::Activity::OnAPlane);
		group.items.push_back(jreen::Activity::OnATrain);
		group.items.push_back(jreen::Activity::OnATrip);
		group.items.push_back(jreen::Activity::Walking);
		activities.push_back(group);
	}
	{
		ActivityGroup group(jreen::Activity::Working);
		group.items.push_back(jreen::Activity::Coding);
		group.items.push_back(jreen::Activity::InAMeeting);
		group.items.push_back(jreen::Activity::Studying);
		group.items.push_back(jreen::Activity::Writing);
		activities.push_back(group);
	}
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(ActivityGroups, allActivityGroups, init_activity_groups(*x));

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
	ui->buttonBox->addButton("Choose", QDialogButtonBox::AcceptRole);
	connect(ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(ui->buttonBox, SIGNAL(rejected()), SLOT(reject()));
	connect(this, SIGNAL(accepted()), SLOT(sendActivity()));
	connect(ui->activitiesWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
			SLOT(onCurrentItemChanged(QTreeWidgetItem *)));
	ui->textEdit->setPlainText(currentText);

	// Load available activities
	QTreeWidgetItem *current = 0;
	foreach (const ActivityGroup &activity, *allActivityGroups()) {
		QString generalName = jreen::Activity::generalName(activity.general);
		QString generalIconName = QLatin1String("user-status-") + generalName;
		QTreeWidgetItem *generalItem = new QTreeWidgetItem(ui->activitiesWidget);
		generalItem->setData(0, Qt::DisplayRole, JPersonActivityConverter::generalTitle(activity.general).toString());
		generalItem->setData(0, Qt::DecorationRole, Icon(generalIconName + QLatin1String("-jabber")));
		generalItem->setData(0, Qt::UserRole+1, generalName);
		bool isGeneralCurrent = current == 0 && generalName == currentGeneral;

		foreach (jreen::Activity::Specific specific, activity.items) {
			QString specificName = jreen::Activity::specificName(specific);
			QString iconName = generalIconName + QLatin1Char('-') + specificName + QLatin1String("-jabber");
			QTreeWidgetItem *item = new QTreeWidgetItem();
			item->setData(0, Qt::DisplayRole, JPersonActivityConverter::specificTitle(specific).toString());
			item->setData(0, Qt::DecorationRole, Icon(iconName));
			item->setData(0, Qt::UserRole+1, generalName);
			item->setData(0, Qt::UserRole+2, specificName);
			generalItem->addChild(item);
			if (current == 0 && isGeneralCurrent && specificName == currentSpecific)
				current = item;
		}

		if (isGeneralCurrent && current == 0)
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

void JActivityChooser::init(qutim_sdk_0_3::Account *account, const JabberParams &params)
{
	Q_UNUSED(params);
	m_account = account;
	m_eventId = qutim_sdk_0_3::Event::registerType("jabber-personal-event");
	// Add action to context menu
	static JActivityChooserAction gen(QIcon(), tr("Set activity"), this, SLOT(show(QObject*)));
	gen.setType(0x60000);
	gen.setPriority(30);
	account->addAction(&gen);
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
