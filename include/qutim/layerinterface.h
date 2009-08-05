/*!
 * Layer Interface
 *
 * @author Nigmatullin Ruslan
 * Copyright (c) 2009 by Nigmatullin Ruslan «euroelessar@gmail.com»
 * encoding: UTF-8
 *
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 */

#ifndef QUTIM_LAYERINTERFACE_H
#define QUTIM_LAYERINTERFACE_H

class QString;
class QWidget;
class QTextDocument;
class QTextEdit;

#include <qutim/plugininterface.h>
#include <QIODevice>
#include <QHBoxLayout>
#include <QMenu>

namespace qutim_sdk_0_2 {

class ProtocolInterface;
class SimplePluginInterface;
class SettingsInterface;

// TODO: SDK 0.3 Create valid SoundEngine
class SoundEngineLayerInterface : public LayerInterface
{
public:
	virtual ~SoundEngineLayerInterface() {}
	virtual void playSound(QIODevice *device) = 0;
	virtual void playSound(const QString &filename) = 0;
	virtual QIODevice *grabSound() = 0;
};

// TODO: SDK 0.3 Create valid VideoEngine
class VideoEngineLayerInterface : public LayerInterface
{
public:
	virtual ~VideoEngineLayerInterface() {}
	virtual void playVideo(QIODevice *device) = 0;
	virtual QIODevice *grabVideo() = 0;
};

// TODO: SDK 0.3 Add methods for store a list of messages by time, i.e. for merging history from different places
class HistoryLayerInterface : public LayerInterface
{
public:
	virtual ~HistoryLayerInterface() {}
	virtual void openWindow(const TreeModelItem &item) = 0;
	virtual bool storeMessage(const HistoryItem &item) = 0;
	virtual QList<HistoryItem> getMessages(const TreeModelItem &item, const QDateTime &last_time) = 0;
};

// TODO: SDK 0.2 Think about notifing chat layer about changing theme of emoticons
class EmoticonsLayerInterface : public LayerInterface
{
public:
	virtual ~EmoticonsLayerInterface() {}
	virtual QHash<QString,QStringList> getEmoticonsList() = 0;
	virtual void checkMessageForEmoticons(QString &message) = 0;
	virtual QString getEmoticonsPath() = 0;
};

class StatusLayerInterface : public LayerInterface
{
public:
	virtual ~StatusLayerInterface() {}
	virtual bool setStatusMessage(const TreeModelItem &item, const QString &status_type, QString &status_message, bool &dshow) = 0;

