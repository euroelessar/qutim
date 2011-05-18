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

#ifndef JPERSONTUNECONVERTER_H
#define JPERSONTUNECONVERTER_H

#include "../../../sdk/jabber.h"

namespace Jabber
{
	class JPersonTuneConverter : public QObject, public PersonEventConverter
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::PersonEventConverter)
	public:
		JPersonTuneConverter();
		virtual ~JPersonTuneConverter();
		
		virtual QString name() const;
		virtual int entityType() const;
		virtual QSharedPointer<Jreen::StanzaExtension> convertTo(const QVariantHash &map) const;
		virtual QVariantHash convertFrom(const QSharedPointer<Jreen::StanzaExtension> &entity) const;
	};

	class JPersonTuneRegistrator : public QObject
	{
		Q_OBJECT
	public:
		JPersonTuneRegistrator();
	protected:
		bool eventFilter(QObject *obj, QEvent *event);
	};
}

#endif // JPERSONTUNECONVERTER_H
