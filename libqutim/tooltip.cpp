/****************************************************************************
 *  tooltip.cpp
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

#include "tooltip.h"
#include "objectgenerator.h"
#include "buddy.h"
#include "conference.h"
#include "account.h"
#include "protocol.h"
#include "iconloader.h"
#include <QPointer>
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
		FieldsMap fields;
		bool generateLayout;
	};

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

	void ToolTipEvent::addField(const LocalizedString &title, const LocalizedString &data,
								quint8 priority)
	{
		addField(title, data, QString(), priority);
	}

	void ToolTipEvent::addField(const LocalizedString &title, const LocalizedString &data,
								const QString &icon, quint8 priority)
	{
		QString text;
		bool dataEmpty = data.toString().isEmpty();
		if (!icon.isEmpty()) {
			QString iconPath = IconLoader::instance()->iconPath(icon, 16);
			if (!iconPath.isEmpty())
				text += "<img src='" + iconPath + "'> ";
		}
		if (!title.toString().isEmpty()) {
			text += QLatin1Literal("<b>") % title.toString();
			if (!dataEmpty)
				text += ":";
			text += "</b>";
		}
		if (!dataEmpty) {
			if (!text.isEmpty())
				text += " ";
			text += data.toString();
		}
		if (!text.isEmpty())
			text.prepend("<br/>");
		d->fields.insert(priority, text);
	}

	void ToolTipEvent::addField(const LocalizedString &title, const LocalizedString &data,
								const ExtensionIcon &icon, quint8 priority)
	{
		addField(title, data, icon.name(), priority);
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
		QPointer<ToolTip> self;
		bool isInited;
	};

	Q_GLOBAL_STATIC(ToolTipManagerData, p)

	ToolTip *ToolTip::instance()
	{
		ToolTipManagerData *d = p();
		if (!d->isInited) {
			d->isInited = true;
			GeneratorList gens = moduleGenerators<ToolTip>();
			if (gens.isEmpty())
				d->self = new ToolTip();
			else
				d->self = gens.first()->generate<ToolTip>();
//			qApp->installEventFilter(d->self);
		}
		return d->self;
	}

	ToolTip::ToolTip(QObject *parent) :
		QObject(parent)
	{
	}

	void ToolTip::showText(const QPoint &pos, QObject *obj, QWidget *w)
	{
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
