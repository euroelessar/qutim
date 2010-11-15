#include "jpersonactivityconverter.h"
#include <gloox/tag.h>
#include <protocol/jprotocol.h>
#include <qutim/iconloader.h>

namespace Jabber {

// Adds a new activity
#define ADD_ACTIVITY(Name, Value, General) \
	activities.insert(Name, JPersonActivity( Name , Value, General ))
// Adds a new general activity
#define ADD_GENERAL(Name, Value)\
	ADD_ACTIVITY (Name, Value, QString())
// Adds a new specific activity
#define ADD_SPECIFIC(Name, Value)\
	ADD_ACTIVITY (Name, Value, general)

	typedef QMultiHash<QString, JPersonActivity> Activities;
	static void init_activities(Activities &activities)
	{
		{
			QString general = "doing_chores";
			ADD_GENERAL(general, QT_TRANSLATE_NOOP("Activity", "Doing chores"));
			ADD_SPECIFIC("buying_groceries", QT_TRANSLATE_NOOP("Activity", "Buying groceries"));
			ADD_SPECIFIC("cleaning", QT_TRANSLATE_NOOP("Activity", "Cleaning"));
			ADD_SPECIFIC("cooking", QT_TRANSLATE_NOOP("Activity", "Cooking"));
			ADD_SPECIFIC("doing_maintenance", QT_TRANSLATE_NOOP("Activity", "Doing maintenance"));
			ADD_SPECIFIC("doing_the_dishes", QT_TRANSLATE_NOOP("Activity", "Doing the dishes"));
			ADD_SPECIFIC("doing_the_laundry", QT_TRANSLATE_NOOP("Activity", "Doing the laundry"));
			ADD_SPECIFIC("gardening", QT_TRANSLATE_NOOP("Activity", "Gardening"));
			ADD_SPECIFIC("running_an_errand", QT_TRANSLATE_NOOP("Activity", "Running an errand"));
			ADD_SPECIFIC("walking_the_dog", QT_TRANSLATE_NOOP("Activity", "Walking the dog"));
		}
		{
			QString general = "drinking";
			ADD_GENERAL(general, QT_TRANSLATE_NOOP("Activity", "Drinking"));
			ADD_SPECIFIC("having_a_beer", QT_TRANSLATE_NOOP("Activity", "Having a beer"));
			ADD_SPECIFIC("having_coffee", QT_TRANSLATE_NOOP("Activity", "Having coffee"));
			ADD_SPECIFIC("having_tea", QT_TRANSLATE_NOOP("Activity", "Having tea"));
		}
		{
			QString general = "eating";
			ADD_GENERAL(general, QT_TRANSLATE_NOOP("Activity", "Eeating"));
			ADD_SPECIFIC("having_a_snack", QT_TRANSLATE_NOOP("Activity", "Having a snack"));
			ADD_SPECIFIC("having_breakfast", QT_TRANSLATE_NOOP("Activity", "Having breakfast"));
			ADD_SPECIFIC("having_dinner", QT_TRANSLATE_NOOP("Activity", "Having dinner"));
			ADD_SPECIFIC("having_lunch", QT_TRANSLATE_NOOP("Activity", "Having lunch"));
		}
		{
			QString general = "exercising";
			ADD_GENERAL(general, QT_TRANSLATE_NOOP("Activity", "Exercising"));
			ADD_SPECIFIC("cycling", QT_TRANSLATE_NOOP("Activity", "Cycling"));
			ADD_SPECIFIC("dancing", QT_TRANSLATE_NOOP("Activity", "Dancing"));
			ADD_SPECIFIC("hiking", QT_TRANSLATE_NOOP("Activity", "Hiking"));
			ADD_SPECIFIC("jogging", QT_TRANSLATE_NOOP("Activity", "Jogging"));
			ADD_SPECIFIC("playing_sports", QT_TRANSLATE_NOOP("Activity", "Playing sports"));
			ADD_SPECIFIC("running", QT_TRANSLATE_NOOP("Activity", "Running"));
			ADD_SPECIFIC("skiing", QT_TRANSLATE_NOOP("Activity", "Skiing"));
			ADD_SPECIFIC("swimming", QT_TRANSLATE_NOOP("Activity", "Swimming"));
			ADD_SPECIFIC("working_out", QT_TRANSLATE_NOOP("Activity", "Working out"));
		}
		{
			QString general = "grooming";
			ADD_GENERAL(general, QT_TRANSLATE_NOOP("Activity", "Grooming"));
			ADD_SPECIFIC("at_the_spa", QT_TRANSLATE_NOOP("Activity", "At the spa"));
			ADD_SPECIFIC("brushing_teeth", QT_TRANSLATE_NOOP("Activity", "Brushing teeth"));
			ADD_SPECIFIC("getting_a_haircut", QT_TRANSLATE_NOOP("Activity", "Getting a haircut"));
			ADD_SPECIFIC("shaving", QT_TRANSLATE_NOOP("Activity", "Shaving"));
			ADD_SPECIFIC("taking_a_bath", QT_TRANSLATE_NOOP("Activity", "Taking a bath"));
			ADD_SPECIFIC("taking_a_shower", QT_TRANSLATE_NOOP("Activity", "Taking a shower"));
		}
		{
			ADD_GENERAL("having_appointment", QT_TRANSLATE_NOOP("Activity", "Having appointment"));
		}
		{
			QString general = "inactive";
			ADD_GENERAL(general, QT_TRANSLATE_NOOP("Activity", "Inactive"));
			ADD_SPECIFIC("day_off", QT_TRANSLATE_NOOP("Activity", "Day off"));
			ADD_SPECIFIC("hanging_out", QT_TRANSLATE_NOOP("Activity", "Hanging out"));
			ADD_SPECIFIC("hiding", QT_TRANSLATE_NOOP("Activity", "Hiding"));
			ADD_SPECIFIC("on_vacation", QT_TRANSLATE_NOOP("Activity", "On vacation"));
			ADD_SPECIFIC("praying", QT_TRANSLATE_NOOP("Activity", "Praying"));
			ADD_SPECIFIC("scheduled_holiday", QT_TRANSLATE_NOOP("Activity", "Scheduled holiday"));
			ADD_SPECIFIC("sleeping", QT_TRANSLATE_NOOP("Activity", "Sleeping"));
			ADD_SPECIFIC("thinking", QT_TRANSLATE_NOOP("Activity", "Thinking"));
		}
		{
			QString general = "relaxing";
			ADD_GENERAL(general, QT_TRANSLATE_NOOP("Activity", "Relaxing"));
			ADD_SPECIFIC("fishing", QT_TRANSLATE_NOOP("Activity", "Fishing"));
			ADD_SPECIFIC("gaming", QT_TRANSLATE_NOOP("Activity", "Gaming"));
			ADD_SPECIFIC("going_out", QT_TRANSLATE_NOOP("Activity", "Going out"));
			ADD_SPECIFIC("partying", QT_TRANSLATE_NOOP("Activity", "Partying"));
			ADD_SPECIFIC("reading", QT_TRANSLATE_NOOP("Activity", "Reading"));
			ADD_SPECIFIC("rehearsing", QT_TRANSLATE_NOOP("Activity", "Rehearsing"));
			ADD_SPECIFIC("shopping", QT_TRANSLATE_NOOP("Activity", "Shopping"));
			ADD_SPECIFIC("smoking", QT_TRANSLATE_NOOP("Activity", "Smoking"));
			ADD_SPECIFIC("socializing", QT_TRANSLATE_NOOP("Activity", "Socializing"));
			ADD_SPECIFIC("sunbathing", QT_TRANSLATE_NOOP("Activity", "Sunbathing"));
			ADD_SPECIFIC("watching_tv", QT_TRANSLATE_NOOP("Activity", "Watching tv"));
			ADD_SPECIFIC("watching_a_movie", QT_TRANSLATE_NOOP("Activity", "Watching a movie"));
		}
		{
			QString general = "talking";
			ADD_GENERAL(general, QT_TRANSLATE_NOOP("Activity", "Talking"));
			ADD_SPECIFIC("in_real_life", QT_TRANSLATE_NOOP("Activity", "In real life"));
			ADD_SPECIFIC("on_the_phone", QT_TRANSLATE_NOOP("Activity", "On the phone"));
			ADD_SPECIFIC("on_video_phone", QT_TRANSLATE_NOOP("Activity", "On video phone"));
		}
		{
			QString general = "traveling";
			ADD_GENERAL(general, QT_TRANSLATE_NOOP("Activity", "Traveling"));
			ADD_SPECIFIC("commuting", QT_TRANSLATE_NOOP("Activity", "Commuting"));
			ADD_SPECIFIC("cycling", QT_TRANSLATE_NOOP("Activity", "Cycling"));
			ADD_SPECIFIC("driving", QT_TRANSLATE_NOOP("Activity", "Driving"));
			ADD_SPECIFIC("in_a_car", QT_TRANSLATE_NOOP("Activity", "In a car"));
			ADD_SPECIFIC("on_a_bus", QT_TRANSLATE_NOOP("Activity", "On a bus"));
			ADD_SPECIFIC("on_a_plane", QT_TRANSLATE_NOOP("Activity", "On a plane"));
			ADD_SPECIFIC("on_a_train", QT_TRANSLATE_NOOP("Activity", "On a train"));
			ADD_SPECIFIC("on_a_trip", QT_TRANSLATE_NOOP("Activity", "On a trip"));
			ADD_SPECIFIC("walking", QT_TRANSLATE_NOOP("Activity", "Walking"));
		}
		{
			QString general = "working";
			ADD_GENERAL(general, QT_TRANSLATE_NOOP("Activity", "Working"));
			ADD_SPECIFIC("coding", QT_TRANSLATE_NOOP("Activity", "Coding"));
			ADD_SPECIFIC("in_a_meeting", QT_TRANSLATE_NOOP("Activity", "In a meeting"));
			ADD_SPECIFIC("studying", QT_TRANSLATE_NOOP("Activity", "Studying"));
			ADD_SPECIFIC("writing", QT_TRANSLATE_NOOP("Activity", "Writing"));
		}
	}

#undef ADD_ACTIVITY
#undef ADD_GENERAL
#undef ADD_SPECIFIC

