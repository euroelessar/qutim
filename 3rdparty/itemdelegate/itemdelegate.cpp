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

namespace qutim_sdk_0_3
{

//small hack from Qt sources
bool isSeparator(const QModelIndex &index) {
	return index.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String("separator") || index.data(SeparatorRole).toBool();
}

bool isTitle(const QModelIndex &index)
{
	return index.data(TitleRole).toBool();
}

const QWidget *getWidget(const QStyleOptionViewItem &option)
{
	if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
		return v3->widget;

	return 0;
}

QStyle *getStyle(const QStyleOptionViewItem& option)
{
	if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
		return v3->widget ? v3->widget->style() : QApplication::style();

	return QApplication::style();
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
		m_padding(6),m_command_link_style(false),
		m_tree_view(0)
{
	m_tree_view = qobject_cast<QTreeView *>(parent);
}

ItemDelegate::~ItemDelegate()
{

}

void ItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
						 const QModelIndex& index) const
	{
	QStyleOptionViewItemV4 opt(option);
	QStyle *style = getStyle(opt);

	QString title = index.data(Qt::DisplayRole).toString();
	//fix trouble with localized strings
	if (title.isEmpty())
		title = index.data(Qt::DisplayRole).value<LocalizedString>();

	if (isSeparator(index)) {
		opt.features &= ~QStyleOptionViewItemV2::Alternate;
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
			rect.adjust(m_padding,0,0,0);
			if (m_tree_view) {
				QStyleOption branchOption;
				static const int i = 9; // ### hardcoded in qcommonstyle.cpp
				QRect r = option.rect;
				branchOption.rect = QRect(r.left() + i/2, r.top() + (r.height() - i)/2, i, i);
				branchOption.palette = option.palette;
				branchOption.state = QStyle::State_Children;
				rect.adjust(branchOption.rect.width() + m_padding,
							branchOption.rect.height() + m_padding,
							0,0);
				if (m_tree_view->isExpanded(index))
					branchOption.state |= QStyle::State_Open;
				style->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, m_tree_view);
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
		drawFocus(painter,opt,option.rect);

		QRect rect = option.rect;
		rect.adjust(m_padding,m_padding,0,0);

		QVariant value = index.data(Qt::CheckStateRole);
		if (value.isValid()) {
			Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
			QRect checkRect = drawCheck(painter,option,rect,state);
			rect.adjust(checkRect.width(),0,0,0);
		}

		rect.adjust(m_padding,0,0,0);
		QIcon item_icon = index.data(Qt::DecorationRole).value<QIcon>();
		item_icon.paint(painter,
						rect.left(),
						rect.top(),
						option.decorationSize.width(),
						option.decorationSize.height(),
						Qt::AlignTop);
		rect.adjust(m_padding + option.decorationSize.width(),0,0,0);
		rect.setBottom(rect.bottom() - m_padding);

		const QFont orig_font = painter->font();
		const QPen orig_pen = painter->pen();
		QFont title_font = orig_font;
		title_font.setBold(true);
		painter->setFont(title_font);
		QRect bounding;
		QString desc = description(index);
		painter->drawText(rect,
						  (desc.isEmpty() ? Qt::AlignVCenter : Qt::AlignTop) | Qt::AlignLeft,
						  title,
						  &bounding);
		painter->setFont(orig_font);
		painter->setPen(orig_pen);

		if (!desc.isEmpty()) {
			rect.adjust(0,bounding.height() + 0.5*m_padding,0,0);
			QFont description_font = orig_font;
			description_font.setPointSize(orig_font.pointSize() - 1);
			painter->setFont(description_font);
			painter->drawText(rect, Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap, desc);
		}
		painter->setFont(orig_font);
		painter->setPen(orig_pen);
	}
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QVariant value = index.data(Qt::SizeHintRole);
	if (value.isValid())
		return value.toSize();

	const QWidget *widget = getWidget(option);
	QRect rect = widget->geometry();
	rect.adjust(m_padding,0,0,0);

	//hack for null width with tree view
	//TODO need rewrite
	if (!rect.isValid() && m_tree_view) {
		rect.setWidth(m_tree_view->viewport()->size().width() - m_tree_view->indentation());
		//mega hack for intendation
		//--root----------------
		//  ^intendation
		//^size().width()------^
		//----first element
		//    ^intendation
		QModelIndex p = index.parent();
		while (p.isValid()) {
			rect.setWidth(rect.width()-m_tree_view->indentation());
			p = p.parent();
		}
	}

	QRect check = checkRect(index,option,rect);
	if (check.isValid())
		rect.adjust(check.width()+m_padding,0,0,0);
	rect.adjust(2*m_padding + option.decorationSize.width(),0,0,0);

	QFontMetrics metrics = option.fontMetrics;
	if (!isSeparator(index)) {
		metrics = option.fontMetrics;
		QFont font = option.font;
		font.setBold(true);
		metrics = QFontMetrics(font);
	}

	QRect bounding = metrics.boundingRect(rect, Qt::AlignTop | Qt::AlignLeft,
									  index.data(Qt::DisplayRole).toString());
	int height = bounding.height();

	QString desc = description(index);
	if (!isSeparator(index) && !desc.isEmpty() && !isTitle(index)) {
		QFont desc_font = option.font;
		desc_font.setPointSize(desc_font.pointSize()-1);
		metrics = QFontMetrics(desc_font);

		bounding = metrics.boundingRect(rect,
									   Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap,
									   desc
									   );
		height += bounding.height();
		height += 2.5*m_padding;
	}
	QSize size (rect.width(),qMax(option.decorationSize.height() + 2*m_padding,height));
	return size;
}

QRect ItemDelegate::checkRect(const QModelIndex& index,const QStyleOptionViewItem &o, const QRect &rect) const
{
	QVariant value = index.data(Qt::CheckStateRole);
	if (!value.isValid())
		return QRect();
	else
		return checkRect(o,rect);
}

QRect ItemDelegate::checkRect(const QStyleOptionViewItem &o, const QRect &rect) const
{
	QStyleOptionViewItemV4 option(o);
	QSize size = option.decorationSize;
	QStyleOptionButton opt;
	opt.QStyleOption::operator=(option);
	opt.rect = rect;
	const QWidget *widget = getWidget(option);
	QStyle *style = getStyle(option);
	QRect checkRect = style->subElementRect(QStyle::SE_ViewItemCheckIndicator, &opt, widget);
//TODO need to testing on some platforms
//	QRect result (rect.left(),
//		  rect.top() + (size.height() - checkRect.height())/2,
//		  size.width() - checkRect.width(),
//		  checkRect.height()
//		  );
	return checkRect;
}

void ItemDelegate::setCommandLinkStyle(bool style)
{
	m_command_link_style = style;
}

bool ItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
							   const QStyleOptionViewItem& option, const QModelIndex& index)
{
	//ported from Qt sources
	Q_ASSERT(event);
	Q_ASSERT(model);

	// make sure that the item is checkable
	Qt::ItemFlags flags = model->flags(index);
	if (!(flags & Qt::ItemIsUserCheckable) || !(option.state & QStyle::State_Enabled)
		|| !(flags & Qt::ItemIsEnabled))
		return false;

	// make sure that we have a check state
	QVariant value = index.data(Qt::CheckStateRole);
	if (!value.isValid())
		return false;

	// make sure that we have the right event type
	if ((event->type() == QEvent::MouseButtonRelease)
		|| (event->type() == QEvent::MouseButtonDblClick)) {
		QRect rect = option.rect;
		rect.adjust(m_padding,m_padding,0,0);
		QRect check = checkRect(index,option, rect);
		QMouseEvent *me = static_cast<QMouseEvent*>(event);
		if (me->button() != Qt::LeftButton || !check.contains(me->pos()))
			return false;

		// eat the double click events inside the check rect
		if (event->type() == QEvent::MouseButtonDblClick)
			return true;

	} else if (event->type() == QEvent::KeyPress) {
		if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space
		&& static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
			return false;
	} else {
		return false;
	}

	Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked
							? Qt::Unchecked : Qt::Checked);
	return model->setData(index, state, Qt::CheckStateRole);
}

