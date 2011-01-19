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

#ifndef TEXTVIEWFACTORY_H
#define TEXTVIEWFACTORY_H

#include <chatlayer/chatviewfactory.h>

namespace Core
{
namespace AdiumChat
{
class TextViewFactory : public ChatViewFactory
{
public:
    TextViewFactory();
	virtual QWidget *createViewWidget();
	virtual QObject *createViewController();
};
}
}

#endif // TEXTVIEWFACTORY_H
