/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "jpersonmoodconverter.h"
#include <gloox/tag.h>
#include <qutim/extensionicon.h>
#include <qutim/localizedstring.h>
#include <qutim/status.h>
#include <protocol/jprotocol.h>

namespace Jabber
{
	typedef QHash<QString, qutim_sdk_0_3::LocalizedString> MoodsNames;
	static void init_moods_names(MoodsNames &names)
	{
		names.insert("afraid", QT_TRANSLATE_NOOP("Mood", "Afraid"));
		names.insert("amazed", QT_TRANSLATE_NOOP("Mood", "Amazed"));
		names.insert("amorous", QT_TRANSLATE_NOOP("Mood", "Amorous"));
		names.insert("angry", QT_TRANSLATE_NOOP("Mood", "Angry"));
		names.insert("annoyed", QT_TRANSLATE_NOOP("Mood", "Annoyed"));
		names.insert("anxious", QT_TRANSLATE_NOOP("Mood", "Anxious"));
		names.insert("aroused", QT_TRANSLATE_NOOP("Mood", "Aroused"));
		names.insert("ashamed", QT_TRANSLATE_NOOP("Mood", "Ashamed"));
		names.insert("bored", QT_TRANSLATE_NOOP("Mood", "Bored"));
		names.insert("brave", QT_TRANSLATE_NOOP("Mood", "Brave"));
		names.insert("calm", QT_TRANSLATE_NOOP("Mood", "Calm"));
		names.insert("cautious", QT_TRANSLATE_NOOP("Mood", "Cautious"));
		names.insert("cold", QT_TRANSLATE_NOOP("Mood", "Cold"));
		names.insert("confident", QT_TRANSLATE_NOOP("Mood", "Confident"));
		names.insert("confused", QT_TRANSLATE_NOOP("Mood", "Confused"));
		names.insert("contemplative", QT_TRANSLATE_NOOP("Mood", "Contemplative"));
		names.insert("contented", QT_TRANSLATE_NOOP("Mood", "Contented"));
		names.insert("cranky", QT_TRANSLATE_NOOP("Mood", "Cranky"));
		names.insert("crazy", QT_TRANSLATE_NOOP("Mood", "Crazy"));
		names.insert("creative", QT_TRANSLATE_NOOP("Mood", "Creative"));
		names.insert("curious", QT_TRANSLATE_NOOP("Mood", "Curious"));
		names.insert("dejected", QT_TRANSLATE_NOOP("Mood", "Dejected"));
		names.insert("depressed", QT_TRANSLATE_NOOP("Mood", "Depressed"));
		names.insert("disappointed", QT_TRANSLATE_NOOP("Mood", "Disappointed"));
		names.insert("disgusted", QT_TRANSLATE_NOOP("Mood", "Disgusted"));
		names.insert("dismayed", QT_TRANSLATE_NOOP("Mood", "Dismayed"));
		names.insert("distracted", QT_TRANSLATE_NOOP("Mood", "Distracted"));
		names.insert("embarrassed", QT_TRANSLATE_NOOP("Mood", "Embarrassed"));
		names.insert("envious", QT_TRANSLATE_NOOP("Mood", "Envious"));
		names.insert("excited", QT_TRANSLATE_NOOP("Mood", "Excited"));
		names.insert("flirtatious", QT_TRANSLATE_NOOP("Mood", "Flirtatious"));
		names.insert("frustrated", QT_TRANSLATE_NOOP("Mood", "Frustrated"));
		names.insert("grateful", QT_TRANSLATE_NOOP("Mood", "Grateful"));
		names.insert("grieving", QT_TRANSLATE_NOOP("Mood", "Grieving"));
		names.insert("grumpy", QT_TRANSLATE_NOOP("Mood", "Grumpy"));
		names.insert("guilty", QT_TRANSLATE_NOOP("Mood", "Guilty"));
		names.insert("happy", QT_TRANSLATE_NOOP("Mood", "Happy"));
		names.insert("hopeful", QT_TRANSLATE_NOOP("Mood", "Hopeful"));
		names.insert("hot", QT_TRANSLATE_NOOP("Mood", "Hot"));
		names.insert("humbled", QT_TRANSLATE_NOOP("Mood", "Humbled"));
		names.insert("humiliated", QT_TRANSLATE_NOOP("Mood", "Humiliated"));
		names.insert("hungry", QT_TRANSLATE_NOOP("Mood", "Hungry"));
		names.insert("hurt", QT_TRANSLATE_NOOP("Mood", "Hurt"));
		names.insert("impressed", QT_TRANSLATE_NOOP("Mood", "Impressed"));
		names.insert("in_awe", QT_TRANSLATE_NOOP("Mood", "In awe"));
		names.insert("in_love", QT_TRANSLATE_NOOP("Mood", "In love"));
		names.insert("indignant", QT_TRANSLATE_NOOP("Mood", "Iindignant"));
		names.insert("interested", QT_TRANSLATE_NOOP("Mood", "Interested"));
		names.insert("intoxicated", QT_TRANSLATE_NOOP("Mood", "Intoxicated"));
		names.insert("invincible", QT_TRANSLATE_NOOP("Mood", "Invincible"));
		names.insert("jealous", QT_TRANSLATE_NOOP("Mood", "Jealous"));
		names.insert("lonely", QT_TRANSLATE_NOOP("Mood", "Lonely"));
		names.insert("lost", QT_TRANSLATE_NOOP("Mood", "Lost"));
		names.insert("lucky", QT_TRANSLATE_NOOP("Mood", "Lucky"));
		names.insert("mean", QT_TRANSLATE_NOOP("Mood", "Mean"));
		names.insert("moody", QT_TRANSLATE_NOOP("Mood", "Moody"));
		names.insert("nervous", QT_TRANSLATE_NOOP("Mood", "Nervous"));
		names.insert("neutral", QT_TRANSLATE_NOOP("Mood", "Neutral"));
		names.insert("offended", QT_TRANSLATE_NOOP("Mood", "Offended"));
		names.insert("outraged", QT_TRANSLATE_NOOP("Mood", "Outraged"));
		names.insert("playful", QT_TRANSLATE_NOOP("Mood", "Playful"));
		names.insert("proud", QT_TRANSLATE_NOOP("Mood", "Proud"));
		names.insert("relaxed", QT_TRANSLATE_NOOP("Mood", "Relaxed"));
		names.insert("relieved", QT_TRANSLATE_NOOP("Mood", "Relieved"));
		names.insert("remorseful", QT_TRANSLATE_NOOP("Mood", "Remorseful"));
		names.insert("restless", QT_TRANSLATE_NOOP("Mood", "Restless"));
		names.insert("sad", QT_TRANSLATE_NOOP("Mood", "Sad"));
		names.insert("sarcastic", QT_TRANSLATE_NOOP("Mood", "Sarcastic"));
		names.insert("satisfied", QT_TRANSLATE_NOOP("Mood", "Satisfied"));
		names.insert("serious", QT_TRANSLATE_NOOP("Mood", "Serious"));
		names.insert("shocked", QT_TRANSLATE_NOOP("Mood", "Shocked"));
		names.insert("shy", QT_TRANSLATE_NOOP("Mood", "Shy"));
		names.insert("sick", QT_TRANSLATE_NOOP("Mood", "Sick"));
		names.insert("sleepy", QT_TRANSLATE_NOOP("Mood", "Sleepy"));
		names.insert("spontaneous", QT_TRANSLATE_NOOP("Mood", "Spontaneous"));
		names.insert("stressed", QT_TRANSLATE_NOOP("Mood", "Stressed"));
		names.insert("strong", QT_TRANSLATE_NOOP("Mood", "Strong"));
		names.insert("surprised", QT_TRANSLATE_NOOP("Mood", "Surprised"));
		names.insert("thankful", QT_TRANSLATE_NOOP("Mood", "Thankful"));
		names.insert("thirsty", QT_TRANSLATE_NOOP("Mood", "Thirsty"));
		names.insert("tired", QT_TRANSLATE_NOOP("Mood", "Tired"));
		names.insert("undefined", QT_TRANSLATE_NOOP("Mood", "Undefined"));
		names.insert("weak", QT_TRANSLATE_NOOP("Mood", "Weak"));
		names.insert("worried", QT_TRANSLATE_NOOP("Mood", "Worried"));
	}