void ItemDelegate::drawFocus(QPainter *painter,
							 const QStyleOptionViewItem &option,
							 const QRect &rect) const
{
	Q_UNUSED(rect);
	QStyle *style = getStyle(option);
	const QWidget *widget = getWidget(option);
	if (m_command_link_style) {
		QStyleOptionButton buttonOption;

		buttonOption.state = option.state;
		buttonOption.rect = option.rect;
		buttonOption.palette = option.palette;
		buttonOption.features |= QStyleOptionButton::CommandLinkButton;

		style->drawControl(QStyle::CE_PushButton, &buttonOption, painter, widget);
	}
	else
		style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, widget);
}

QRect ItemDelegate::drawCheck(QPainter *painter,
							 const QStyleOptionViewItem &option, const QRect &rect,
							 Qt::CheckState state) const
{
	QStyleOptionViewItem checkOption(option);
	checkOption.state = checkOption.state & ~QStyle::State_HasFocus;
	checkOption.rect = checkRect(option,rect);
	switch (state) {
	case Qt::Unchecked:
		checkOption.state |= QStyle::State_Off;
		break;
	case Qt::PartiallyChecked:
		checkOption.state |= QStyle::State_NoChange;
		break;
	case Qt::Checked:
		checkOption.state |= QStyle::State_On;
		break;
	}
	getStyle(option)->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &checkOption, painter,getWidget(option));
	return checkOption.rect;
}

}
