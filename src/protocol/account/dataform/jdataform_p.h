/****************************************************************************
 *  jdataform_p.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                     by Denis Daschenko <daschenko@gmail.com>
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

#ifndef JDATAFORM_P_H
#define JDATAFORM_P_H

#include "jdataform.h"
#include "../../jjidvalidator.h"
#include <QStyledItemDelegate>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QTextEdit>

namespace Jabber
{
	class JDataFormJidMultiItemDelegate : public QStyledItemDelegate
	{
		Q_OBJECT
	public:
		explicit JDataFormJidMultiItemDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}
		virtual ~JDataFormJidMultiItemDelegate() {}

		QWidget *createEditor(QWidget *parent,
							  const QStyleOptionViewItem &,
							  const QModelIndex &) const
		{
			QLineEdit * const line = new QLineEdit(parent);
			line->setValidator(new JJidValidator(gloox::EmptyString, line));
			return line;
		}
	};

	class JDataFormJidMulti : public QListWidget
	{
		Q_OBJECT
	public:
		JDataFormJidMulti(QWidget *parent);
		virtual ~JDataFormJidMulti() {}

		QLayout *localLayout() { return layout; }
	protected:
		void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	private slots:
		void jidEditTextChanged(const QString &jid);
		void addButtonClicked();
		void removeButtonClicked();
	private:
		QLineEdit *jidEdit;
		QPushButton *addButton;
		QPushButton *removeButton;
		QGridLayout *layout;
	};
}

#endif // JDATAFORM_P_H
