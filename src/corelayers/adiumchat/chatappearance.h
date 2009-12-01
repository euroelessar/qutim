/****************************************************************************
 *  chatappearance.cpp.cpp
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
#ifndef CHATAPPEARANCE_H
#define CHATAPPEARANCE_H
#include "chatlayerimpl.h"
#include "chatstyle.h"
#include "chatstyleoutput.h"
#include "ui_chatappearance.h"
#include <QWidget>
#include <QDebug>
#include <QDateTime>
#include <QWebFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <libqutim/configbase.h>
#include <libqutim/protocol.h>
#include <libqutim/account.h>
#include <libqutim/notificationslayer.h>
#include <libqutim/settingswidget.h>
#include <libqutim/libqutim_global.h>

namespace Ui
{
	class chatAppearance;
}
class QWebPage;
namespace AdiumChat
{
	class ChatStyleOutput;
	class ChatAppearance : public SettingsWidget
	{
		Q_OBJECT
	public:
		ChatAppearance();
		virtual void cancelImpl();
		virtual void loadImpl();
		virtual void saveImpl();
		virtual ~ChatAppearance();
	private slots:
		void onCurrentIndexChanged(int index);
		void changeVariant(const QString &variant);
	private:
		int getThemes();
		void makeSettings(const QString &theme);
		void makePage();
		Ui::chatAppearance *ui;
		QHash<QString, QString> m_themes;
		QString m_current_style_name;
		QString m_current_variant;
		StyleVariables m_current_style_variables;
		QWebPage *m_page;
		ChatSessionImpl *m_chat_session;
		bool is_load;
		QWidget *settingsWidget;
	};

}

#endif // CHATAPPEARANCE_H
