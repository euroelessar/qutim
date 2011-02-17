/****************************************************************************
 *  quickchatviewfactory.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "quickchatviewfactory.h"
#include "quickchatviewcontroller.h"
#include "quickchatviewwidget.h"
#include <QDeclarativeEngine>

namespace Core {
namespace AdiumChat {

QuickChatViewFactory::QuickChatViewFactory() :
	m_engine(new QDeclarativeEngine(this))
{

}

QObject* QuickChatViewFactory::createViewController()
{
	return new QuickChatViewController(m_engine);
}

QWidget* QuickChatViewFactory::createViewWidget()
{
	return new QuickChatViewWidget();
}

QuickChatViewFactory::~QuickChatViewFactory()
{

}

	
} // namespace AdiumChat
} // namespace Core
