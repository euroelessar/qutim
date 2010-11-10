/****************************************************************************
*  jmucjoinbookmarksitemdelegate.h
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
#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QAbstractItemDelegate>
#include "libqutim/libqutim_global.h"

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

class LIBQUTIM_EXPORT ItemDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
public:
	ItemDelegate(QObject* parent = 0);
	virtual ~ItemDelegate();
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void setCommandLinkStyle(bool style = true);
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
private:
	QRect checkRect(const QModelIndex& index,const QStyleOptionViewItem& option,const QRect &rect) const;
	QRect checkRect(const QStyleOptionViewItem& option,const QRect &rect) const;
	int m_padding;
	bool m_commandLinkStyle;
	//hacks
	QTreeView *m_treeView;
	QListView *m_listView;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(qutim_sdk_0_3::ItemRoles);

#endif // ITEMDELEGATE_H
