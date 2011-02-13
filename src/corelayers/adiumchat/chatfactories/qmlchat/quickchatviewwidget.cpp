/****************************************************************************
 *  quickchatviewwidget.cpp
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

#include "quickchatviewwidget.h"
#include "quickchatviewcontroller.h"

namespace Core {
namespace AdiumChat {

QuickChatViewWidget::QuickChatViewWidget()
{
}

void QuickChatViewWidget::setViewController(QObject* object)
{
	QuickChatViewController* controller = qobject_cast<QuickChatViewController*>(object);
	if (controller) {
		setScene(controller);
	} else {
		setScene(new QGraphicsScene(this));
	}
}


} // namespace AdiumChat
} // namespace Core
