/****************************************************************************
 *  xsettingsdialog.cpp
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include "chatappearance.h"
#include "ui_chatappearance.h"

namespace AdiumChat
{
	
	ChatAppearance::ChatAppearance(): ui(new Ui::chatAppearance)
	{
		ui->setupUi(this);
	}
	
	ChatAppearance::~ChatAppearance()
	{
		delete ui;
	}
	
	void ChatAppearance::cancelImpl()
	{

	}

	void ChatAppearance::loadImpl()
	{

	}

	void ChatAppearance::saveImpl()
	{

	}

}