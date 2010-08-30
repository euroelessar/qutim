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

#ifndef JPERSONMOODCONVERTER_H
#define JPERSONMOODCONVERTER_H

#include "../../../sdk/jabber.h"

namespace Jabber
{
	class JPersonMoodConverter : public QObject, public PersonEventConverter
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::PersonEventConverter)
	public:
		JPersonMoodConverter();
		virtual ~JPersonMoodConverter();
		virtual std::string feature() const;
		virtual QString name() const;
		virtual gloox::Tag *toXml(const QVariantHash &map) const;
		virtual QVariantHash fromXml(gloox::Tag *tag) const;
	private:
		std::string m_feature;
	};

	class JPersonMoodRegistrator : public QObject
	{
	public:
		JPersonMoodRegistrator();
	protected:
		bool eventFilter(QObject *obj, QEvent *event);
	};
}

#endif // JPERSONMOODCONVERTER_H
