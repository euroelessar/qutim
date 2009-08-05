/*
  TempGlobalInstance

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

#ifndef TEMPGLOBALINSTANCE_H
#define TEMPGLOBALINSTANCE_H

#include "../../../include/qutim/plugininterface.h"
#include "../../../include/qutim/layerinterface.h"
#include "include/qutim/layerscity.h"
#include "src/iconmanager.h"
#include <QDateTime>
using namespace qutim_sdk_0_2;

struct UnreadedMessage {
    TreeModelItem m_item;
    QDateTime m_date;
    QString m_message;
};

class TempGlobalInstance
{
public:
    TempGlobalInstance();
    static TempGlobalInstance &instance();
    void setPluginSystem(PluginSystemInterface *plugin_system)
    {
	m_plugin_system = plugin_system;
	m_show_context_menu = m_plugin_system->registerEventHandler("Core/ContactList/ContextMenu");
	m_sending_message_before_showing = m_plugin_system->registerEventHandler("Core/ChatWindow/SendLevel1");
    }

    QIcon getIcon(const QString &name)
    {
	return m_plugin_system->getIcon(name);
    }

    void createChat(const TreeModelItem &item)
    {
	m_plugin_system->createChat(item);
    }
    void setLayerInterface( LayerType type, LayerInterface *linterface);
    QIcon getContactIcon(const TreeModelItem &item,int position);
    QStringList getItemInfo(const TreeModelItem &item);
    QString getConferenceItemToolTip(const QString &protocol_name, const QString &conference_name,
				     const QString &account_name, const QString &nickname)
    {
	return m_plugin_system->getConferenceItemToolTip(protocol_name,conference_name,account_name,nickname);
    }
    void conferenceItemContextMenu(const QString &protocol_name, const QString &conference_name,
			const QString &account_name, const QString &nickname, const QPoint &menu_point)
    {
	 quint16 event_id = m_plugin_system->registerEventHandler("Core/Conference/ContactContextMenu");
	 Event ev(event_id, 5, &protocol_name, &conference_name, &account_name, &nickname, &menu_point);
	 m_plugin_system->sendEvent(ev);
    }
    void sendMessageTo(const TreeModelItem &item, const QString &message, int message_position);
    void setTrayMessageIconAnimating(bool animate)
    {
	if( !m_unreaded_messages_list.count() && !m_waiting_for_activation.count())
	m_plugin_system->setTrayMessageIconAnimating(animate);
    }

    void notifyAboutUnreadedMessage(const TreeModelItem &item);
    void notifyAboutReadedMessage(const TreeModelItem &item);
    QHash<QString,QVector<UnreadedMessage>*> m_unreaded_messages_list;
    QHash<QString,TreeModelItem> m_waiting_for_activation;

    void waitingItemActivated(const TreeModelItem &item);

    void chatOpened(const TreeModelItem &item)
    {
	m_plugin_system->chatWindowOpened(item);
    }
    void chatAboutToBeOpened(const TreeModelItem &item)
    {
	m_plugin_system->chatWindowAboutToBeOpened(item);
    }
    void chatClosed(const TreeModelItem &item)
    {
	m_plugin_system->chatWindowClosed(item);
    }
    void sendTypingNotification(const TreeModelItem &item, int type)
    {
	m_plugin_system->sendTypingNotification(item,type);
    }

    void openHistoryFor(const TreeModelItem &item)
    {
	m_history_layer->openWindow(item);
    }

    void sendImageTo(const TreeModelItem &item, const QByteArray &image_raw);

    void sendFileTo(const TreeModelItem &item, const QStringList &file_names)
    {
	m_plugin_system->sendFileTo(item,file_names);
    }

    void showContactInformation(const TreeModelItem &item)
    {
	m_plugin_system->showContactInformation(item);
    }
    void showContextMenu(const TreeModelItem &item, QPoint point)
    {
	Event ev(m_show_context_menu, 2, &item, &point);
	m_plugin_system->sendEvent ( ev );
    }
    QString getIconPath(const QString &name)
    {
	return m_plugin_system->getIconFileName(name);
    }

    void showTopicConfig(const TreeModelItem &item)
    {
	m_plugin_system->showTopicConfig(item.m_protocol_name, item.m_account_name, item.m_item_name);
    }

    void showConferenceMenu(const TreeModelItem &item, const QPoint &menu_point)
    {
	m_plugin_system->showConferenceMenu(item.m_protocol_name, item.m_account_name, item.m_item_name,menu_point);
    }

    void saveMessageToHistory(const HistoryItem &item)
    {
	m_history_layer->storeMessage(item);
    }

	QHash<QString,QStringList> getEmoticons()
    {
	return m_emoticons_layer->getEmoticonsList();
    }

    QString getEmoticonsPath()
    {
	return m_emoticons_layer->getEmoticonsPath();
    }

	void checkForEmoticons(QString &message)
    {
	m_emoticons_layer->checkMessageForEmoticons(message);
    }

    void sendingMessageBeforeShowing(const TreeModelItem item, QString &message)
    {
	m_plugin_system->sendMessageBeforeShowing(item,message);
    }
    void receivingMessageBeforeShowing(const TreeModelItem item, QString &message)
    {
	m_plugin_system->receivingMessageBeforeShowing(item,message);
    }
private:

    PluginSystemInterface *m_plugin_system;
    ContactListLayerInterface *m_cl_layer;
    HistoryLayerInterface *m_history_layer;
    EmoticonsLayerInterface *m_emoticons_layer;
    quint16 m_show_context_menu;
	quint16 m_sending_message_before_showing;

};

#endif // TEMPGLOBALINSTANCE_H
