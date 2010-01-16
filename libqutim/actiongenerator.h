/****************************************************************************
 *  actiongenerator.h
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

#ifndef ACTIONGENERATOR_H
#define ACTIONGENERATOR_H

#include "objectgenerator.h"
#include "localizedstring.h"
#include <QtGui/QIcon>
#include <QtGui/QAction>

namespace qutim_sdk_0_3
{
	class ActionGeneratorPrivate;
	class MenuController;

	class LIBQUTIM_EXPORT ActionGenerator : public ObjectGenerator
	{
		Q_DECLARE_PRIVATE(ActionGenerator)
		Q_DISABLE_COPY(ActionGenerator)
	public:
		ActionGenerator(const QIcon &icon, const LocalizedString &text, const QObject *receiver, const char *member);
		virtual ~ActionGenerator();
		QIcon icon() const;
		const LocalizedString &text() const;
		const QObject *receiver() const;
		const char *member() const;
		ActionGenerator *addProperty(const QByteArray &name, const QVariant &value);
		int type() const;
		ActionGenerator *setType(int type);
		int priority() const;
		ActionGenerator *setPriority(int priority);
		void setMenuController(MenuController *controller);
	protected:
		QAction *prepareAction(QAction *action) const;
		virtual QObject *generateHelper() const;
		virtual const QMetaObject *metaObject() const;
		virtual bool hasInterface(const char *id) const;
	};
}

#endif // ACTIONGENERATOR_H
