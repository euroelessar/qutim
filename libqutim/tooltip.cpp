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
#include "account.h"
#include "protocol.h"
#include "iconloader.h"
#include <QPointer>
#include <QCoreApplication>
#include <QAbstractItemView>
#include <QHelpEvent>
#include <QStringBuilder>
#include <QToolTip>

namespace qutim_sdk_0_3
{
	struct InfoField
	{
		LocalizedString name;
		QVariant data;
		QString icon;
	};
	typedef QList<InfoField> InfoFieldList;

	class ToolTipEventPrivate
	{
	public:
		InfoFieldList list;
		bool extra;
	};

	ToolTipEvent::ToolTipEvent(bool extra) :
		QEvent(eventType()), d(new ToolTipEventPrivate)
	{
		d->extra = extra;
	}

	void ToolTipEvent::appendField(const LocalizedString &title, const QVariant &data, const QString &icon)
	{
		InfoField field = { title, data, icon };
		d->list.append(field);
	}

	void ToolTipEvent::appendField(const LocalizedString &title, const QVariant &data, const ExtensionIcon &icon)
	{
		appendField(title, data, icon.name());
	}

        ToolTipEvent::~ToolTipEvent()
        {

        }

	bool ToolTipEvent::extra() const
	{
		return d->extra;
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
		if (Buddy *c = qobject_cast<Buddy *>(obj)) {
			QString ava = c->avatar();
			if (ava.isEmpty())
				ava = QLatin1String(":/icons/qutim_64.png");
			QString text = QLatin1Literal("<table><tr><td><b>")
						   % c->name()
						   % QLatin1Literal("</b> &lt;")
						   % c->id()
						   % QLatin1Literal("&gt;<br/>")
						   % c->account()->id()
						   % QLatin1Literal("<br/>")
						   % html(c, true)
						   % QLatin1Literal("</td><td><img width=\"64\" src=\"")
						   % ava
						   % QLatin1Literal("\"/></td></tr>")
						   % QLatin1Literal("</table>");
			QToolTip::showText(pos, text, w);
		} else {
			ToolTip::hideText();
		}
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

	QString ToolTip::html(QObject *object, bool extra)
	{
		ToolTipEvent event(extra);
		qApp->sendEvent(object, &event);
		const InfoFieldList &list = event.d->list;
		QString text;
		foreach (const InfoField &field, list) {
			if (!text.isNull())
				text += QLatin1String("<br/>");
			if (!field.icon.isEmpty()) {
				QString icon = IconLoader::instance()->iconPath(field.icon, 16);
				if (!icon.isEmpty())
					text += "<img src='" + icon + "'> ";
			}
			if (!field.data.canConvert<QString>()) {
				text += QLatin1Literal("<b>")
						% field.name.toString()
						% QLatin1Literal("</b>");
			} else {
				text += QLatin1Literal("<b>")
						% field.name.toString()
						% QLatin1Literal("</b>: ")
						% field.data.toString();
			}
		}
		return text;
	}
}
