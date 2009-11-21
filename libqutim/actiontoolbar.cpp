/****************************************************************************
 *  actiontoolbar.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "actiontoolbar.h"
#include <QAction>

namespace qutim_sdk_0_3
{
	struct ActionToolBarPrivate
	{
		QList<ActionGenerator *> generators;
		QList<QPointer<QAction> > actions;
		QVariant data;
	};

	ActionToolBar::ActionToolBar(const QString &title, QWidget *parent)
			: QToolBar(title, parent), p(new ActionToolBarPrivate)
	{
	}

	ActionToolBar::ActionToolBar(QWidget *parent)
			: QToolBar(parent), p(new ActionToolBarPrivate)
	{
	}

	ActionToolBar::~ActionToolBar()
	{
	}

	QAction *ActionToolBar::addAction(ActionGenerator *generator)
	{
		Q_ASSERT(generator);
		if (int index = p->generators.indexOf(generator))
			return p->actions.at(index);
		QAction *action = generator->generate<QAction>();
		action->setData(p->data);
		p->generators << generator;
		p->actions << action;
		QWidget::addAction(action);
		return action;
	}

	void ActionToolBar::setData(const QVariant &var)
	{
		if (p->data == var)
			return;
		for (int i = 0; i < p->actions.size(); i++) {
			if (p->actions.at(i).isNull()) {
				p->actions.removeAt(i);
				p->generators.removeAt(i);
				i--;
			} else
				p->actions.at(i)->setData(var);
		}
	}

	QVariant ActionToolBar::data() const
	{
		return p->data;
	}

	struct ActionGeneratorPrivate
	{
		QIcon icon;
		QString text;
		QPointer<QObject> receiver;
		const char *member;
		QList<QByteArray> names;
		QList<QVariant>   values;
	};

	ActionGenerator::ActionGenerator(const QIcon &icon, const QString &text, const QObject *receiver, const char *member)
			: p(new ActionGeneratorPrivate)
	{
		Q_ASSERT(receiver&&member&&*member);
		p->icon = icon;
		p->text = text;
		p->receiver = const_cast<QObject *>(receiver);
		p->member = member;
	}

	ActionGenerator::~ActionGenerator()
	{
	}

	QIcon ActionGenerator::icon() const
	{
		return p->icon;
	}

	QString ActionGenerator::text() const
	{
		return p->text;
	}

	ActionGenerator *ActionGenerator::addProperty(const QByteArray &name, const QVariant &value)
	{
		int index = p->names.indexOf(name);
		if (index != -1) {
			p->values[index] = value;
		} else {
			p->names.append(name);
			p->values.append(value);
		}
		return this;
	}

	QAction *ActionGenerator::prepareAction(QAction *action) const
	{
		if (p->receiver.isNull()) {
			action->deleteLater();
			return NULL;
		}
		action->setParent(p->receiver);
		action->setIcon(p->icon);
		action->setText(p->text);
		for (int i = 0; i < p->names.size(); i++)
			action->setProperty(p->names.at(i), p->values.at(i));
		QObject::connect(action, SIGNAL(triggered()), p->receiver, p->member);
		return action;
	}

	QObject *ActionGenerator::generateHelper() const
	{
		return prepareAction(new QAction(NULL));
	}

	const QMetaObject *ActionGenerator::metaObject() const
	{
		return &QAction::staticMetaObject;
	}

	bool ActionGenerator::hasInterface(const char *id) const
	{
		Q_UNUSED(id);
		return false;
	}
}
