/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef TOOLTIPMANAGER_H
#define TOOLTIPMANAGER_H

#include "libqutim_global.h"
#include "localizedstring.h"
#include "extensionicon.h"
#include <QPoint>
#include <QEvent>
#include <QVariant>

namespace qutim_sdk_0_3
{
	class Buddy;
	class ToolTipEventPrivate;

	class LIBQUTIM_EXPORT ToolTipEvent : public QEvent
	{
	public:
		enum IconPosition {
			IconBeforeTitle = 0,
			IconBeforeDescription
		};

		ToolTipEvent(bool generateLayout = true);
		~ToolTipEvent();
		void addHtml(const QString &html, quint8 priority = 60);
		void addField(const LocalizedString &title,
					  const LocalizedString &description = LocalizedString(),
					  quint8 priority = 60);
		void addField(const LocalizedString &title,
					  const LocalizedString &description,
					  const QString &icon,
					  quint8 priority = 60);
		void addField(const LocalizedString &title,
					  const LocalizedString &description,
					  const ExtensionIcon &icon,
					  quint8 priority = 60);
		void addField(const LocalizedString &title,
					  const LocalizedString &description,
					  const QString &icon,
					  IconPosition iconPosition,
					  quint8 priority = 60);
		void addField(const LocalizedString &title,
					  const LocalizedString &description,
					  const ExtensionIcon &icon,
					  IconPosition iconPosition,
					  quint8 priority = 60);
		bool generateLayout() const;
		QString html() const;
		static QEvent::Type eventType();
	protected:
		friend class ToolTip;
		QScopedPointer<ToolTipEventPrivate> d;
	};

	class LIBQUTIM_EXPORT ToolTip : public QObject
	{
		Q_OBJECT
	public:
		static ToolTip *instance();
		virtual void showText(const QPoint &pos, QObject *obj, QWidget *w = 0);
		inline void hideText() { showText(QPoint(), 0); }
	protected:
		ToolTip(QObject *parent = 0);
		bool eventFilter(QObject *, QEvent *);
	};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ToolTipEvent::IconPosition)

#endif // TOOLTIPMANAGER_H

