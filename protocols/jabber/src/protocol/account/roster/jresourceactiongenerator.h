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
#ifndef RESOURCEACTIONGENERATOR_H
#define RESOURCEACTIONGENERATOR_H

#include <qutim/actiongenerator.h>

namespace Jabber
{
	class JResourceActionGeneratorPrivate;

	class JResourceActionGenerator : public qutim_sdk_0_3::ActionGenerator
	{
		Q_DECLARE_PRIVATE(JResourceActionGenerator)
	public:
		JResourceActionGenerator(const QIcon &icon, const qutim_sdk_0_3::LocalizedString &text,
								const QObject *receiver, const char *member);
		virtual ~JResourceActionGenerator();
		void setFeature(const QString &feature);
		void setFeature(const std::string &feature);
		void setFeature(const QLatin1String &feature);
	protected:
		virtual QObject *generateHelper() const;
		QScopedPointer<JResourceActionGeneratorPrivate> d_ptr;
	};
}

#endif // RESOURCEACTIONGENERATOR_H

