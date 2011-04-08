/*

    Copyright (c) 2011 by Stanislav (proDOOMman) Kosolapov <prodoomman@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef juickPLUGIN_H
#define juickPLUGIN_H
#include <qutim/plugin.h>
#include <qutim/settingslayer.h>
#include <QTextDocument>
#include <QMessageBox>
#include "juicksettings.h"

namespace qutim_sdk_0_3 {
	class ChatUnit;
	class ChatSession;
	class Message;
}

namespace Juick {
	
	using namespace qutim_sdk_0_3;
	
	class JuickPlugin : public Plugin
	{
		Q_OBJECT
		Q_CLASSINFO("DebugName", "Juick")
		Q_CLASSINFO("Uses", "ChatLayer")
		Q_CLASSINFO("Uses", "SettingsLayer")
	public:
		virtual void init();
		virtual bool load();
		virtual bool unload();
	private slots:
		void sessionCreated(qutim_sdk_0_3::ChatSession*);
		void processMessage(qutim_sdk_0_3::Message*);
		void loadSettings();
	private:
		bool m_user_bold, m_user_italic, m_user_underline;
		bool m_msg_bold, m_msg_italic, m_msg_underline;
		bool m_extra_cmds;
		QColor m_user_color, m_msg_color;
		SettingsItem *settings;
		QStringList m_user_cmds;
		QStringList m_post_cmds;
	};
}
#endif
