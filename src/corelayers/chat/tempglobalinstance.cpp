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

#include "tempglobalinstance.h"


TempGlobalInstance::TempGlobalInstance()
{
}

TempGlobalInstance &TempGlobalInstance::instance()
{
	static TempGlobalInstance tgi;
	return tgi;
}

void TempGlobalInstance::setLayerInterface(LayerType type, LayerInterface *linterface)
{
    	switch(type)
	{
	    case ContactListLayer:
		    m_cl_layer = reinterpret_cast<ContactListLayerInterface *>(linterface);
		    break;
	    case HistoryLayer:
		    m_history_layer = reinterpret_cast<HistoryLayerInterface *>(linterface);
		    break;
	    case EmoticonsLayer:
		    m_emoticons_layer = reinterpret_cast<EmoticonsLayerInterface *>(linterface);
		    break;
	    default:;
	}
}

QIcon TempGlobalInstance::getContactIcon(const TreeModelItem &item,int position)
{
    const ItemData *item_data = m_cl_layer->getItemData(item);
    if(item_data)
	return item_data->icons.value(position, QIcon());
    else
	return QIcon();
}

QStringList TempGlobalInstance::getItemInfo(const TreeModelItem &item)
{
    return m_plugin_system->getAdditionalInfoAboutContact(item);
}

void TempGlobalInstance::sendMessageTo(const TreeModelItem &item, const QString &message, int message_position)
{
    QString tmp = message;
    m_plugin_system->sendMessageToContact(item,tmp,message_position);
}

void TempGlobalInstance::notifyAboutUnreadedMessage(const TreeModelItem &item)
{
    m_cl_layer->setItemAttribute(item,ItemHasUnreaded,true);
}

void TempGlobalInstance::notifyAboutReadedMessage(const TreeModelItem &item)
{
    m_cl_layer->setItemAttribute(item,ItemHasUnreaded,false);
    setTrayMessageIconAnimating(false);
}

void TempGlobalInstance::waitingItemActivated(const TreeModelItem &item)
{
    m_cl_layer->setItemAttribute(item,ItemHasUnreaded,false);
       QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    m_waiting_for_activation.remove(identification);
    setTrayMessageIconAnimating(false);
}
void TempGlobalInstance::sendImageTo(const TreeModelItem &item, const QByteArray &image_raw)
{
    m_plugin_system->sendImageTo(item,image_raw);
}