	Q_GLOBAL_STATIC_WITH_INITIALIZER(Activities, activities, init_activities(*x));


	JPersonActivity::JPersonActivity(const QString &name, const LocalizedString &value,
									 const QString &generalActivity) :
		m_name(name),
		m_value(value),
		m_general(generalActivity)
	{
	}

	QString JPersonActivity::iconName() const
	{
		QString iconName = "user-status-";
		if (!m_general.isEmpty())
			iconName += m_general + "-";
		iconName += m_name;
		return iconName;
	}

	ExtensionIcon JPersonActivity::icon() const
	{
		return ExtensionIcon(iconName());
	}

	JPersonActivityConverter::JPersonActivityConverter() :
			m_feature("http://jabber.org/protocol/activity")
	{
		static JPersonActivityRegistrator activityRegistrator;
		Q_UNUSED(activityRegistrator);
	}

	JPersonActivityConverter::~JPersonActivityConverter()
	{
	}

	std::string JPersonActivityConverter::feature() const
	{
		return m_feature;
	}

	QString JPersonActivityConverter::name() const
	{
		return "activity";
	}

	gloox::Tag *JPersonActivityConverter::toXml(const QVariantHash &data) const
	{
		gloox::Tag *t = new gloox::Tag("activity", gloox::XMLNS, m_feature);
		QString str = data.value(QLatin1String("general")).toString();
		if (!str.isEmpty()) {
			gloox::Tag *child = new gloox::Tag(t, str.toStdString());
			str = data.value(QLatin1String("specific")).toString();
			if (!str.isEmpty())
				new gloox::Tag(child, str.toStdString());
			str = data.value(QLatin1String("description")).toString();
			if (!str.isEmpty())
				new gloox::Tag(t, "text", str.toStdString());
		}
		return t;
	}

