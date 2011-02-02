/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "textviewwidget.h"
#include "textviewcontroller.h"
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>

namespace Core
{
namespace AdiumChat
{
TextViewWidget::TextViewWidget()
{
//	m_textEdit = new QTextEdit(this);
	setReadOnly(true);

//	setAutoFillBackground(true);
//	QPalette p = palette();
//	p.setColor(QPalette::Base,Qt::white);
//	setPalette(p);
	
//	new QVBoxLayout(this);
//	layout()->addWidget(m_textEdit);
//	layout()->setMargin(0);
////	m_textEdit->installEventFilter(this);
//	setFrameStyle(QFrame::StyledPanel);
//	setFrameShadow(QFrame::Sunken);
//	setReadOnly(true);
//	QVBoxLayout *layout = new QVBoxLayout(this);
//	layout->addWidget(m_textEdit);
//	QSizePolicy policy = sizePolicy();
//	policy.setVerticalPolicy(QSizePolicy::Ignored);
//	setSizePolicy(policy);
//	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void TextViewWidget::setViewController(QObject *object)
{
	TextViewController *controller = qobject_cast<TextViewController*>(document());
	if (controller) {
		controller->setScrollBarPosition(verticalScrollBar()->value());
		controller->setTextEdit(0);
	}
	controller = qobject_cast<TextViewController*>(object);
	if (controller) {
		controller->setTextEdit(this);
		setDocument(controller);
		verticalScrollBar()->setValue(controller->scrollBarPosition());
		QTimer::singleShot(0, controller, SLOT(ensureScrolling()));
//		controller->ensureScrolling();
	} else {
		setDocument(new QTextDocument(this));
	}
}
}
}