	//TODO: SDK 0.3 Remove deprecated function
	inline bool setStatusMessage(QString &status_message, bool &dshow)
	{
		return setStatusMessage(TreeModelItem(), QString(), status_message, dshow);
	}
};

// TODO: Rewrite notification layer
// TODO: SDK 0.3 Separate Notification layer into Sound and Popup layers
class NotificationLayerInterface : public LayerInterface
{
public:
	virtual ~NotificationLayerInterface() {}
	/*
	 * Show popup only
	 */
	virtual void showPopup(const TreeModelItem &item, const QString &message, NotificationType type) = 0;
	/*
	 * Play sound only
	 */
	virtual void playSound(const TreeModelItem &item, NotificationType type) = 0;
	/*
	 * Show popup and play sound
	 */
	virtual void notify(const TreeModelItem &item, const QString &message, NotificationType type) = 0;
	/*
	 * Compatibility
	 */
	//TODO: SDK 0.3 Remove deprecated functions
	inline void systemMessage(const TreeModelItem &item, const QString &message)
	{
		notify(item, message, NotifySystem);
	}
	inline void userMessage(const TreeModelItem &item, const QString &message, NotificationType type)
	{
		notify(item, message, type);
	}
};

// TODO: SDK 0.3 Think about flexibility of layer
class AntiSpamLayerInterface : public LayerInterface
{
public:
	virtual ~AntiSpamLayerInterface() {}
	virtual bool checkForMessageValidation(const TreeModelItem &item, const QString &message,
										   MessageType message_type, bool special_status) = 0;
};

class SettingsLayerInterface : public LayerInterface
{
public:
	virtual ~SettingsLayerInterface() {}
	virtual SettingsInterface *getSettings( const TreeModelItem &item ) = 0;
	virtual SettingsInterface *getSettings( const QString &name ) = 0;
	virtual bool logIn( const QString &profile, const QString &password ) = 0;
	virtual QString getProfilePath() = 0;
	virtual QDir getProfileDir() = 0;
};

class SpellerLayerInterface : public LayerInterface
{
public:
	virtual ~SpellerLayerInterface() {}
	virtual void startSpellCheck( QTextEdit *document ) = 0;
	virtual void stopSpellCheck( QTextEdit *document ) = 0;
	virtual bool isCorrect( const QString &word ) const = 0;
	virtual bool isMisspelled( const QString &word ) const = 0;
	virtual QStringList suggest( const QString &word ) const = 0;
	inline bool checkAndSuggest( const QString &word, QStringList &suggestions ) const
	{
		if( isCorrect( word ) )
			return true;
		suggestions = suggest( word );
		return false;
	}
};

// TODO: SDK 0.3 Think about flexibility of layer
class TrayLayerInterface : public LayerInterface
{
public:
	virtual ~TrayLayerInterface() {}
	virtual QMenu *contextMenu() const = 0;
	virtual QIcon icon() const = 0;
	virtual void setContextMenu(QMenu * menu) = 0;
	virtual void setIcon(const QIcon & icon) = 0;
	virtual void setToolTip(const QString & tip) = 0;
	virtual void showMessage(const QString & title, const QString & message, int timeout_hint = 10000) = 0;
	virtual QString toolTip() const = 0;
};

class ContactListLayerInterface : public LayerInterface
{
public:
	virtual ~ContactListLayerInterface() {}
	/*
	  TODO: Think about this meaning of groups
	  This should be equivalent for groups, it's more flexible and compatible with Jabber,
	  which allows to use more than one group for contact at roster
	virtual void setItemTags(const TreeModelItem &item, const QStringList &tags) = 0;
	virtual QStringList getItemTags(const TreeModelItem &item) = 0;
	 */
	virtual void addItem(const TreeModelItem &item, const QString &name = "") = 0;
	virtual void removeItem(const TreeModelItem &item) = 0;
	virtual void moveItem(const TreeModelItem &old_item, const TreeModelItem &new_item) = 0;
	virtual void setItemName(const TreeModelItem &item, const QString &name) = 0;
	virtual void setItemIcon(const TreeModelItem &item, const QIcon &icon, int position) = 0;
	virtual void setItemStatus(const TreeModelItem &item, const QIcon &icon, const QString &id, int mass) = 0;
	virtual void setItemText(const TreeModelItem &item, const QVector<QVariant> &text) = 0;
	virtual void setItemVisibility(const TreeModelItem &item, int flags) = 0;
	virtual void setItemAttribute(const TreeModelItem &item, ItemAttribute type, bool on) = 0;
	virtual QList<TreeModelItem> getItemChildren(const TreeModelItem &item = TreeModelItem()) = 0;
	virtual const ItemData *getItemData(const TreeModelItem &item) = 0;
	virtual QHBoxLayout *getAccountButtonsLayout() = 0;
	virtual void setMainMenu(QMenu *menu) = 0;
};


// TODO: SDK 0.3 Add ways for sending and accepting html/rtf messages
class ChatLayerInterface : public LayerInterface
{
public:
	virtual ~ChatLayerInterface() {}
    virtual void createChat(const TreeModelItem &item) = 0;
    virtual void newMessageArrivedTo(const TreeModelItem &item, const QString &message,
				     const QDateTime &date, bool history=false, bool in = true) = 0;
    virtual void newServiceMessageArriveTo(const TreeModelItem &item, const QString &message) = 0;
    virtual void setItemTypingState(const TreeModelItem &item, TypingAttribute) = 0;
    virtual void messageDelievered(const TreeModelItem &item, int message_position) = 0;
    virtual void contactChangeHisStatus(const TreeModelItem &item, const QIcon &icon) = 0;
    virtual void contactChangeHisClient(const TreeModelItem &item) = 0;
    virtual void changeOwnNickNameInConference(const TreeModelItem &item, const QString &new_nickname) = 0;
    virtual void setConferenceTopic(const TreeModelItem &item, const QString &topic)  = 0;
    virtual void addConferenceItem(const TreeModelItem &item, const QString &name="") = 0;
    virtual void renameConferenceItem(const TreeModelItem &item, const QString &new_name) = 0;
    virtual void removeConferenceItem(const TreeModelItem &item) = 0;
    virtual void setConferenceItemStatus(const TreeModelItem &item, const QIcon &icon, const QString &status, int mass) = 0;
    virtual void setConferenceItemIcon(const TreeModelItem &item, const QIcon &icon, int position) = 0;
    virtual void setConferenceItemRole(const TreeModelItem &item, const QIcon &icon, const QString &role, int mass) = 0;
    virtual void addImage(const TreeModelItem &item, const QByteArray &image_raw) = 0;
    virtual QStringList getConferenceItemsList(const TreeModelItem &item) = 0;
};


// TODO: SDK 0.3 Is it real to create SettingsLayerInterface?.. For example for creating MacOS-styled settings

//class SettingsLayerInterface : public LayerInterface
//{
//public:
//	virtual ~SettingsLayerInterface() {}
//	void setPlugins(ProtocolList *protocols, SimplePluginList *plugins) { m_protocols = protocols; m_plugins = plugins; }
//protected:
//	ProtocolList *m_protocols; //!< it has all protocol plug-ins
//	SimplePluginList *m_plugins; //!< it is all non-protocol plug-ins
//};

} //end namespace qutim_sdk_0_2

#endif // QUTIM_LAYERINTERFACE_H
