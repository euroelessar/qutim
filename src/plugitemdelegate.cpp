/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "plugitemdelegate.h"

plugItemDelegate::plugItemDelegate(QObject* parent) {
	
}


QSize plugItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {

}

void plugItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {

}

QWidget* plugItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {

}

void plugItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
QAbstractItemDelegate::setEditorData(editor, index);
}

void plugItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
QAbstractItemDelegate::setModelData(editor, model, index);
}

void plugItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
QAbstractItemDelegate::updateEditorGeometry(editor, option, index);
}

