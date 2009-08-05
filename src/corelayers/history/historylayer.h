 /*
    Abstract History Layer

    Copyright (c) 2008 by Nigmatullin Ruslan <euroelessar@gmail.com>
    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef HISTORYLAYER_H_
#define HISTORYLAYER_H_
#include <QDebug>
#include <QList>
#include <QDateTime>
#include "historyengine.h"
#include "historywindow.h"
#include "historysettings.h"
#include "../../../include/qutim/plugininterface.h"
#include "../../../include/qutim/layerinterface.h"
using namespace qutim_sdk_0_2;

class AbstractHistoryLayer: public HistoryLayerInterface, public EventHandler
{
public:
	AbstractHistoryLayer();
	virtual ~AbstractHistoryLayer();
	bool saveHistoryMessage(const HistoryItem &item);
	bool saveSystemMessage(const HistoryItem &item);
	void loadHistoryMessage(const TreeModelItem &item, const QDateTime &date_last);
	void closeHistoyWindow(const QString &identification);
	void openHistoryWindow(const TreeModelItem &item);
	void setContactHistoryName(const TreeModelItem &item);
	QString getContactHistoryName(const TreeModelItem &item);
	void loadSettings();
	virtual void release(){}
	virtual void setProfileName(const QString &profile_name);
	virtual void setLayerInterface( LayerType type, LayerInterface *linterface){}

	virtual void openWindow(const TreeModelItem &item){openHistoryWindow(item);}
	virtual bool storeMessage(const HistoryItem &item){saveHistoryMessage(item);return true;}
	virtual QList<HistoryItem> getMessages(const TreeModelItem &item, const QDateTime &last_time);

	virtual void saveLayerSettings();
	virtual QList<SettingsStructure> getLayerSettingsList();
	virtual void removeLayerSettings();

	virtual bool init(PluginSystemInterface *plugin_system);

	virtual void saveGuiSettingsPressed() {}

	virtual void removeGuiLayerSettings() {}

	void processEvent(Event &e);

private:
	HistoryEngine *m_engine;
	QHash <QString, HistoryWindow *> m_user_history_list;
	QHash <QString, QString> m_contact_history;
	QString m_profile_name;
	bool m_save_history;
	bool m_show_recent_messages;
	quint16 m_recent_messages_count;
	QList<SettingsStructure> m_settings;
	HistorySettings *m_settings_widget;
	QTreeWidgetItem *m_settings_item;
	PluginSystemInterface *m_plugin_system;
	quint16 m_event_add_item;
};

#endif /*HISTORYLAYER_H_*/

