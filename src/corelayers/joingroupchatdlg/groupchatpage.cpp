/****************************************************************************
 *  groupchatpage.cpp
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

#include "groupchatpage.h"
#include <QVBoxLayout>

namespace Core {

GroupChatPage::GroupChatPage(QWidget *parent) :
    QScrollArea(parent)
{
	QWidget *widget = new QWidget(this);
	setWidget(widget);
	setWidgetResizable(true);
	m_layout = new QVBoxLayout(widget);
	m_layout->setMargin(0);
	setFrameShape(QAbstractScrollArea::NoFrame);
}

} // namespace Core
