/****************************************************************************
 *  jmucjoinItemDelegate.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include <libqutim/debug.h>
#include "itemdelegate.h"
#include <QPainter>
#include <QApplication>
#include <QLatin1Literal>
#include <qtextlayout.h>
#include <libqutim/localizedstring.h>
#include <QTreeView>

namespace Core
{
	using namespace qutim_sdk_0_3;

	//small hack from Qt sources
	bool isSeparator(const QModelIndex &index) {
		return index.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String("separator") || index.data(SeparatorRole).toBool();
	}

	bool isTitle(const QModelIndex &index)
	{
		return index.data(TitleRole).toBool();
	}

	QString description(const QModelIndex& index)
	{
		QVariant data = index.data(DescriptionRole);
		QString desc = data.toString();
		if (data.canConvert<LocalizedString>()) {
			desc = data.value<LocalizedString>();
		}else if (data.canConvert<QVariantMap>()) {
			QVariantMap fields = data.toMap();
			QVariantMap::const_iterator it;
			for (it = fields.constBegin();it!=fields.constEnd();it++) {
				desc += it.key() % QLatin1Literal(": ") % it.value().toString() % QLatin1Literal(" \n");
			}
			desc.remove(desc.length()-2,2); //remove last \n
		}
		return desc;
	};

	ItemDelegate::ItemDelegate(QObject* parent):
			QAbstractItemDelegate(parent),
			m_padding(6),m_command_link_style(false)
	{

	}

	ItemDelegate::~ItemDelegate()
	{

	}

	void ItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
							 const QModelIndex& index) const
	{
		QStyleOptionViewItemV4 opt(option);
		QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
		QString title = index.data(Qt::DisplayRole).toString();
		//fix trouble with localized strings
		if (title.isEmpty())
			title = index.data(Qt::DisplayRole).value<LocalizedString>();

		if (isSeparator(index)) {
			painter->drawText(option.rect, Qt::AlignCenter, title);
		}
		else if (isTitle(index)) {
				QStyleOptionButton buttonOption;

				buttonOption.state = option.state;
#ifdef Q_WS_MAC
				buttonOption.features = QStyleOptionButton::Flat;
				buttonOption.state |= QStyle::State_Raised;
				buttonOption.state &= ~QStyle::State_HasFocus;
#endif

				buttonOption.rect = option.rect;
				buttonOption.palette = option.palette;
				style->drawControl(QStyle::CE_PushButton, &buttonOption, painter, opt.widget);
				QRect rect = option.rect;
				rect.translate(m_padding,0);
				QTreeView *view = qobject_cast<QTreeView *>(parent());
				if (view) {
					QStyleOption branchOption;
					static const int i = 9; // ### hardcoded in qcommonstyle.cpp
					QRect r = option.rect;
					branchOption.rect = QRect(r.left() + i/2, r.top() + (r.height() - i)/2, i, i);
					branchOption.palette = option.palette;
					branchOption.state = QStyle::State_Children;
					rect.translate(branchOption.rect.width() + m_padding,
								   branchOption.rect.height() + m_padding);
					if (view->isExpanded(index))
						branchOption.state |= QStyle::State_Open;
					style->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, view);
				}
				QFont font = opt.font;
				const QFont orig_font = font;
				font.setBold(true);
				painter->setFont(font);
				painter->drawText(rect,
								  Qt::AlignVCenter,
								  title
								  );
				painter->setFont(orig_font);

		}
		else {
			if (m_command_link_style) {
				QStyleOptionButton buttonOption;

				buttonOption.state = option.state;
				buttonOption.rect = option.rect;
				buttonOption.palette = option.palette;
				buttonOption.features |= QStyleOptionButton::CommandLinkButton;

				style->drawControl(QStyle::CE_PushButton, &buttonOption, painter, opt.widget);
			}
			else {
#ifndef Q_OS_SYMBIAN
				//disable very ugly appearance
				style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
#endif
			}
			QRect rect = option.rect;
			rect.translate(2*m_padding + option.decorationSize.width(),m_padding);
			rect.setBottom(rect.bottom() - m_padding);

			const QFont orig_font = painter->font();
			const QPen orig_pen = painter->pen();
			QFont title_font = orig_font;
			title_font.setBold(true);
			painter->setFont(title_font);
			QRect bounding;
			painter->drawText(rect, Qt::AlignTop | Qt::AlignLeft, title,&bounding);
			painter->setFont(orig_font);
			painter->setPen(orig_pen);

			QString desc = description(index);
			rect.translate(0,bounding.height() + 0.5*m_padding);
			QFont description_font = orig_font;
			description_font.setPointSize(orig_font.pointSize() - 1);
			painter->setFont(description_font);
			painter->drawText(rect, Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap, desc);
			painter->setFont(orig_font);
			painter->setPen(orig_pen);

			QIcon item_icon = index.data(Qt::DecorationRole).value<QIcon>();
			item_icon.paint(painter,
							option.rect.left() + m_padding,
							option.rect.top() + m_padding,
							option.decorationSize.width(),
							option.decorationSize.height(),
							Qt::AlignTop);
		}
	}

	QSize ItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QRect rect = option.rect;
		int width = rect.width();
		rect.setLeft(rect.left() + 2*m_padding + option.decorationSize.width());

		QFontMetrics metrics = option.fontMetrics;
		int height = metrics.boundingRect(rect, Qt::TextWordWrap,
										  index.data(Qt::DisplayRole).toString()).height();

		QString desc = description(index);
		if (!isSeparator(index) && !desc.isEmpty() && !isTitle(index)) {
			QFont desc_font = option.font;
			desc_font.setPointSize(desc_font.pointSize() - 1);
			metrics = QFontMetrics(desc_font);

			height += metrics.boundingRect(rect,
										   Qt::TextWordWrap,
										   desc
										   ).height();
			height += 1.5*m_padding;
		}
		height += m_padding;
		QSize size (width,height);
		return size;
	}

	void ItemDelegate::setCommandLinkStyle(bool style)
	{
		m_command_link_style = style;
	}
}
