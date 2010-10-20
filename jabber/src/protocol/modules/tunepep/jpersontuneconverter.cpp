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
#include <gloox/tag.h>
#include <qutim/extensionicon.h>
#include <protocol/jprotocol.h>

namespace Jabber
{
	JPersonTuneConverter::JPersonTuneConverter() : m_feature("http://jabber.org/protocol/tune")
	{
		static JPersonTuneRegistrator tuneRegistrator;
		Q_UNUSED(tuneRegistrator);
	}
	
	JPersonTuneConverter::~JPersonTuneConverter()
	{
	}

	std::string JPersonTuneConverter::feature() const
	{
		return m_feature;
	}
	
	QString JPersonTuneConverter::name() const
	{
		return QLatin1String("tune");
	}
	
	gloox::Tag *JPersonTuneConverter::toXml(const QVariantHash &map) const
	{
		gloox::Tag *t = new gloox::Tag("tune", gloox::XMLNS, m_feature);
		
		QVariant var = map.value(QLatin1String("artist"));
		if (var.isValid())
			new gloox::Tag(t, "artist", var.toString().toStdString());
		var = map.value(QLatin1String("length"));
		if (var.isValid()) {
			bool ok;
			int length = var.toInt(&ok);
			if (ok && length > 0)
				new gloox::Tag(t, "length", var.toString().toStdString());
		}
		var = map.value(QLatin1String("rating"));
		if (var.isValid()) {
			bool ok;
			int rating = var.toInt(&ok);
			if (ok && rating > 0 && rating <= 10)
				new gloox::Tag(t, "rating", var.toString().toStdString());
		}
		var = map.value(QLatin1String("source"));
		if (var.isValid())
			new gloox::Tag(t, "source", var.toString().toStdString());
		var = map.value(QLatin1String("title"));
		if (var.isValid())
			new gloox::Tag(t, "title", var.toString().toStdString());
		var = map.value(QLatin1String("track"));
		if (var.isValid())
			new gloox::Tag(t, "track", var.toString().toStdString());
		var = map.value(QLatin1String("uri"));
		if (var.isValid())
			new gloox::Tag(t, "uri", var.toString().toStdString());

		return t;
	}

#define ADD_TAG(Tag, ValueMethod)\
	t = tag->findChild(#Tag); \
	if(t) { \
		QString value = QString::fromStdString(t->cdata()); \
		data.insert(QLatin1String(#Tag), value ValueMethod); \
		if (!description.isEmpty()) \
			description += " - "; \
		description += value; \
	}


	QVariantHash JPersonTuneConverter::fromXml(gloox::Tag *tag) const
	{
		QVariantHash data;
		QString description;
		
		gloox::Tag* t;
		ADD_TAG(artist,);
		ADD_TAG(length,.toInt());
		ADD_TAG(rating,.toInt());
		ADD_TAG(source,);
		ADD_TAG(title,);
		ADD_TAG(track,);
		ADD_TAG(uri,);

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

#undef ADD_TAG

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

