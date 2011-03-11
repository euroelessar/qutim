#include "jpersonactivityconverter.h"
#include <protocol/jprotocol.h>
#include <qutim/iconloader.h>

namespace Jabber {

typedef QList<qutim_sdk_0_3::LocalizedString> ActivityNames;

static void init_general_names(ActivityNames &names)
{
	names.append(QT_TRANSLATE_NOOP("Activity", "Doing chores"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Drinking"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Eating"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Exercising"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Grooming"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Having appointment"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Inactive"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Relaxing"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Talking"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Traveling"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Undefined"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Working"));
}

static void init_specific_names(ActivityNames &names)
{
	names.append(QT_TRANSLATE_NOOP("Activity", "At the spa"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Brushing teeth"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Buying groceries"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Cleaning"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Coding"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Commuting"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Cooking"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Cycling"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Dancing"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Day off"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Doing maintenance"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Doing the dishes"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Doing the laundry"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Driving"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Fishing"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Gaming"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Gardening"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Getting a haircut"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Going out"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Hanging out"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Having a beer"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Having a snack"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Having breakfast"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Having coffee"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Having dinner"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Having lunch"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Having tea"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Hiding"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Hiking"));
	names.append(QT_TRANSLATE_NOOP("Activity", "In a car"));
	names.append(QT_TRANSLATE_NOOP("Activity", "In a meeting"));
	names.append(QT_TRANSLATE_NOOP("Activity", "In real life"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Jogging"));
	names.append(QT_TRANSLATE_NOOP("Activity", "On a bus"));
	names.append(QT_TRANSLATE_NOOP("Activity", "On a plane"));
	names.append(QT_TRANSLATE_NOOP("Activity", "On a train"));
	names.append(QT_TRANSLATE_NOOP("Activity", "On a trip"));
	names.append(QT_TRANSLATE_NOOP("Activity", "On the phone"));
	names.append(QT_TRANSLATE_NOOP("Activity", "On vacation"));
	names.append(QT_TRANSLATE_NOOP("Activity", "On video phone"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Other"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Partying"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Playing sports"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Praying"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Reading"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Rehearsing"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Running"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Running an errand"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Scheduled holiday"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Shaving"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Shopping"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Skiing"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Sleeping"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Smoking"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Socializing"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Studying"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Sunbathing"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Swimming"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Taking a bath"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Taking a shower"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Thinking"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Walking"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Walking the dog"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Watching a movie"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Watching TV"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Working out"));
	names.append(QT_TRANSLATE_NOOP("Activity", "Writing"));
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(ActivityNames, generalNames, init_general_names(*x));
Q_GLOBAL_STATIC_WITH_INITIALIZER(ActivityNames, specificNames, init_specific_names(*x));
Q_GLOBAL_STATIC(JPersonActivityRegistrator, activityRegistrator);

JPersonActivityConverter::JPersonActivityConverter()
{
	activityRegistrator();
}

JPersonActivityConverter::~JPersonActivityConverter()
{
}

QString JPersonActivityConverter::name() const
{
	return QLatin1String("activity");
}

int JPersonActivityConverter::entityType() const
{
	return Jreen::Activity::staticExtensionType();
}

QSharedPointer<Jreen::StanzaExtension> JPersonActivityConverter::convertTo(const QVariantHash &map) const
{
	QString general = map.value(QLatin1String("general")).toString();
	QString specific = map.value(QLatin1String("specific")).toString();
	QString text = map.value(QLatin1String("description")).toString();
	return Jreen::StanzaExtension::Ptr(new Jreen::Activity(general, specific, text));
}

QVariantHash JPersonActivityConverter::convertFrom(const QSharedPointer<Jreen::StanzaExtension> &entity) const
{
	Jreen::Activity *activity = Jreen::se_cast<Jreen::Activity*>(entity.data());
	QVariantHash data;
	QString title;
	QString iconName;
	data.insert(QLatin1String("id"), QLatin1String("activity"));
	if (activity->general() <= Jreen::Activity::InvalidGeneral)
		return data;
	data.insert(QLatin1String("general"), activity->generalName());
	iconName = QLatin1String("user-status-") + activity->generalName();
	if (activity->specific() <= Jreen::Activity::InvalidSpecific) {
		title = generalNames()->value(activity->general());
	} else {
		data.insert(QLatin1String("specific"), activity->specificName());
		title = specificNames()->value(activity->specific());
		iconName += QLatin1Char('-') + activity->specificName();
	}
	iconName += QLatin1String("-jabber");
	qutim_sdk_0_3::ExtensionIcon icon(iconName);
	data.insert(QLatin1String("icon"), qVariantFromValue(icon));
	data.insert(QLatin1String("title"), title);
	if (!activity->text().isEmpty())
		data.insert(QLatin1String("description"), activity->text());
	data.insert(QLatin1String("showInTooltip"), true);
	data.insert(QLatin1String("priorityInTooltip"), 70);
	return data;
}

qutim_sdk_0_3::LocalizedString JPersonActivityConverter::generalTitle(Jreen::Activity::General general)
{
	return generalNames()->value(general);
}

qutim_sdk_0_3::LocalizedString JPersonActivityConverter::specificTitle(Jreen::Activity::Specific specific)
{
	return specificNames()->value(specific);
}

JPersonActivityRegistrator::JPersonActivityRegistrator()
{
	JProtocol::instance()->installEventFilter(this);
}

bool JPersonActivityRegistrator::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == ExtendedInfosEvent::eventType() && obj == JProtocol::instance()) {
		ExtendedInfosEvent *event = static_cast<ExtendedInfosEvent*>(ev);
		QVariantHash extStatus;
		extStatus.insert("id", "activity");
		extStatus.insert("name", QT_TRANSLATE_NOOP("Activity", "Activity").toString());
		extStatus.insert("settingsDescription",
						 QT_TRANSLATE_NOOP("Activity", "Show contact activity icon").toString());
		event->addInfo("activity", extStatus);
	}
	return false;
}

} // namespace Jabber
