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

#include "urlpreview.h"
#include <qutim/debug.h>
#include <qutim/config.h>
#include "messagehandler.h"

namespace UrlPreview
{

using namespace qutim_sdk_0_3;

void UrlPreviewPlugin::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "UrlPreview"),
			QT_TRANSLATE_NOOP("Plugin", "Preview images directly in the chat window"),
			PLUGIN_VERSION(0, 1, 0, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author","Alexander Kazarin"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("boiler@co.ru"));
	addAuthor(QT_TRANSLATE_NOOP("Author","Sidorov Aleksey"),
			  QT_TRANSLATE_NOOP("Task","Developer"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
}

bool UrlPreviewPlugin::load()
{
	if (!m_handler)
		m_handler = new UrlHandler;
	qutim_sdk_0_3::MessageHandler::registerHandler(m_handler,
												   qutim_sdk_0_3::MessageHandler::HighPriority,
												   qutim_sdk_0_3::MessageHandler::HighPriority);
	return true;
}

bool UrlPreviewPlugin::unload()
{
	if (m_handler) {
		m_handler->unregisterHandler(m_handler);
		m_handler->deleteLater();
		return true;
	}
	return false;
}

}

QUTIM_EXPORT_PLUGIN(UrlPreview::UrlPreviewPlugin);
