#include "contactdelegate.h"
#include <libqutim/buddy.h>
#include <libqutim/tooltip.h>
#include <QApplication>
#include <QHelpEvent>
#include <QAbstractItemView>

namespace qutim_sdk_0_3
{

ContactDelegate::ContactDelegate(QObject *parent) :
		QStyledItemDelegate(parent)
{
}

void ContactDelegate::paint(QPainter *painter,
							const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_ASSERT(index.isValid());

	QStyleOptionViewItemV4 opt = option;
	initStyleOption(&opt, index);
	opt.features |= QStyleOptionViewItemV2::WrapText;

	const QWidget *widget = opt.widget;
	QStyle *style = widget ? widget->style() : QApplication::style();
	style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

QSize ContactDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QVariant value = index.data(Qt::SizeHintRole);
	if (value.isValid())
		return qvariant_cast<QSize>(value);

	QStyleOptionViewItemV4 opt = option;
	initStyleOption(&opt, index);
	const QWidget *widget = opt.widget;
	QStyle *style = widget ? widget->style() : QApplication::style();
	return style->sizeFromContents(QStyle::CT_ItemViewItem, &opt, QSize(), widget);
}


bool ContactDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view,
										const QStyleOptionViewItem &option,
										const QModelIndex &index)
{
	if (event->type() == QEvent::ToolTip) {
		Buddy *buddy = index.data(Qt::UserRole).value<Buddy*>();
		if (buddy)
			ToolTip::instance()->showText(event->globalPos(), buddy, view);
		return true;
	} else {
		return QAbstractItemDelegate::helpEvent(event, view, option, index);
	}
}

}
