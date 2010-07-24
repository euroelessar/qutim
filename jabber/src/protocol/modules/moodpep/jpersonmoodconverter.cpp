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

namespace Jabber
{
	JPersonMoodConverter::JPersonMoodConverter() : m_feature("http://jabber.org/protocol/mood")
	{
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
	
	gloox::Tag *JPersonMoodConverter::toXml(const QVariantMap &map) const
	{
		gloox::Tag *t = new gloox::Tag("mood", gloox::XMLNS, m_feature);
		QString str = map.value(QLatin1String("mood")).toString();
		if (!str.isEmpty()) {
			new gloox::Tag(t, str.toStdString());
			str = map.value(QLatin1String("text")).toString();
			if (!str.isEmpty())
				new gloox::Tag(t, "text", str.toStdString());
		}
		return t;
	}
	
	QVariantMap JPersonMoodConverter::fromXml(gloox::Tag *tag) const
	{
		QVariantMap data;
		if (!tag->children().empty()) {
			gloox::Tag *child = tag->children().front();
			if (child->name() != "text") {
				QString mood = QString::fromStdString(child->name());
				data.insert(QLatin1String("mood"), mood);
				if (!!(child = tag->findChild("text"))) {
					QString text = QString::fromStdString(tag->cdata());
					data.insert(QLatin1String("text"), text);
				}
			}
		}
		return data;
	}
}
