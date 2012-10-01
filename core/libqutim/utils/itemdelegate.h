/****************************************************************************
*  jmucjoinbookmarksitemdelegate.h
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
#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QAbstractItemDelegate>
#include "libqutim_global.h"

class QTreeView;
class QListView;
namespace qutim_sdk_0_3
{
enum ItemRole
{
	DescriptionRole = Qt::UserRole + 33,
	SeparatorRole,
	TitleRole, //NOTE: in title and separator description and decoration roles will be ignored
	ActionRole //TODO
};

Q_DECLARE_FLAGS(ItemRoles,ItemRole);

class ItemDelegatePrivate;
class LIBQUTIM_EXPORT ItemDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ItemDelegate)
public:
	ItemDelegate(QObject* parent = 0);
	virtual ~ItemDelegate();
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void setCommandLinkStyle(bool style = true);
	void setUserDefinedEditorSupport(bool support = true);
	virtual void updateEditorGeometry(QWidget *editor,
									  const QStyleOptionViewItem &option,
									  const QModelIndex &index) const;
protected:
	//TODO split paint to set of virtual functions
	virtual bool editorEvent(QEvent* event, QAbstractItemModel* model,
							 const QStyleOptionViewItem& option, const QModelIndex& index);
	void drawFocus(QPainter *painter,
				   const QStyleOptionViewItem &option,
				   const QRect &rect) const;
	QRect drawCheck(QPainter *painter,
				   const QStyleOptionViewItem &option,
				   const QRect &rect, Qt::CheckState state) const;
	QSize getEditorSize(const QWidget *widget, const QModelIndex &index) const;
	virtual bool eventFilter(QObject *obj, QEvent *event);
private:
	QScopedPointer<ItemDelegatePrivate> d_ptr;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(qutim_sdk_0_3::ItemRoles);

#endif // ITEMDELEGATE_H
