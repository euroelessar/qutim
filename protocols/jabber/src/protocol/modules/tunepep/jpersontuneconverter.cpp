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

#include "jpersontuneconverter.h"
#include <jreen/tune.h>
#include <qutim/extensionicon.h>
#include <protocol/jprotocol.h>
#include <QUrl>

namespace Jabber
{
	JPersonTuneConverter::JPersonTuneConverter()
	{
		static JPersonTuneRegistrator tuneRegistrator;
		Q_UNUSED(tuneRegistrator);
	}
	
	JPersonTuneConverter::~JPersonTuneConverter()
	{
	}
	
	QString JPersonTuneConverter::name() const
	{
		return QLatin1String("tune");
	}
	
	int JPersonTuneConverter::entityType() const
	{
		return Jreen::Tune::staticPayloadType();
	}
	
	QSharedPointer<Jreen::Payload> JPersonTuneConverter::convertTo(const QVariantHash &map) const
	{
		Jreen::Tune *tune = new Jreen::Tune();
		bool ok = true;
		tune->setArtist(map.value(QLatin1String("artist")).toString());
		tune->setLength(map.value(QLatin1String("length")).toInt(&ok) * ok + ok - 1);
		tune->setRating(map.value(QLatin1String("rating")).toInt(&ok) * ok + ok - 1);
		tune->setSource(map.value(QLatin1String("source")).toString());
		tune->setTitle(map.value(QLatin1String("title")).toString());
		tune->setTrack(map.value(QLatin1String("track")).toString());
		tune->setUri(map.value(QLatin1String("uri")).toUrl());
		return Jreen::Payload::Ptr(tune);
	}
	
#define ADD_TAG(Tag, IsValid, ValueMethod)\
		if(tune->Tag() IsValid) { \
			QString value = ValueMethod(tune->Tag()); \
			data.insert(QLatin1String(#Tag), tune->Tag()); \
			if (!description.isEmpty()) \
				description += QLatin1String(" - "); \
			description += value; \
	   }
#define ADD_TEXT_TAG(Tag) \
		ADD_TAG(Tag, .length() > 0, )
		
	QString uriToStringHelper(const QUrl &uri)
	{
		return uri.toString();
	}

	QVariantHash JPersonTuneConverter::convertFrom(const QSharedPointer<Jreen::Payload> &entity) const
	{
		Jreen::Tune *tune = Jreen::se_cast<Jreen::Tune*>(entity.data());
		Q_ASSERT(tune);
		QVariantHash data;
		QString description;
		
		ADD_TEXT_TAG(artist);
		ADD_TAG(length, > -1, QString::number);
		ADD_TAG(rating, > -1, QString::number);
		ADD_TEXT_TAG(source);
		ADD_TEXT_TAG(title);
		ADD_TEXT_TAG(track);
		ADD_TAG(uri, .isValid(), uriToStringHelper);

		if (!data.isEmpty()) {
			data.insert(QLatin1String("id"), "tune");
			data.insert(QLatin1String("title"), QT_TRANSLATE_NOOP("Tune", "Now listening").toString());
			data.insert(QLatin1String("description"), description);
			data.insert(QLatin1String("icon"),
						qVariantFromValue(qutim_sdk_0_3::ExtensionIcon("user-status-listening_to_music")));
			data.insert(QLatin1String("showInTooltip"), true);
			data.insert(QLatin1String("priorityInTooltip"), 70);
		}

		return data;
	}

	JPersonTuneRegistrator::JPersonTuneRegistrator()
	{
		JProtocol::instance()->installEventFilter(this);
	}

	bool JPersonTuneRegistrator::eventFilter(QObject *obj, QEvent *ev)
	{
		if (ev->type() == ExtendedInfosEvent::eventType() && obj == JProtocol::instance()) {
			ExtendedInfosEvent *event = static_cast<ExtendedInfosEvent*>(ev);
			QVariantHash extStatus;
			extStatus.insert("id", "tune");
			extStatus.insert("name", QT_TRANSLATE_NOOP("Tune", "Tune").toString());
			extStatus.insert("settingsDescription",
							 QT_TRANSLATE_NOOP("Tune", "Show tune icon").toString());
			event->addInfo("tune", extStatus);
		}
		return false;
	}

}

