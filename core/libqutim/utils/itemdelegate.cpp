/****************************************************************************
*  jmucjoinItemDelegate.cpp
*
*  Copyright (c) 2010 by Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "../../libqutim/debug.h"
#include "itemdelegate.h"
#include <QPainter>
#include <QApplication>
#include <QLatin1Literal>
#include <qtextlayout.h>
#include "../../libqutim/localizedstring.h"
#include <QTreeView>
#include <QListView>
#include <QStringBuilder>

namespace qutim_sdk_0_3
{

class ItemDelegatePrivate
{
public:
	QRect checkRect(const QModelIndex& index,const QStyleOptionViewItem& option,const QRect &rect) const;
	QRect checkRect(const QStyleOptionViewItem& option,const QRect &rect) const;
	int padding;
	bool commandLinkStyle;
	bool editorSupport;
};

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

QString description(const QModelIndex& index, QFontMetrics metrics, int width = 0, int firstLineWidth = 0)
{
	QVariant data = index.data(DescriptionRole);
	QString desc = data.toString();
	if (data.canConvert<QVariantMap>()) {
		QVariantMap fields = data.toMap();
		QVariantMap::const_iterator it;
		for (it = fields.constBegin(); it != fields.constEnd(); ++it) {
			QString newLine = it.key() % QLatin1Literal(": ") % it.value().toString();
			if (width) {
				newLine = metrics.elidedText(newLine, Qt::ElideRight, firstLineWidth);
				firstLineWidth = width;
			}
			desc += newLine % QLatin1Literal(" \n");
		}
		desc.remove(desc.length()-2,2); //remove last \n
	}
	return desc;
};

ItemDelegate::ItemDelegate(QObject* parent):
	QAbstractItemDelegate(parent),d_ptr(new ItemDelegatePrivate)
{
	Q_D(ItemDelegate);
	d->padding = 6;
	d->commandLinkStyle = false;
	d->editorSupport = false;
}

ItemDelegate::~ItemDelegate()
{

}

void ItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
						 const QModelIndex& index) const
{
	Q_D(const ItemDelegate);
	painter->setClipping(false); //spike
	QStyleOptionViewItemV4 opt(option);
	QStyle *style = getStyle(opt);

	QString title = index.data(Qt::DisplayRole).toString();

	if (isSeparator(index)) {
		opt.features &= ~QStyleOptionViewItemV2::Alternate;
		painter->drawText(option.rect, Qt::AlignCenter, title);
	}
	else if (isTitle(index)) {
		QStyleOptionButton buttonOption;

		buttonOption.state = option.state;
#ifdef Q_OS_MAC
		buttonOption.features = QStyleOptionButton::Flat;
		buttonOption.state |= QStyle::State_Raised;
		buttonOption.state &= ~QStyle::State_HasFocus;
#endif

		buttonOption.rect = option.rect;
		buttonOption.palette = option.palette;
		style->drawControl(QStyle::CE_PushButton, &buttonOption, painter, opt.widget);
		QRect rect = option.rect;
		rect.adjust(d->padding,0,0,0);
		if (const QTreeView *view = qobject_cast<const QTreeView*>(getWidget(option))) {
			QStyleOptionViewItemV2 branchOption;
			static const int i = 9; // ### hardcoded in qcommonstyle.cpp
			QRect r = option.rect;
			branchOption.rect = QRect(r.left() + i/2, r.top() + (r.height() - i)/2, i, i);
			branchOption.palette = option.palette;
			branchOption.state = QStyle::State_Children;
			rect.adjust(branchOption.rect.width() + d->padding,
						branchOption.rect.height() + d->padding,
						0,0);
			if (view->isExpanded(index))
				branchOption.state |= QStyle::State_Open;
			if (option.state & QStyle::State_MouseOver)
				 branchOption.state |= QStyle::State_MouseOver;
			style->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, view);
		}
		QFont font = opt.font;
		painter->save();
		font.setBold(true);
		painter->setFont(font);
		painter->drawText(rect,
						  Qt::AlignVCenter,
						  title
						  );
		painter->restore();
	}
	else {
		drawFocus(painter,opt,option.rect);

		QRect rect = option.rect;
		rect.adjust(d->padding,d->padding,0,0);

		QVariant value = index.data(Qt::CheckStateRole);
		if (value.isValid()) {
			Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
			QRect checkRect = drawCheck(painter,option,rect,state);
			rect.adjust(checkRect.width() + d->padding,0,0,0);
		}

		QIcon item_icon = index.data(Qt::DecorationRole).value<QIcon>();
		item_icon.paint(painter,
						rect.left(),
						rect.top(),
						option.decorationSize.width(),
						option.decorationSize.height(),
						Qt::AlignTop);
		rect.adjust(d->padding + option.decorationSize.width(),0,0,0);
		rect.setBottom(rect.bottom() - d->padding);

		painter->save();
		QFont title_font = opt.font;
		title_font.setBold(true);
		QFont description_font = opt.font;
		description_font.setPointSize(opt.font.pointSize() - 1);

		int titleWidth = rect.width() - getEditorSize(getWidget(opt), index).width();
		title = QFontMetrics(title_font).elidedText(title, Qt::ElideRight, titleWidth);

		QRect bounding;
		QString desc = description(index, QFontMetrics(description_font), rect.width(), titleWidth);
		painter->setFont(title_font);
		painter->drawText(rect,
						  (desc.isEmpty() ? Qt::AlignVCenter : Qt::AlignTop) | Qt::AlignLeft,
						  title,
						  &bounding);

		if (!desc.isEmpty()) {
			rect.adjust(0,bounding.height() + 0.5*d->padding,0,0);
			painter->setFont(description_font);
			painter->drawText(rect, Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap, desc);
		}

		painter->restore();
	}
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_D(const ItemDelegate);
	QVariant value = index.data(Qt::SizeHintRole);
	if (value.isValid())
		return value.toSize();

	const QWidget *widget = getWidget(option);
	//add event filter for any itemView's that use this delegate
	const_cast<QWidget*>(widget)->installEventFilter(const_cast<ItemDelegate*>(this));
	//HACK black magic
	//fix trouble with sizeHint change
	QRect rect = option.rect;
	if (!rect.isValid()) {
		if (const QAbstractScrollArea *area = qobject_cast<const QAbstractScrollArea*>(widget))
			rect = area->viewport()->geometry();
		else
			rect = widget->geometry();
	}
	rect.adjust(d->padding, 0, 0, 0);

	QRect check = d->checkRect(index, option, rect);
	if (check.isValid())
		rect.adjust(check.width() + d->padding, 0, 0, 0);
	rect.adjust(d->padding + option.decorationSize.width(), 0, 0, 0);

	QFontMetrics metrics = option.fontMetrics;
	if (!isSeparator(index)) {
		QFont font = option.font;
		font.setBold(true);
		metrics = QFontMetrics(font);
	}

	QString desc = description(index,
							   metrics,
							   rect.width(),
							   rect.width() - getEditorSize(widget, index).width());
	QRect bounding = metrics.boundingRect(rect,
										  (desc.isEmpty() ? Qt::AlignVCenter : Qt::AlignTop) | Qt::AlignLeft,
										  index.data(Qt::DisplayRole).toString());
	int height = bounding.height();

	if (!isSeparator(index) && !desc.isEmpty() && !isTitle(index)) {
		QFont desc_font = option.font;
		desc_font.setPointSize(desc_font.pointSize()-1);
		metrics = QFontMetrics(desc_font);

		bounding = metrics.boundingRect(rect,
										Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap,
										desc);
		height += bounding.height();
		height += 3*d->padding;
	}
	QSize size (rect.width(),qMax(option.decorationSize.height() + 2*d->padding,height));
	return size;
}

QRect ItemDelegatePrivate::checkRect(const QModelIndex& index,const QStyleOptionViewItem &o, const QRect &rect) const
{
	QVariant value = index.data(Qt::CheckStateRole);
	return value.isValid() ? checkRect(o,rect) : QRect();
}

QRect ItemDelegatePrivate::checkRect(const QStyleOptionViewItem &o, const QRect &rect) const
{
	QStyleOptionViewItemV4 option(o);
	QStyleOptionButton opt;
	opt.QStyleOption::operator=(option);
	opt.rect = rect;
	const QWidget *widget = getWidget(option);
	QStyle *style = getStyle(option);
	QRect checkRect = style->subElementRect(QStyle::SE_ViewItemCheckIndicator, &opt, widget);
	return checkRect;
}

void ItemDelegate::setCommandLinkStyle(bool style)
{
	d_func()->commandLinkStyle = style;
}

bool ItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
							   const QStyleOptionViewItem& option, const QModelIndex& index)
{
	//ported from Qt sources
	Q_D(ItemDelegate);
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
		rect.adjust(d->padding,d->padding,0,0);
		QRect check = d->checkRect(index,option, rect);
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
	if (d_func()->commandLinkStyle) {
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
	checkOption.rect = d_func()->checkRect(option, rect);
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

void ItemDelegate::setUserDefinedEditorSupport(bool support)
{
	d_func()->editorSupport = support;
}

QSize ItemDelegate::getEditorSize(const QWidget *widget, const QModelIndex &index) const
{
	if (d_func()->editorSupport) {
		if (const QAbstractItemView *view = qobject_cast<const QAbstractItemView*>(widget))
			if (QWidget *editor = view->indexWidget(index))
				return editor->size();
	}
	return QSize();
}

bool ItemDelegate::eventFilter(QObject *obj, QEvent *event)
{
	Q_UNUSED(obj);
	if(event->type() == QEvent::Resize || event->type() == QEvent::Show) {
		//update sizeHint cache on itemViews, because item view's doesn't update it
		//when changing the word wrapping
		QEvent e(QEvent::StyleChange);
		qApp->sendEvent(obj,&e);
	}
	return QAbstractItemDelegate::eventFilter(obj,event);
}

void ItemDelegate::updateEditorGeometry(QWidget *editor,
										const QStyleOptionViewItem &option,
										const QModelIndex &index) const
{
	Q_UNUSED(index);
	Q_D(const ItemDelegate);
	QRect rect(option.rect.x() + option.rect.width() - editor->sizeHint().width() - d->padding,
			   option.rect.y() + d->padding,
			   d->padding + editor->sizeHint().width(),
			   option.rect.height() - d->padding);
	editor->setGeometry(rect);
}

}
