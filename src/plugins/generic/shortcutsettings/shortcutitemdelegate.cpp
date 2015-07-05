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
#include "shortcutitemdelegate.h"
#include <QApplication>
#include <QPainter>
#include <QPushButton>
#include <QMouseEvent>
#include "keysequencewidget.h"

namespace Core
{

ShortcutItemDelegate::ShortcutItemDelegate(QObject* parent):
	QAbstractItemDelegate(parent),
	m_vertical_padding(6),
	m_horizontal_padding(9)
{

}

void ShortcutItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyleOptionViewItemV4 opt(option);
	QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
	style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

	QIcon item_icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));

	QIcon::Mode mode = QIcon::Normal;

	//Qt::CheckState state = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());

	QFont painter_font = painter->font();

	bool group = index.data(Qt::UserRole).toBool();

	if (group) {
		QFont font = painter_font;
		font.setBold(true);
		painter->setFont(font);

		QStyleOptionButton buttonOption;
		buttonOption.state = option.state;
#ifdef Q_OS_MAC
		buttonOption.state |= QStyle::State_Raised;
		buttonOption.features = QStyleOptionButton::Flat;
		buttonOption.state &= ~QStyle::State_HasFocus;
#endif

		buttonOption.rect = option.rect;
		buttonOption.palette = option.palette;
		style->drawControl(QStyle::CE_PushButton, &buttonOption, painter, opt.widget);
	}

	item_icon.paint(painter,
					option.rect,
					Qt::AlignVCenter,
					mode);

	QString name = index.data(Qt::DisplayRole).toString();

	if (index.data(GlobalRole).toBool())
		name.append(tr(" (global)"));

	QRect rect = option.rect;
	int decoration_width = item_icon.isNull() ? 0 : option.decorationSize.width();
	rect.setX(rect.x() + decoration_width + m_horizontal_padding);
	painter->drawText(rect,
					  Qt::AlignVCenter | Qt::AlignLeft,
					  name
					  );

	painter->setFont(painter_font);

	QKeySequence sequence = index.data(SequenceRole).value<QKeySequence>();

	rect.setWidth(rect.width() - m_horizontal_padding);
	painter->drawText(rect,
					  Qt::AlignVCenter | Qt::AlignRight,
					  sequence.toString()
					  );

}

QSize ShortcutItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(index);
	QSize size(option.decorationSize);
	size += QSize(0,m_vertical_padding);
	return size;
}


bool ShortcutItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	//		if (event->type() == QEvent::MouseButtonPress) {
	//		}
	return QAbstractItemDelegate::editorEvent(event, model, option, index);
}

QWidget *ShortcutItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
	KeySequenceWidget *input_btn = new KeySequenceWidget(parent);
	input_btn->setText(tr("%1 : input").arg(index.data(Qt::DisplayRole).toString()));
	input_btn->captureSequence();
	return input_btn;
}

void ShortcutItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
	editor->setGeometry(option.rect);
}

void ShortcutItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	KeySequenceWidget *button = static_cast<KeySequenceWidget *>(editor);
	button->setText(index.data(SequenceRole).value<QKeySequence>().toString());
	QAbstractItemDelegate::setEditorData(editor, index);
}

void ShortcutItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	KeySequenceWidget *button = static_cast<KeySequenceWidget *>(editor);
	model->setData(index,button->sequence(),SequenceRole);
}

}

