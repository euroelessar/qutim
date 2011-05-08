/*
    UrlPreviewPlugin

  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
  (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#ifndef urlpreviewPLUGIN_H
#define urlpreviewPLUGIN_H
#include <qutim/plugin.h>

namespace qutim_sdk_0_3
{
	class SettingsItem;
}

namespace UrlPreview {

class UrlHandler;
class UrlPreviewPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "UrlPreview")
	Q_CLASSINFO("Uses", "ChatLayer")
	qutim_sdk_0_3::SettingsItem *m_settingsItem;
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
private:
	QWeakPointer<UrlHandler> m_handler;
};
}

#endif
