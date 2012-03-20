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

#include "tooltip.h"
#include "objectgenerator.h"
#include "buddy.h"
#include "conference.h"
#include "account.h"
#include "protocol.h"
#include "iconloader.h"
#include <QWeakPointer>
#include <QCoreApplication>
#include <QAbstractItemView>
#include <QHelpEvent>
#include <QStringBuilder>
#include <QToolTip>
#include <QTextDocument>

namespace qutim_sdk_0_3
{
	typedef QMultiMap<quint8, QString> FieldsMap;

	class ToolTipEventPrivate
	{
	public:
		inline void addIcon(QString &text, const QString &icon);
		FieldsMap fields;
		bool generateLayout;
	};

	void ToolTipEventPrivate::addIcon(QString &text, const QString &icon)
	{
		if (!icon.isEmpty()) {
			QString iconPath = IconLoader::iconPath(icon, 16);
			if (!iconPath.isEmpty()) {
				if (!text.isEmpty())
					text += " ";
				text += "<img width='16' height='16' src='" + iconPath + "'> ";
			}
		}
	}

	ToolTipEvent::ToolTipEvent(bool generateLayout) :
		QEvent(eventType()), d(new ToolTipEventPrivate)
	{
		d->generateLayout = generateLayout;
	}


	ToolTipEvent::~ToolTipEvent()
	{
	}

	void ToolTipEvent::addHtml(const QString &html, quint8 priority)
	{
		d->fields.insert(priority, html);
	}

	void ToolTipEvent::addField(const LocalizedString &title, const LocalizedString &description,
								quint8 priority)
	{
		addField(title, description, QString(), IconBeforeTitle, priority);
	}

	void ToolTipEvent::addField(const LocalizedString &title, const LocalizedString &description,
								const QString &icon, quint8 priority)
	{
		addField(title, description, icon, IconBeforeTitle, priority);
	}

	void ToolTipEvent::addField(const LocalizedString &title, const LocalizedString &description,
								const ExtensionIcon &icon, quint8 priority)
	{
		addField(title, description, icon.name(), IconBeforeTitle, priority);
	}

	void ToolTipEvent::addField(const LocalizedString &title, const LocalizedString &description,
								const QString &icon, IconPosition iconPosition, quint8 priority)
	{
		QString text;
		bool descriptionEmpty = description.toString().isEmpty();
		if (iconPosition == IconBeforeTitle)
			d->addIcon(text, icon);
		if (!title.toString().isEmpty()) {
			text += QLatin1Literal("<b>") % title.toString();
			if (!descriptionEmpty)
				text += ":";
			text += "</b>";
		}
		if (iconPosition == IconBeforeDescription)
			d->addIcon(text, icon);
		if (!descriptionEmpty) {
			if (!text.isEmpty())
				text += " ";
			text += description.toString();
		}
		if (!text.isEmpty())
			text.prepend("<br/>");
		d->fields.insert(priority, text);
	}

	void ToolTipEvent::addField(const LocalizedString &title, const LocalizedString &description,
								const ExtensionIcon &icon, IconPosition iconPosition, quint8 priority)
	{
		addField(title, description, icon.name(), iconPosition, priority);
	}

	bool ToolTipEvent::generateLayout() const
	{
		return d->generateLayout;
	}

	QString ToolTipEvent::html() const
	{
		QString text;
		QMapIterator<quint8, QString> i(d->fields);
		i.toBack();
		while (i.hasPrevious())
			text += i.previous().value();
		text.remove(QRegExp("^<br/>"));
		return text;
	}

	QEvent::Type ToolTipEvent::eventType()
	{
		static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 103));
		return type;
	}

	struct ToolTipManagerData
	{
		ToolTipManagerData() : isInited(false) {}
		QWeakPointer<ToolTip> self;
		bool isInited;
	};

	Q_GLOBAL_STATIC(ToolTipManagerData, p)

	ToolTip *ToolTip::instance()
	{
		ToolTipManagerData *d = p();
		if (!d->isInited) {
			d->isInited = true;
			GeneratorList gens = ObjectGenerator::module<ToolTip>();
			if (gens.isEmpty())
				d->self = new ToolTip();
			else
				d->self = gens.first()->generate<ToolTip>();
//			qApp->installEventFilter(d->self);
		}
		return d->self.data();
	}

	ToolTip::ToolTip(QObject *parent) :
		QObject(parent)
	{
	}

	void ToolTip::showText(const QPoint &pos, QObject *obj, QWidget *w)
	{
		if (!obj)
			return;
		ToolTipEvent event;
		qApp->sendEvent(obj, &event);
		QString text = event.html();
		if (text.isEmpty())
			QToolTip::hideText();
		else
			QToolTip::showText(pos, text, w);
	}

	bool ToolTip::eventFilter(QObject *obj, QEvent *ev)
	{
//		if (ev->type() == QEvent::ToolTip) {
//			QHelpEvent *he = static_cast<QHelpEvent*>(event);
//			if (qobject_cast<QAbstractItemView *>(obj)) {
//				const QModelIndex index = indexAt(he->pos());
//				QStyleOptionViewItemV4 option = d->viewOptionsV4();
//				option.rect = visualRect(index);
//				option.state |= (index == currentIndex() ? QStyle::State_HasFocus : QStyle::State_None);
//				bool retval = false;
//				// ### Qt 5: make this a normal function call to a virtual function
//				QMetaObject::invokeMethod(d->delegateForIndex(index), "helpEvent",
//										  Q_RETURN_ARG(bool, retval),
//										  Q_ARG(QHelpEvent *, he),
//										  Q_ARG(QAbstractItemView *, this),
//										  Q_ARG(QStyleOptionViewItem, option),
//										  Q_ARG(QModelIndex, index));
//			}
//		}
		return QObject::eventFilter(obj, ev);
	}

}