	QVariantHash JPersonActivityConverter::fromXml(gloox::Tag *tag) const
	{
		QVariantHash data;
		if (!tag->children().empty()) {
			gloox::Tag *child = tag->children().front();
			if (child->name() != "text") {
				QString general = QString::fromStdString(child->name());
				QString specific;
				data.insert(QLatin1String("general"), general);
				if (!child->children().empty()) {
					gloox::Tag *subChild = child->children().front();
					specific = QString::fromStdString(subChild->name());
					data.insert(QLatin1String("specific"), specific);
				}
				if (!!(child = tag->findChild("text"))) {
					QString text = QString::fromStdString(child->cdata());
					data.insert(QLatin1String("description"), text);
				}
				JPersonActivity activity = activities()->contains(specific) ?
										   activities()->value(specific) :
										   activities()->value(general);
				if (activity.isEmpty())
					return data;
				data.insert(QLatin1String("id"), "activity");
				data.insert(QLatin1String("title"), activity.value().toString());
				data.insert(QLatin1String("icon"), qVariantFromValue(activity.icon()));
				data.insert(QLatin1String("showInTooltip"), true);
				data.insert(QLatin1String("priorityInTooltip"), 70);
			}
		}
		return data;
	}

	QMultiHash<QString, JPersonActivity> *JPersonActivityConverter::allActivities()
	{
		return activities();
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
