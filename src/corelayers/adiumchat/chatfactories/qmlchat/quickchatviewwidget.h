/****************************************************************************
 *  quickchatviewwidget.h
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

#ifndef QUICKCHATVIEWWIDGET_H
#define QUICKCHATVIEWWIDGET_H
#include <QGraphicsView>
#include <chatlayer/chatviewfactory.h>
#include <QPointer>

namespace Core {
namespace AdiumChat {

class QuickChatViewController;
class QuickChatViewWidget : public QGraphicsView, public Core::AdiumChat::ChatViewWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewWidget)
public:
    QuickChatViewWidget();
    virtual void setViewController(QObject* controller);
protected:
	void resizeEvent(QResizeEvent *event);
	void updateView();
private:
	QPointer<QuickChatViewController> m_controller;
};

} // namespace AdiumChat
} // namespace Core

#endif // QUICKCHATVIEWWIDGET_H