	Q_GLOBAL_STATIC_WITH_INITIALIZER(MoodsNames, moodsNames, init_moods_names(*x));

	JPersonMoodConverter::JPersonMoodConverter() : m_feature("http://jabber.org/protocol/mood")
	{
		static JPersonMoodRegistrator moodRegistrator;
		Q_UNUSED(moodRegistrator)
	}
	
	JPersonMoodConverter::~JPersonMoodConverter()
	{
	}

	std::string JPersonMoodConverter::feature() const
	{
		return m_feature;
	}
	
	QString JPersonMoodConverter::name() const
	{
		return QLatin1String("mood");
	}
	
	gloox::Tag *JPersonMoodConverter::toXml(const QVariantHash &map) const
	{
		gloox::Tag *t = new gloox::Tag("mood", gloox::XMLNS, m_feature);
		QString str = map.value(QLatin1String("mood")).toString();
		if (!str.isEmpty()) {
			new gloox::Tag(t, str.toStdString());
			str = map.value(QLatin1String("description")).toString();
			if (!str.isEmpty())
				new gloox::Tag(t, "text", str.toStdString());
		}
		return t;
	}
	
	QVariantHash JPersonMoodConverter::fromXml(gloox::Tag *tag) const
	{
		QVariantHash data;
		if (!tag->children().empty()) {
			gloox::Tag *child = tag->children().front();
			if (child->name() != "text") {
				QString mood = QString::fromStdString(child->name());
				data.insert(QLatin1String("mood"), mood);
				if (!!(child = tag->findChild("text"))) {
					QString text = QString::fromStdString(tag->cdata());
					data.insert(QLatin1String("description"), text);
				}
				data.insert(QLatin1String("id"), "mood");
				data.insert(QLatin1String("title"), qVariantFromValue(moodsNames()->value(mood)));
				qutim_sdk_0_3::ExtensionIcon icon("user-status-" + mood);
				data.insert(QLatin1String("icon"), qVariantFromValue(icon));
				data.insert(QLatin1String("showInTooltip"), true);
				data.insert(QLatin1String("priorityInTooltip"), 70);
			}
		}
		return data;
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
			extStatus.insert("name", tr("Mood"));
			extStatus.insert("settingsDescription", tr("Show contact mood icon"));
			event->addInfo("xstatus", extStatus);
		}
		return false;
	}
}
