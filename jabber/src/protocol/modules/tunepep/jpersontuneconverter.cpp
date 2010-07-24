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

namespace Jabber
{
	JPersonTuneConverter::JPersonTuneConverter() : m_feature("http://jabber.org/protocol/tune")
	{
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
	
	gloox::Tag *JPersonTuneConverter::toXml(const QVariantMap &map) const
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
	
	QVariantMap JPersonTuneConverter::fromXml(gloox::Tag *tag) const
	{
		QVariantMap data;
		
        gloox::Tag* t = tag->findChild("artist");
        if(t)
			data.insert(QLatin1String("artist"), QString::fromStdString(t->cdata()));
        t = tag->findChild("length");
        if(t)
			data.insert(QLatin1String("length"), QString::fromStdString(t->cdata()).toInt());
        t = tag->findChild("rating");
        if(t)
			data.insert(QLatin1String("rating"), QString::fromStdString(t->cdata()).toInt());
        t = tag->findChild("source");
        if(t)
			data.insert(QLatin1String("source"), QString::fromStdString(t->cdata()));
        t = tag->findChild("title");
        if(t)
			data.insert(QLatin1String("title"), QString::fromStdString(t->cdata()));
        t = tag->findChild("track");
        if(t)
			data.insert(QLatin1String("track"), QString::fromStdString(t->cdata()));
        t = tag->findChild("uri");
        if(t)
			data.insert(QLatin1String("uri"), QString::fromStdString(t->cdata()));
		
		return data;
	}
}
