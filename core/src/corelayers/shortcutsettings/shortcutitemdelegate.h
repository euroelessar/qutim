/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#ifndef SHORTCUTITEMDELEGATE_H
#define SHORTCUTITEMDELEGATE_H

#include <QAbstractItemDelegate>
#include <QPushButton>
#include <QMouseEvent>

namespace Core
{

	class ShortcutItemDelegate : public QAbstractItemDelegate
	{
		Q_OBJECT
	public:
		enum Role {
			GroupRole = Qt::UserRole,
			SequenceRole,
			IdRole,
			GlobalRole
		};
		ShortcutItemDelegate(QObject* parent = 0);
		virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
		virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		virtual void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
		virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
		virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
	private:
		virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);
		int m_vertical_padding;
		int m_horizontal_padding;
	};
	
}
#endif // SHORTCUTITEMDELEGATE_H

