/****************************************************************************
 *  localizedstring.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "localizedstring.h"
#include <QtCore/QCoreApplication>
#include <QSharedData>

namespace qutim_sdk_0_3
{
	// It may be usefull for localizable XMPP string
	// i.e. Message with body in several languages
	class LocalizationEngine
	{
	public:
		virtual QString text() const = 0;
		virtual ~LocalizationEngine() {}
	};
	// Args are usefull sometimes, i.e. at strings like
	// LocalizedString("", "Hello, %1").arg("Tom"), where
	// there may be several unlocalizable arguments
	// which should be inserted after translation
	class LocalizedStringData : public QSharedData
	{
	public:
		LocalizationEngine *engine;
		QByteArray ctx;
		QByteArray str;
		QList<QString> args;
	};
	
	QString LocalizedString::toString() const
	{
		if (m_ctx.isEmpty())
			return QString::fromUtf8(m_str);
		return QCoreApplication::translate(m_ctx.constData(), m_str.constData(), 0, QCoreApplication::UnicodeUTF8);
	}
}
