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

#include "jpersonmoodconverter.h"
#include <jreen/mood.h>
#include <qutim/extensionicon.h>
#include <qutim/status.h>
#include "../../jprotocol.h"

namespace Jabber
{
    typedef QList<qutim_sdk_0_3::LocalizedString> MoodsNames;
    static MoodsNames init_moods_names()
    {
        MoodsNames names;
        names.append(QT_TRANSLATE_NOOP("Mood", "Afraid"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Amazed"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Amorous"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Angry"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Annoyed"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Anxious"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Aroused"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Ashamed"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Bored"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Brave"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Calm"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Cautious"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Cold"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Confident"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Confused"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Contemplative"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Contented"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Cranky"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Crazy"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Creative"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Curious"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Dejected"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Depressed"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Disappointed"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Disgusted"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Dismayed"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Distracted"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Embarrassed"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Envious"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Excited"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Flirtatious"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Frustrated"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Grateful"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Grieving"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Grumpy"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Guilty"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Happy"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Hopeful"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Hot"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Humbled"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Humiliated"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Hungry"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Hurt"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Impressed"));
        names.append(QT_TRANSLATE_NOOP("Mood", "In awe"));
        names.append(QT_TRANSLATE_NOOP("Mood", "In love"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Indignant"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Interested"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Intoxicated"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Invincible"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Jealous"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Lonely"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Lost"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Lucky"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Mean"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Moody"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Nervous"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Neutral"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Offended"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Outraged"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Playful"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Proud"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Relaxed"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Relieved"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Remorseful"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Restless"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Sad"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Sarcastic"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Satisfied"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Serious"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Shocked"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Shy"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Sick"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Sleepy"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Spontaneous"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Stressed"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Strong"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Surprised"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Thankful"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Thirsty"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Tired"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Undefined"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Weak"));
        names.append(QT_TRANSLATE_NOOP("Mood", "Worried"));
        return names;
    }

    Q_GLOBAL_STATIC_WITH_ARGS(MoodsNames, moodsNames, (init_moods_names()));
	Q_GLOBAL_STATIC(JPersonMoodRegistrator, moodRegistrator);

	JPersonMoodConverter::JPersonMoodConverter()
	{
		moodRegistrator();
	}
	
	JPersonMoodConverter::~JPersonMoodConverter()
	{
	}
	
	QString JPersonMoodConverter::name() const
	{
		return QLatin1String("mood");
	}
	
	int JPersonMoodConverter::entityType() const
	{
		return Jreen::Mood::staticPayloadType();
	}
	
	QSharedPointer<Jreen::Payload> JPersonMoodConverter::convertTo(const QVariantHash &map) const
	{
		QString mood = map.value(QLatin1String("mood")).toString();
		QString text = map.value(QLatin1String("description")).toString();
		return Jreen::Payload::Ptr(new Jreen::Mood(mood, text));
	}
	
	QVariantHash JPersonMoodConverter::convertFrom(const QSharedPointer<Jreen::Payload> &entity) const
	{
		Jreen::Mood *mood = Jreen::se_cast<Jreen::Mood*>(entity.data());
		QVariantHash data;
		data.insert(QLatin1String("id"), QLatin1String("mood"));
		if (mood->type() <= Jreen::Mood::Invalid)
			return data;
		data.insert(QLatin1String("mood"), mood->typeName());
		data.insert(QLatin1String("title"), qVariantFromValue(moodsNames()->value(mood->type())));
		if (!mood->text().isEmpty())
			data.insert(QLatin1String("description"), mood->text());
		qutim_sdk_0_3::ExtensionIcon icon(QLatin1String("user-status-") + mood->typeName());
		data.insert(QLatin1String("icon"), qVariantFromValue(icon));
		data.insert(QLatin1String("showInTooltip"), true);
		data.insert(QLatin1String("priorityInTooltip"), 70);
		return data;
	}

	QList<qutim_sdk_0_3::LocalizedString> JPersonMoodConverter::moods()
	{
		return *moodsNames();
	}

	JPersonMoodRegistrator::JPersonMoodRegistrator()
	{
		JProtocol::instance()->installEventFilter(this);
	}

	bool JPersonMoodRegistrator::eventFilter(QObject *obj, QEvent *ev)
	{
		if (ev->type() == ExtendedInfosEvent::eventType() && obj == JProtocol::instance()) {
			ExtendedInfosEvent *event = static_cast<ExtendedInfosEvent*>(ev);
			QVariantHash extStatus;
			extStatus.insert("id", "mood");
			extStatus.insert("name", QT_TRANSLATE_NOOP("Mood", "Mood").toString());
			extStatus.insert("settingsDescription",
							 QT_TRANSLATE_NOOP("Mood", "Show contact mood icon").toString());
			event->addInfo("mood", extStatus);
		}
		return false;
	}
}

