/*!
 * Plugin Interface
 *
 * @author m0rph
 * @author Nigmatullin Ruslan <euroelessar@gmail.com>
 * Copyright (c) 2008 by m0rph <m0rph.mailbox@gmail.com>
 *          2008-2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef QUTIM_SDK_PLUGININTERFACE_H
#define QUTIM_SDK_PLUGININTERFACE_H

#include <QtPlugin>
#include <QIcon>
#include <QDateTime>
#include <QAction>
#include <QSettings>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QCoreApplication>
#include <stdarg.h>

class QDir;

#ifndef BUILD_QUTIM
#  define QUTIM_DEPRECATED Q_DECL_DEPRECATED
#else
#  define QUTIM_DEPRECATED
#endif

#define Q_REGISTER_EVENT(ARG, ID) \
		static const quint16 ARG = ::qutim_sdk_0_2::SystemsCity::PluginSystem()->registerEventHandler( ID )

namespace qutim_sdk_0_3 {
	struct Event;
	struct TreeModelItem;
	struct HistoryItem;
}

namespace qutim_sdk_0_2 {

class PluginSystemInterface;
class PluginInterface;
class SimplePluginInterface;
class DeprecatedSimplePluginInterface;
class IconManagerInterface;
class TranslatorInterface;
class SettingsInterface;

/*!
 * @brief Structure for settings widget
 */
struct SettingsStructure
{
        QTreeWidgetItem *settings_item; /*!< Settings Tree widget item */
		QWidget *settings_widget;       /*!< Settings widget */
};

/*!
 * @brief Enumeration of Layers' types
 */
enum LayerType
{
	ContactListLayer = 0,   // + +
	ChatLayer,              // + -
	HistoryLayer,           // + -
	NotificationLayer,      // + +
	AntiSpamLayer,          // + +
	SoundEngineLayer,       // + +
	VideoEngineLayer,       // + -
	StatusLayer,            // + +
	TrayLayer,              // + +
	EmoticonsLayer,
	SpellerLayer,
	SettingsLayer,
	UnknownLayer,
	UnknownLayer1,
	UnknownLayer2,
	UnknownLayer3,
	UnknownLayer4,
	UnknownLayer5,
	UnknownLayer6,
	UnknownLayer7,
	InvalidLayer
};

/** 
* @brief Interface for core layers, like contact list or chat
*/
class LayerInterface
{
public:
	virtual ~LayerInterface() {}

	// TODO: SDK 0.3 Change const QString & to QString or something like this, it is easy to forget about it
	/** 
	* @brief Get name of layer, like "qutim" or "kde".
	* 
	* @return Name of layer
	*/
	const QString &name() { return m_name; }
	/** 
	* @brief Get version of layer
	* 
	* @return Version of layer
	*/
	const QString &version() { return m_version; }

	// TODO: SDK 0.3 Remove pointer to PluginSystemInterface, using of SystemsCity is more efficient and easy
	/** 
	* @brief Initialization of layer
	* 
	* @param plugin_system Pointer to PluginSystemInterface
	* 
	* @return true if success, otherwise false
	*/
	virtual bool init(PluginSystemInterface *plugin_system) = 0;
	/** 
	* @brief Release resources of layer
	*/
	virtual void release() = 0;
	/** 
	* @brief Set name of profile, entry point for reading of settings
	* 
	* @param profile_name Name of profile
	*/
	virtual void setProfileName(const QString &profile_name) = 0;
	// TODO: SDK 0.3 Set as default realization settings pointers to SystemsCity
	/** 
	* @brief Set pointer to layer interfaces
	* 
	* @param type Type of layer
	* @param layer_interface Pointer to LayerInterface
	*/
	virtual void setLayerInterface( LayerType type, LayerInterface *layer_interface) = 0;

	// TODO: SDK 0.3 Set fucntions as non-pure virtual, it's annoying to declare them
	/** 
	* @brief Apply settings button has been pressed, so it's needed to save settings
	*/
	virtual void saveLayerSettings() = 0;
	/** 
	* @brief Get list of settings structures
	* 
	* @return List of settings structures
	*/
	virtual QList<SettingsStructure> getLayerSettingsList() { return m_settings; }
	/** 
	* @brief Remove all settings structures without applying changes
	*/
	virtual void removeLayerSettings() = 0;

	virtual void saveGuiSettingsPressed() = 0;
	virtual QList<SettingsStructure> getGuiSettingsList() { return m_gui_settings; }
	virtual void removeGuiLayerSettings() = 0;

protected:
	QString m_name;
	QString m_version;
	QList<SettingsStructure> m_settings;
	QList<SettingsStructure> m_gui_settings;
};
/*!
 * @brief item of the TreeModel, used to represent contact, message or somewhat
 */
struct TreeModelItem
{
	enum Type
	{
		Buddy = 0,
		Group,
		Account,
		Conference = 32,
		ConferenceItem
	};
	inline TreeModelItem() : m_item_type(0xff) {}
	QString m_protocol_name; /*!< Protocol name */
	QString m_account_name; /*!< Account name */
	QString m_item_name; /*!< item name */
	QString m_parent_name; /*!< Parent item name */
	quint8 m_item_type; /*!< item type: 0 - buddy; 1 - group; 2 - account item */
	QString m_item_history;
	inline operator qutim_sdk_0_3::TreeModelItem &();
	inline operator const qutim_sdk_0_3::TreeModelItem &() const;
};
/*!
 * @brief item of the history
 *
 * In a few words - one separate message
 * HistoryItem structure is not fully understood by me, look at m_user and m_from
 */
struct HistoryItem
{
	QString m_message; /*!< Message */
	QDateTime m_time; /*!< Time of sending */
	TreeModelItem m_user; /*!< Contact's item */
	QString m_from; /*!< Deprecated */
	bool m_in;  /*!< FlagIncoming (true) or outgoing (false) message */
	qint8 m_type; /*!< 0 - system; 1Private; */
	inline operator qutim_sdk_0_3::HistoryItem &();
	inline operator const qutim_sdk_0_3::HistoryItem &() const;
};

/*
 * This interface and structure may be used for getting access Item's information
 * which is contained at ContactList, and for creating own impls of item Delegate.
 * Pointer to ItemData can be accessed by Qt::UserData, to Delegate by Qt::UserData+1
 *
 */

struct ItemData
{
	ItemData() : icons(13), delegate(0) {}
	TreeModelItem item;
	QString name;
	QVector<QVariant> status;
	QVector<QIcon> icons;
	QString status_id;
	qint16 status_mass;
	quint16 visibility;
	quint16 attributes;
	QAbstractItemDelegate *delegate;
};

//class ItemDelegate
//{
//	virtual ~ItemDelegate() {}
//	virtual void paint(QPainter *painter, const QRect &rect, ItemData *data);
//	virtual QSize size(const, ItemData *data);
//};

/*!
 * @brief plugin event
 * @attention this structure is an unknown stuff, created by m0rph, noone can tell, what the hell it is
 */

enum EventType { 
	ContactContextAction = 0,
	ItemAddedAction,
	ItemRemovedAction,
	ItemMovedAction,
	ItemChangedNameAction,
	ItemChangedStatusAction,
	AccountIsOnlineAction,
	SendingMessageBeforeShowing,
	SendingMessageAfterShowing,
	SengingMessageAfterShowingLastOutput,
	PointersAreInitialized,
	SoundAction,
	SystemNotification,
	UserNotification,
	ReceivingMessageFirstLevel,
	ReceivingMessageSecondLevel,
	ReceivingMessageThirdLevel,
	ReceivingMessageFourthLevel,
	ChangeWindowId,
	AllPluginsLoaded
};

/** 
* @brief Message types for anti-spam
*/
enum MessageType
{
	MessageText = 0,
	MessageAuthorization,
	MessageFileTransfer
};

/** 
* @brief Visibility flags
*/
enum Visibility
{
	ShowOffline = 0x01,
	ShowTyping  = 0x02,
	ShowMessage = 0x04,
	ShowStatus  = 0x08,
	ShowOnline  = 0x10,
	ShowAlwaysVisible	= ShowOffline | ShowTyping | ShowMessage | ShowStatus | ShowOnline,
	ShowAlwaysInvisible	= ShowTyping  | ShowMessage,
	ShowDefault = ShowAlwaysVisible ^ ShowOffline
};

enum ItemAttribute
{
	ItemHasUnreaded = 0x01,
	ItemIsTyping = 0x02
};

enum TypingAttribute
{
	ItemTypingStop = 0,
	ItemTypingStart
};

/*!
 * @brief Event structure
 *
 * It is used for communicating between plugins and core.
 *
 * Examples are located in http://wiki.qutim.org/events
 */
struct Event
{
	inline Event(quint16 event_id = 0xffff) : id(event_id)  {}
	inline Event(quint16 event_id, int num, ...) : id(event_id)
	{
		if(num<1)
			return;
		args.reserve(num);
		va_list types;
		va_start(types, num);
		while(num--)
			args.append(va_arg(types, void *));
		va_end(types);
	}
	template< typename T >
	inline const T &at(int i) const { return *reinterpret_cast<const T *>(args.at(i)); } // usually you don't need it
	template< typename T >
	inline T &at(int i) { return *reinterpret_cast<T *>(args[i]); }

	inline void append(const void *arg) { args.append(reinterpret_cast<void *>(reinterpret_cast<qptrdiff>(arg))); }
	inline void append(void *arg) { args.append(arg); }
	inline int size() { return args.size(); }
	inline bool send();
	inline operator qutim_sdk_0_3::Event &();
	inline operator const qutim_sdk_0_3::Event &() const;
	quint16 id;             /*!< Event type id */
	QVector<void *> args;   /*!< List with pointers to event's arguments */
};
/*!
 * @brief Event handler
 *
 * It is the interface for getting Events from core and plugins
 */
class EventHandler
{
protected:
	inline void destruct_helper();
public:
	enum Priority
	{
		HighestPriority	= 0x4000,
		HighPriority	= 0x2000,
		NormalPriority	= 0x1000,
		LowPriority		= 0x0800,
		LowestPriority	= 0x0400
	};
	virtual void processEvent(Event &event) { Q_UNUSED(event); }
	virtual ~EventHandler() { destruct_helper(); }
};

typedef void (EventHandler::*EventHandlerFunc)( Event &event );
typedef void (*ProcessEventFunc)( Event &event );

/** 
* @brief Notification types
*/
enum NotificationType
{
	NotifyOnline = 0,
	NotifyOffline,
	NotifyStatusChange,
	NotifyBirthday,
	NotifyStartup,
	NotifyMessageGet,
	NotifyMessageSend,
	NotifySystem,
	NotifyTyping,
	NotifyBlockedMessage,
	NotifyCustom,
	NotifyCount
};

enum MessageToProtocolEventType {
	SetStatus = 0,
	RestoreStatus
};

struct PluginEvent {
	EventType system_event_type;
	QList<void *> args;
};
/*!
 * @brief event for sound engine
 */
// TODO: Remove it! It's annoying
const char* const XmlEventNames[] = { "c_online",
	"c_offline", "c_changed_status",
	"c_birth", "start",
	"m_get", "m_send", "sys_event", "c_typing", "c_blocked_message", "sys_custom", 0 };

/*!
 * Abstract interface of plugin system, visible to plug-ins
 * These functions can be called by plugin. Visible kernel functions. 
 */
// TODO: SDK 0.3 Remove deprecated functions, move some blocks of functions to separated classes
// Also think about giving plugins the direct access to layers
class PluginSystemInterface
{
protected:
	virtual ~PluginSystemInterface() {}
public:
	virtual bool setLayerInterface( LayerType type, LayerInterface *interface) = 0;
	/*!
	 * @brief Register event id with certain Event Handler
	 *
	 * @param event_id String id of the event type
	 * @param handler Pointer to Event Handler for this id, zero if only getting the integer id of the string id is needed
	 * @return quint16 Integer id of the string id
	 */
	virtual quint16 registerEventHandler(const QString &event_id, EventHandler *handler = 0, quint16 priority = EventHandler::NormalPriority) = 0;
	/*!
	 * @brief Remove event handler from list of hanlers with current id
	 *
	 * @param event_id Id of the event type of handler
	 * @param handler Pointer to Event Handler for this id
	 */
	virtual void removeEventHandler(quint16 id, EventHandler *handler) = 0;
	/*!
	 * @brief Send Event to every handler, registered with Event's id
	 *
	 * @param event Event, which plugin wants to send
	 * @return false if no handler gets the Event, otherwise true
	 */
	virtual bool sendEvent(Event &event) = 0;
	/*!
	 * @brief Update status icons
	 *
	 * Called each time, when account status is changed
	 * It updates your status icon in system tray (may be also in tooltip, who knows)
	 */
	// TODO: SDK 0.3 Remove this function, use setContactItemStatus instead, it would be more flexible
	virtual void updateStatusIcons() = 0;
	/*!
	 * @brief Add an item to contact-list
	 *
	 * Adds an contact to CL
	 *
	 * @param item Item to be added
	 * @param name Name of the item
	 * @return Result of the attempt, true if all is ok
	 * @see TreeModelItem
	 */
	virtual bool addItemToContactList(const TreeModelItem &item, const QString &name = QString()) = 0;
	/*!
	 * @brief Remove an item from contact-list
	 *
	 * Removes an contact from CL.
	 * Please, notice that we have no unique ID for an item
	 *
	 * @param item Item to be deleted
	 * @return Result of the attempt, TRUE if all is ok
	 * @see TreeModelItem
	 */
	virtual bool removeItemFromContactList(const TreeModelItem &item) = 0;
	/*!
	 * @brief Move an item in contact-list
	 *
	 * Move item in the CL, e.g. change the group
	 * It's just an visualisation, server-side move should be done by the protocol!
	 * 
	 * @param old_item Item's old "status"
	 * @param new_item Item's new "status"
	 * @return Result of the attempt, true if all is ok
	 * @see TreeModelItem
	 */
	virtual bool moveItemInContactList(const TreeModelItem &old_item, const TreeModelItem &new_item) = 0;

	/*!
	 * @brief Set the name for the item in contact-list
	 * 
	 * Sets the name for the item (contact, group, somewhat) in the CL
	 *
	 * @param item Item, to be changed
	 * @param name New name for the item
	 * @return Result of the attempt, true if all is ok
	 * @see TreeModelItem
	 */
	virtual bool setContactItemName(const TreeModelItem &item, const QString &name) = 0;
	/*!
	 * @brief Set an icon for the contact
	 *
	 * Sets an icon for the contact
	 * item have two places left and ten places right for the icon
	 * Position 1 is reserved for the avatar
	 * Position 12 is reserved for the client icon
	 *
	 * @param item Item in CL
	 * @param icon Icon to be added
	 * @param position Position in which icon should be placed, can be from 0 to 11
	 * @return Result of the attempt, true if all is ok
	 * @see TreeModelItem
	 */
	virtual bool setContactItemIcon(const TreeModelItem &item, const QIcon &icon, int position) = 0;
	/*!
	 * @brief Set an row for contact-list item
	 *
	 * Each item has 3 additional rows below, which can be set by this function
	 *
	 * @param item Item in CL
	 * @param row Row to be set
	 * @param position Position in which row should be placed, can be from 0 to 3
	 * @return Result of the attempt, true if all is ok
	 * @see TreeModelItem
	 */
	virtual bool setContactItemRow(const TreeModelItem &item, const QList<QVariant> &row, int position) = 0;
	/*!
	 * @brief Set contact-list item status
	 *
	 * Sets the status of the item.
	 *
	 * @param item Item in CL
	 * @param icon Icon for the status
	 * @param text Text for the status
	 * @param mass "weight" of the item, used for item sort, can be from 1 to 1000 (offline)
	 * @return Result of the attempt, true if all is ok
	 * @see TreeModelItem
	 */
	virtual bool setContactItemStatus(const TreeModelItem &item, const QIcon &icon, const QString &text, int mass) = 0;
	/*!
	 * @brief Set status message
	 *
	 * @param status_message Status message
	 * @param dshow If true, don't show this dialog again
	 * @return Result of the attempt, true if all is ok
	 */
	virtual bool setStatusMessage(QString &status_message, bool &dshow) = 0;
	/*!
	 * @brief Transfer message from some contact
	 *
	 * Create message, received from the contact
	 *
	 * @param item Contact, which sends a message
	 * @param message Message sent
	 * @param message_date Message sending datetime
	 * @see TreeModelItem
	 */
	virtual void addMessageFromContact(const TreeModelItem &item, const QString &message, const QDateTime &message_date) = 0;
	/*!
	 * @brief Transfer service message
	 *
	 * Create service message
	 *
	 * @param item Contact, related to the message
	 * @param message Message
	 * @see TreeModelItem
	 */
	virtual void addServiceMessage(const TreeModelItem &item, const QString &message) = 0;
	/*!
	 * @brief Send raw image
	 *
	 * Send raw-format image
	 *
	 * @param item Contact who sent an image
	 * @param image_raw Image in raw format
	 * @see TreeModelItem
	 */
	virtual void addImage(const TreeModelItem &item, const QByteArray &image_raw) = 0;
	/*!
	 * @brief Create typing notify
	 *
	 * Create a notification, that some contact is typing
	 *
	 * @param item Contact, which is typing
	 * @param typing Typing status, true or false
	 * @see TreeModelItem
	 */
	virtual void contactTyping(const TreeModelItem &item, bool typing) = 0;
	/*!
	 * @brief Set the flag that message is delivered
	 *
	 * Marks (by checkmark usually) message sent to contact as delivered
	 *
	 * @param item Contact, whom message is sent to
	 * @param message_position Position of the message, got from kernel
	 * @see TreeModelItem
	 */
	virtual void messageDelievered(const TreeModelItem &item, int message_position) = 0;
	/*!
	 * @brief Check the message through the anti-spam
	 * 
	 * @param item Contact who sent the message
	 * @param message The message to be checked
	 * @param message_type Type of the message, 0If simple message, 1 - if authorisation request
	 * @param special_status Invisible for all or any other status, which require silence and non-responding
	 * @return Result of the check, true if all is ok
	 * @see TreeModelItem
	 */
	// TODO: SDK 0.3 change int for enum, look at AntiSpamLayer interface
	virtual bool checkForMessageValidation(const TreeModelItem &item, const QString &message,
			int message_type, bool special_status) = 0;
	/*!
	 * @brief Notify about contact birthday
	 *
	 * Create a notification about contact birthday
	 *
	 * @param item Birthday person :)
	 * @see TreeModelItem
	 */
	// TODO: SDK 0.3 May be it would be useful to know about the date?
	virtual void notifyAboutBirthDay(const TreeModelItem &item) = 0;
	/*!
	 * @brief System notification
	 *
	 * @param item Notification info
	 * @param message Notification message
	 * @see TreeModelItem
	 */
	// TODO: SDK 0.3 Fix misspelling
	virtual void systemNotification(const TreeModelItem &item, const QString &message) = 0;
	inline void systemNotifiacation(const TreeModelItem &item, const QString &message) { systemNotification(item, message); }
	/*!
	 * @brief Custom notification
	 *
	 * @param item Notification info
	 * @param message Notification message
	 * @see TreeModelItem
	 */
	virtual void customNotification(const TreeModelItem &item, const QString &message) = 0;
	inline void customNotifiacation(const TreeModelItem &item, const QString &message) { customNotification(item, message); }
	/*!
	 * @brief Get filename of the icon
	 *
	 * @param icon_name Name of the icon
	 * @return Filename of the icon
	 */
	// TODO: SDK 0.3 Remove functions of icon manager from PluginSystem, IconManager has it already
	virtual QString getIconFileName(const QString & icon_name) = 0;
	/*!
	 * @brief Get icon by its name
	 *
	 * @param icon_name - name of the icon
	 * @return QIcon - the icon requested
	 */
	virtual QIcon getIcon(const QString & icon_name) = 0;
	/*!
	 * @brief Get filename of the status icon
	 *
	 * @param icon_name Name of the icon
	 * @param default_path Path, where icon should be found
	 * @return Filename of the status icon
	 */
	virtual QString getStatusIconFileName(const QString & icon_name, const QString & default_path) = 0;
	/*!
	 * @brief Get status icon
	 *
	 * @param icon_name Name of the icon
	 * @param default_path Path where icon should be found
	 * @return Requested icon
	 */
	virtual QIcon getStatusIcon(const QString & icon_name, const QString & default_path) = 0;
	/*!
	 * @brief Set contant online status
	 *
	 * @param item Affected account
	 * @param online Contact status, true if online
	 * @see TreeModelItem
	 */
	// TODO: SDK 0.3 Remove this function, use setContactItemStatus instead
	virtual void setAccountIsOnline(const TreeModelItem &item, bool online) = 0;
	/*!
	 * @brief Create chat with the target contact
	 *
	 * @param item Contact to create chat with
	 * @see TreeModelItem
	 */
	virtual void createChat(const TreeModelItem &item) = 0;
	/*!
	 * @brief Get qutIM version
	 *
	 * @param major Major version
	 * @param minot Minor version
	 * @param secminor Lesser minor version
	 * @param svn Subversion revision
	 */
	virtual void getQutimVersion(quint8 &major, quint8 &minor, quint8 &secminor, quint16 &svn) = 0;
	/*!
	 * @brief Create a conference
	 *
	 * @param protocol_name Protocol to be used
	 * @param conference_name Name of the conference
	 * @param account_name Account to be used
	 */
	// TODO: SDK 0.3 Remove this functions, use addItemToContactList with TreeModelItem instead
	virtual void createConference(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name) = 0;
	/*!
	 * @brief Add message to the target conference
	 *
	 * @param protocol_name Name of the protocol
	 * @param conference_name Name of the conference
	 * @param account_name Name of the account
	 * @param from Message sender
	 * @param message Message sent
	 * @param date Date of the message
	 * @param history Has the message been taken from history or not
	 */
	virtual void addMessageToConference(const QString &protocol_name,
			const QString &conference_name, const QString &account_name,
			const QString &from,
			const QString &message, const QDateTime &date, bool history) = 0;
	/*!
	 * @brief Change own nickname in the conference
	 *
	 * @param protocol_name Name of the protocol
	 * @param conference_name Name of the conference
	 * @param account_name Name of the account
	 * @param nickname Nickname to be set
	 */
	virtual void changeOwnConferenceNickName(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname) = 0;
	/** 
	* @brief Set topic of conference
	* 
	* @param protocol_name Protocol name
	* @param conference_name Conference name
	* @param account_name Account name
	* @param topic Text of a topic
	*/
	virtual void setConferenceTopic(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &topic) = 0;
	/** 
	* @brief Add system message to conference
	* 
	* @param protocol_name Name of protocol
	* @param conference_name Name of conference
	* @param account_name Name of account
	* @param message Text of message
	* @param date Date of message
	* @param history true if from history, otherwise false
	*/
	virtual void addSystemMessageToConference(const QString &protocol_name,
			const QString &conference_name, const QString &account_name,
			const QString &message, const QDateTime &date, bool history) = 0;
	/** 
	* @brief Add item to conference, usually it is room participant
	* 
	* @param protocol_name Name of protocol
	* @param conference_name Name of conference
	* @param account_name Name of account
	* @param nickname Nickname of item
	*/
	virtual void addConferenceItem(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname) = 0;
	/** 
	* @brief Remove conference from item
	* 
	* @param protocol_name Protocol name
	* @param conference_name Conference name
	* @param account_name Account name
	* @param nickname Nickname of item
	*/
	virtual void removeConferenceItem(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname) = 0;
	/** 
	* @brief Rename item at conference
	* 
	* @param protocol_name Protocol name
	* @param conference_name Conference name
	* @param account_name Account name
	* @param nickname Old nickname of item
	* @param new_nickname New nickname of item
	*/
	virtual void renameConferenceItem(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname,
			const QString &new_nickname) = 0;
	/** 
	* @brief Set status of item at conference
	* 
	* @param protocol_name Protocol name
	* @param conference_name Conference name
	* @param account_name Account name
	* @param nickname Nickname of item
	* @param icon Icon of status
	* @param status ID of status
	* @param mass "Mass" of status
	*/
	virtual void setConferenceItemStatus(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname,
			const QIcon &icon, const QString &status, int mass) = 0;
	/** 
	* @brief Set custom conference item's icon
	* 
	* @param protocol_name Protocol name
	* @param conference_name Conference name
	* @param account_name Account name
	* @param nickname Nickname of item
	* @param icon Icon to set
	* @param position Position of icon, must be between 1 and 14
	*/
	virtual void setConferenceItemIcon(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname,
			const QIcon &icon, int position) = 0;
	/** 
	* @brief Set role of item at conference
	* 
	* @param protocol_name Protocol name
	* @param conference_name Conference name
	* @param account_name Account name
	* @param nickname Nickname of item
	* @param icon Icon for role
	* @param role ID of role
	* @param mass Mass of role
	*/
	virtual void setConferenceItemRole(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname,
			const QIcon &icon, const QString &role, int mass) = 0;
	/** 
	* @brief Get participants of conference
	* 
	* @param protocol_name Protocol name
	* @param conference_name Conference name
	* @param account_name Account name
	* 
	* @return List of participants' nicknames
	*/
	virtual QStringList getConferenceItemsList(const QString &protocol_name,
			const QString &conference_name, const QString &account_name) = 0;
	/** 
	* @brief Get ToolTip of item at conference
	* 
	* @param protocol_name Protocol name
	* @param conference_name Conference name
	* @param account_name Account name
	* @param nickname Nickname of item
	* 
	* @return ToolTip of item
	*/
	virtual QString getConferenceItemToolTip(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname) = 0;
	// TODO: SDK 0.3 Remove this deprecated functions
	/** 
	* @brief Make item always visible at contact list
	* 
	* @param item Item which have to be visible
	* @param visible true if always visible, otherwise item will have default visibility attributes
	*/
	virtual void setItemVisible(const TreeModelItem &item, bool visible) = 0;
	/** 
	* @brief Make item always invisible at contact list
	* 
	* @param item Item which have to be invisible
	* @param invisible true if always invisible, otherwise item will have default visibility attributes
	*/
	virtual void setItemInvisible(const TreeModelItem &item, bool invisible) = 0;
	/** 
	* @brief Add action to menu of item with type 0
	* 
	* @param plugin_action Action to add
	*/
	virtual void registerContactMenuAction(QAction *plugin_action, DeprecatedSimplePluginInterface *pointer_this = 0) = 0;
	/** 
	* @brief Send message over IM protocol
	* 
	* @param item Recipient of message
	* @param message Text of message
	* @param silent If true then show message to user and write to history, otherwise do not
	*/
	virtual void sendCustomMessage(const TreeModelItem &item, const QString &message, bool silent = false) = 0;
	/** 
	* @brief Add action to main menu
	* 
	* @param menu_action Action to add
	*/
	virtual void registerMainMenuAction(QAction *menu_action) = 0;
	virtual void redirectEventToProtocol(const QStringList &protocol_name, const QList<void*> &event) = 0;
	/** 
	* @brief Play sound by it's notification type
	* 
	* @param event Notification type of event
	*/
	virtual void playSound(NotificationType event) = 0;
	/** 
	* @brief Play audio file
	* 
	* @param file_name File name
	*/
	virtual void playSound(const QString &file_name) = 0;
	/** 
	* @brief Simple realization of MacOS X PList settings format
	* 
	* @code
	* QSettings settings("/etc/config.plist", SystemsCity::PluginSystem()->plistFormat());
	* QStringList paths = settings.value("paths").toStringList();
	* @endcode
	* 
	* @return ID of format
	*/
	virtual QSettings::Format plistFormat() = 0;
	/** 
	* @brief Change item name of chat window, i.e. jid for Jabber
	* 
	* @param item Item
	* @param id New name of item
	*/
	virtual bool changeChatWindowID(const TreeModelItem &item, const QString &id) = 0;
	/** 
	* @brief Get some info about contact
	* 
	* @param item Item
	* 
	* @return Size of list may be different.
	* First element - nickname of contact.
	* Second element - path to avatar of contact.
	* Third element - human-readable name of client.
	* Fourth element - some additional information.
	*/
	virtual QStringList getAdditionalInfoAboutContact(const TreeModelItem &item) = 0;
	// TODO: SDK 0.3 Make possible to get name and version of os separately
	/** 
	* @brief Get information about system
	* 
	* @param version Human-readable name of operation system with it's version
	* @param timezone Short name of time-zone
	* @param timezone_offset Timezone offset in minutes
	*/
	virtual void getSystemInfo(QString &version, QString &timezone, int &timezone_offset) = 0;
	virtual void QUTIM_DEPRECATED registerEventHandler(const EventType &type, DeprecatedSimplePluginInterface *plugin) = 0;
	virtual void QUTIM_DEPRECATED releaseEventHandler(const QString &event_id, PluginInterface *plugin) = 0;
	virtual void QUTIM_DEPRECATED processEvent(PluginEvent &event) = 0;
	/** 
	* @brief Get ToolTip of item
	* 
	* @param item Owner of ToolTip
	* 
	* @return ToolTip
	*/
	virtual QString getItemToolTip(const TreeModelItem &item) = 0;
	/** 
	* @brief Send message to contact over IM, should be only used by Chat Layer
	* 
	* @param item Receiver of message
	* @param message Text of message
	* @param message_icon_position Internal position of message icon
	*/
	virtual void sendMessageToContact(const TreeModelItem &item, QString &message, int message_icon_position) = 0;
	/** 
	* @brief Make icon at tray animating
	* 
	* @param animate If true then animate, else not
	*/
	virtual void setTrayMessageIconAnimating(bool animate) = 0;
	/** 
	* @brief Notify protocols that chat window has been opened
	* 
	* @param item Opposite item
	*/
	virtual void chatWindowOpened(const TreeModelItem &item) = 0;
	/**
	 * @brief Notify protocols that chat window is going to be opened
	 *
	 * @param item Opposite item
	 *
	 */
	virtual void chatWindowAboutToBeOpened(const TreeModelItem &item) = 0;
	/** 
	* @brief Notify protocols that chat window has been closed
	* 
	* @param item Opposite item
	*/
	virtual void chatWindowClosed(const TreeModelItem &item) = 0;
	/** 
	* @brief Send typing notification over IM
	* 
	* @param item Receiver of notification
	* @param notification_type 0 - typing is finished, 1 - continue, 2 - started 
	*/
	virtual void sendTypingNotification(const TreeModelItem &item, int notification_type) = 0;
	/** 
	* @brief Send image over IM to contact
	* 
	* @param item Reciever of image
	* @param image_raw Image in raw format 
	*/
	virtual void sendImageTo(const TreeModelItem &item, const QByteArray &image_raw) = 0;
	/** 
	* @brief Send files over IM
	* 
	* @param item Receiver of file
	* @param file_names List of file names
	*/
	virtual void sendFileTo(const TreeModelItem &item, const QStringList &file_names) = 0;
	/** 
	* @brief Show contact information of item
	* 
	* @param item Owner of information
	*/
	virtual void showContactInformation(const TreeModelItem &item) = 0;
	/*!
	 * @biief Register event id with certain Event Handler
	 *
	 * @param event_id - string id of the event type
	 * @param handler - receiver of event
	 * @param memberPointer to EventHandlerFunc for this id, zero if only getting the integer id of the string id is needed
	 * @return quint16Integer id of the string id
	 */
	virtual quint16 registerEventHandler(const QString &event_id, EventHandler *handler, EventHandlerFunc member, quint16 priority = EventHandler::NormalPriority) = 0;
	/*!
	 * @brief Remove event handler from list of hanlers with current id
	 *
	 * @param event_idId of the event type of handler
	 * @param handler - receiver of event
	 * @param memberPointer to EventHandlerFunc for this id
	 */
	virtual void removeEventHandler(quint16 id, EventHandler *handler, EventHandlerFunc member) = 0;
	/*!
	 * @brief Register event id with certain Event Handler
	 *
	 * @param event_id - string id of the event type
	 * @param funcPointer to ProcessEventFunc for this id, zero if only getting the integer id of the string id is needed
	 * @return quint16Integer id of the string id
	 */
	virtual quint16 registerEventHandler(const QString &event_id, ProcessEventFunc func, quint16 priority = EventHandler::NormalPriority) = 0;
	/*!
	 * @brief Remove event handler from list of hanlers with current id
	 *
	 * @param event_idId of the event type of handler
	 * @param funcPointer to ProcessEventFunc for this id
	 */
	virtual void removeEventHandler(quint16 id, ProcessEventFunc func) = 0;
	/*!
	 * @brief Register event id with certain Event Handler by Qt slot/signal way
	 *
	 * @param event_id - string id of the event type
	 * @param objectPointer to event receiver
	 * @param member - signal or slot of a receiver
	 * @return quint16Integer id of the string id
	 */
	virtual quint16 registerEventHandler(const QString &event_id, QObject *object, const char *member, quint16 priority = EventHandler::NormalPriority ) = 0;
	/*!
	 * @brief Remove event handler from list of hanlers with current id
	 *
	 * @param event_idId of the event type of handler
	 * @param objectPointer to receiver for this id
	 * @param member - string of a member to remove, if 0 it will remove every members of this object
	 */
	virtual void removeEventHandler(quint16 id, QObject *object, const char *member = 0) = 0;
	/** 
	* @brief Show configuration of topic
	* 
	* @param protocol_name Protocol name
	* @param account_name Account name
	* @param conference Conference name
	*/
	virtual	void showTopicConfig(const QString &protocol_name, const QString &account_name, const QString &conference) = 0;
	/** 
	* @brief Show conference menu
	* 
	* @param protocol_name Protocol name
	* @param account_name Account name
	* @param conference_name Conference name
	* @param menu_point Position of menu
	*/
	virtual void showConferenceMenu(const QString &protocol_name, const QString &account_name, const QString &conference_name,
			const QPoint &menu_point) = 0;
	/** 
	* @brief Get pointer to IconManager
	* 
	* @return Pointer to IconManager
	*/
	virtual IconManagerInterface *getIconManager() = 0;
	/** 
	* @brief Get paths where qutIM may search it's stuff
	* 
	* @return Relative and absolute paths
	*/
	virtual QStringList getSharePaths() = 0;
	/** 
	* @brief Append one more share path
	* 
	* @param path Absolute or relative path
	*/
	virtual void appendSharePath( const QString &path ) = 0;
	/** 
	* @brief Remove share path
	* 
	* @param path Absolute or relative path
	*/
	virtual void removeSharePath( const QString &path ) = 0;
	/** 
	* @brief Remove event handler
	* 
	* @param handler Event handler to remove
	*/
	virtual void removeEventHandler(EventHandler *handler) = 0;
	/** 
	* @brief Remove event handler
	* 
	* @param func Function to remove
	*/
	virtual void removeEventHandler(ProcessEventFunc func) = 0;
	/** 
	* @brief Remove event handler's method
	*
	* @param handler Owner of method
	* @param member Member of handler to remove
	*/
	virtual void removeEventHandler(EventHandler *handler, EventHandlerFunc member) = 0;
	/** 
	* @brief Remove event handler
	* 
	* @param object Handler to remove
	*/
	virtual void removeEventHandler(QObject *object) = 0;
	virtual void receivingMessageBeforeShowing(const TreeModelItem &item, QString &message) = 0;
	virtual void sendMessageBeforeShowing(const TreeModelItem &item, QString &message) = 0;
	virtual SettingsInterface *getSettings( const TreeModelItem &item ) = 0;
	virtual SettingsInterface *getSettings( const QString &name ) = 0;
	/** 
	* @brief Get profile path, i.e. ~/.config/qutim/qutim.ProfileName/
	* 
	* @return Profile path
	*/
	virtual QString getProfilePath() = 0;
	/** 
	* @brief Get profile dir, i.e. ~/.config/qutim/qutim.ProfileName/
	* 
	* @return Profile dir
	*/
	virtual QDir getProfileDir() = 0;
	virtual void centerizeWidget(QWidget *widget) = 0;
	virtual QList<TreeModelItem> getItemChildren(const TreeModelItem &item = TreeModelItem()) = 0;
	virtual void setItemVisibility(const TreeModelItem &item, int flags) = 0;
//	virtual TranslatorInterface *getTranslator() = 0;
//	virtual TranslatorInterface *getTranslator( const QString &lang ) = 0;
//	virtual QStringList availableTranslations() = 0;
};

class TranslatorInterface
{
public:
	virtual ~TranslatorInterface() {}
	virtual QString translate( const char *context, const char *source_text, const char *comment = 0, int n = -1 ) const = 0;
	virtual QString lang() const = 0;
	virtual QLocale locale() const = 0;
};

// TODO: SDK 0.3 Make SystemsCity as MUST for every plugin, it guarentees the safety of qutIM's event system
/** 
* @brief Helper static class. It's used to have access to main classes from any place in plugin
* @code
* SystemsCity::PluginSystem()->someMethod();
* QString profile_name = SystemsCity::ProfileName();
* @endcode
*/
class SystemsCity
{
public:
	/** 
	* @brief Get instance of SystemsCity
	* 
	* @return Instance of SystemsCity
	*/
	inline static SystemsCity &instance()
	{
		static SystemsCity city;
		return city;
	}
	/** 
	* @brief Set pointer to PluginSystem
	* 
	* @param plugin_system Pointer to PluginSystem
	*/
	inline void setPluginSystem( PluginSystemInterface *plugin_system )
	{
		m_plugin_system = plugin_system;
		m_icon_manager = m_plugin_system->getIconManager();
	}
	/** 
	* @brief Set profile name
	* 
	* @param profile_name Profile name
	*/
	inline void setProfileName( const QString &profile_name ) { m_profile_name = profile_name; }
	/** 
	* @brief Get pointer to PluginSystem 
	* 
	* @return Pointer to PluginSystem 
	*/
	inline PluginSystemInterface *pluginSystem() { return m_plugin_system; }
	/** 
	* @brief Get pointer to IconManager
	* 
	* @return Pointer to IconManager
	*/
	inline IconManagerInterface *iconManager() { return m_icon_manager; }
	inline TranslatorInterface *translator() { return /*m_plugin_system ? m_plugin_system->getTranslator() : */0; }
	/** 
	* @brief Get profile name
	* 
	* @return profile name
	*/
	inline const QString &profileName() { return m_profile_name; }
	/** 
	* @brief Get pointer to PluginSystem
	* 
	* @return Pointer to PluginSystem
	*/
	inline static PluginSystemInterface *PluginSystem() { return instance().pluginSystem(); }
	/** 
	* @brief Get pointer to IconManager
	* 
	* @return Pointer to IconManager
	*/
	inline static IconManagerInterface *IconManager() { return instance().iconManager(); }
	inline static TranslatorInterface *Translator() { return instance().translator(); }
	/** 
	* @brief Get profile name
	* 
	* @return Profile name
	*/
	inline static const QString &ProfileName() { return instance().profileName(); }
protected:
	inline SystemsCity() : m_plugin_system(0), m_icon_manager(0) { m_layers.fill( 0, InvalidLayer ); }
	inline ~SystemsCity() {}
	PluginSystemInterface *m_plugin_system;
	IconManagerInterface *m_icon_manager;
	QString m_profile_name;
	QVector<LayerInterface *> m_layers;
};

inline bool Event::send()
{
	PluginSystemInterface *plugin_system = SystemsCity::PluginSystem();
	bool result = false;
	if( plugin_system )
		result = plugin_system->sendEvent( *this );
	else
		qWarning( "Event: SystemsCity has no pointer to PluginSystemInterface" );
	return result;
}

inline void EventHandler::destruct_helper()
{
	if( SystemsCity::PluginSystem() )
		SystemsCity::PluginSystem()->removeEventHandler( this );
	else
		qWarning( "EventHandler: SystemsCity has no pointer to PluginSystemInterface" );
}

class LocalizedString
{
public:
	QString toString()
	{
		return toString( SystemsCity::Translator() );
	}
	QString toString( TranslatorInterface *translator )
	{
		if( translator )
			return translator->translate( m_context, m_source_text, m_comment, m_n );
		qWarning( "LocalizedString: SystemsCity has no pointer to PluginSystemInterface" );
		return QCoreApplication::translate( m_context, m_source_text, m_comment, QCoreApplication::UnicodeUTF8, m_n );
	}
	inline operator QString() { return toString(); }
private:
	inline LocalizedString( const char *context, const char *source_text, const char *comment = 0, int n = -1 )
			: m_context(context), m_source_text(source_text), m_comment(comment), m_n(n) {}
	QByteArray m_context;
	QByteArray m_source_text;
	QByteArray m_comment;
	int m_n;
	friend inline LocalizedString sqtr( const char *source_text, const char *comment, int n );
	friend inline LocalizedString sqtrc( const char *context, const char *source_text, const char *comment, int n );
};

inline LocalizedString sqtr( const char *source_text, const char *comment = 0, int n = -1 )
{
	return LocalizedString( 0, source_text, comment, n );
}

inline LocalizedString sqtrc( const char *context, const char *source_text, const char *comment = 0, int n = -1 )
{
	return LocalizedString( context, source_text, comment, n );
}

inline QString qtr( const char *source_text, const char *comment = 0, int n = -1 )
{
	return sqtr( source_text, comment, n ).toString();
}

inline QString qtrc( const char *context, const char *source_text, const char *comment = 0, int n = -1 )
{
	return sqtrc( context, source_text, comment, n ).toString();
}

/*!
 * @brief Abstract plug-in interface, all plug-ins must be inherited from this
 * All the following functions must be realized in plug-in, mention it. 
 */
class PluginInterface
{
public:
	virtual ~PluginInterface() {}

	/*!
	 * @brief initialization of the plug-in
	 * 
	 * @param plugin_systemPlugin system realization
	 * @return Result of initialization, true if all is ok
	 * @see PluginSystemInterface
	 */
	virtual bool init(PluginSystemInterface *plugin_system)
	{
		m_plugin_system = plugin_system;
		SystemsCity::instance().setPluginSystem( plugin_system );
		return true;
	}

	// TODO: SDK 0.3 Add functions for dynamic loading and unloading plugins
	/*!
	 * @brief Check if plugin can be stopped
	 */
//	virtual bool canBeUnloaded() { return true; }

	/*!
	 * @brief Start plugin
	 */
//	virtual void load() = 0;

	/*!
	 * @brief Stop plugin
	 */
//	inline void unload() = 0;

	/*!
	 * @brief release resources
	 */
	virtual void release() = 0;

	// TODO: SDK 0.3 Move all this info to one structure named like PluginInfo,
	// also it is useful to know version and author of plugin

	/*!
	 * @brief The name of the plugin
	 * 
	 * @return QStringPlugin name
	 */
	virtual QString name() = 0;

	/*!
	 * @brief The description of the plugin
	 *
	 * @return QStringPlugin description, valid html
	 */
	virtual QString description() = 0;

	/*!
	 * @brief plugin type
	 *
	 * For protocols it should be "protocol", for layers "layer" and so on
	 *
	 * @return QStringPlugin type
	 */
	virtual QString type() = 0;

	/*!
	 * @brief plugin icon
	 *
	 * @return QIconPlugin icon
	 */
	// TODO: SDK 0.3 Change "QIcon *" to "QIcon", return of QIcon() is slower a bit, but more safe
	virtual QIcon *icon() = 0;

	/*!
	 * @brief Set profile name
	 *
	 * @param profile_name - name to be set
	 */
	// TODO: SDK 0.3 May be profile name should be carried by SystemsCity?..
	virtual void setProfileName(const QString &profile_name) = 0;

protected:
	// TODO: SDK 0.3 Remove this member, SystemsCity has pointer to PluginSystemInterface
	PluginSystemInterface *m_plugin_system; /*!< plugin system instance */
};

class CmdArgsHandler
{
public:
	virtual ~CmdArgsHandler() {}
	virtual void setCmdArgs( int argc, char **argv ) = 0;
};

class SimplePluginInterface : public PluginInterface
{
public:
	/*!
	 * @brief plugin settings widget
	 *
	 * @return QWidget, representing plugin settings window
	 */
	// TODO: SDK 0.3 May be this methods should be named more understandable?
	virtual QWidget *settingsWidget() = 0;
	
	virtual void removeSettingsWidget() = 0;
	virtual void saveSettings() = 0;
};

class DeprecatedSimplePluginInterface : public SimplePluginInterface
{
public:
	/*!
	 * @brief process event
	 *
	 * Processes some event, sent to the plugin
	 *
	 * @attention this function is deprecated. Use qutim_sdk_0_2::EventHandler instead of it
	 * @param event - event, sent to the plugin
	 * @see PluginEvent
	 */
	virtual void QUTIM_DEPRECATED processEvent(PluginEvent &event) = 0;
};

class LayerPluginInterface : public PluginInterface
{
public:
	/*!
	 * @brief plugin type
	 *
	 * Function, returning plugin type.
	 * For layers it must always be "layer"
	 *
	 * @return QString("layer")
	 */
	virtual QString type() { return "layer"; }

	// TODO: SDK 0.3 Add this functions for managing layers
//	virtual bool initLayer( LayerType type ) = 0;
//	virtual QList<LayerType> supportedLayers() = 0;

	// TODO: SDK 0.3 Add functions for dynamic loading and unloading plugins
	/*!
	 * @brief layers can not be stopped, they are a part of the core
	 */
//	virtual bool canBeUnloaded() { return false; }
};

class PluginContainerInterface : public PluginInterface
{
public:
	virtual QString type() { return QLatin1String("container"); }
	// TODO: SDK 0.2 May be more functions?
	virtual QObjectList loadPlugins( const QStringList &plugin_paths ) = 0;

	// TODO: SDK 0.3 Add functions for dynamic loading and unloading plugins
	/*!
	 * @brief plugin container can not be stopped because it's not a simple plugin, but magic one
	 */
//	virtual bool canBeUnloaded() { return false; }
};

#ifndef BUILD_QUTIM
typedef DeprecatedSimplePluginInterface DeprecatedSimplePlugin;
typedef LayerInterface Layer;
typedef LayerPluginInterface LayerPlugin;
typedef PluginSystemInterface PluginSystem;
typedef PluginInterface Plugin;
typedef SimplePluginInterface SimplePlugin;
#endif

}//end namespace qutim_sdk_0_2

//inline qutim_sdk_0_2::LocalizedString qtr( const char *source_text, const char *comment = 0, int n = -1 )
//{
//	return qutim_sdk_0_2::qtr(source_text, comment
//}
//
//inline qutim_sdk_0_2::LocalizedString qtrc( const char *context, const char *source_text, const char *comment = 0, int n = -1 )
//{
//	return qutim_sdk_0_2::LocalizedString( context, source_text, comment, n );
//}

inline static QString tr2qtr( const char *source_text, const char *comment = 0 )
{
	qutim_sdk_0_2::TranslatorInterface *translator = qutim_sdk_0_2::SystemsCity::Translator();
	if( translator )
		return translator->translate( 0, source_text, comment );
	qWarning( "tr2qtr: SystemsCity has no pointer to PluginSystemInterface" );
	return QString::fromUtf8( source_text );
}

/*!
 * @brief Namespace, that will be used in the recent future
 *
 * It's main featureFully compatibility with last namespace,
 * easy ways for migration, more comfortable and flexible coding
 */
namespace qutim_sdk_0_3 {

typedef qutim_sdk_0_2::LocalizedString LocalizedString;

//class SystemsCity
//{
//public:
//	/**
//	* @brief Get pointer to PluginSystem
//	*
//	* @return Pointer to PluginSystem
//	*/
//	inline static PluginSystemInterface *PluginSystem() { return instance().m_plugin_system; }
//	/**
//	* @brief Get pointer to IconManager
//	*
//	* @return Pointer to IconManager
//	*/
//	inline static IconManagerInterface *IconManager() { return instance().m_icon_manager; }
//	inline static TranslatorInterface *Translator() { return instance().m_translator; }
//	/**
//	* @brief Get profile name
//	*
//	* @return Profile name
//	*/
//	inline static const QString &ProfileName() { return instance().m_profile_name; }
//protected:
//	inline static SystemsCity &instance()
//	{
//		static SystemsCity city;
//		return city;
//	}
//	inline SystemsCity() : m_plugin_system(0), m_icon_manager(0), m_translator(0) { m_layers.fill( 0, InvalidLayer ); }
//	inline ~SystemsCity() {}
//	PluginSystemInterface *m_plugin_system;
//	IconManagerInterface *m_icon_manager;
//	TranslatorInterface *m_translator;
//	QString m_profile_name;
//	QVector<LayerInterface *> m_layers;
//};

/*!
 * @brief Event structure
 *
 * It is used for communicating between plugins and core
 */
struct Event
{
	inline Event(quint16 event_id = 0xffff) : id(event_id)  {}
	// auto-generated code
	inline Event(const QString &event_id);
	template< typename T1 >
	inline Event(quint16 event_id, const T1 &t1);
	template< typename T1 >
	inline Event(const QString &event_id, const T1 &t1);
	template< typename T1 , typename T2 >
	inline Event(quint16 event_id, const T1 &t1, const T2 &t2);
	template< typename T1 , typename T2 >
	inline Event(const QString &event_id, const T1 &t1, const T2 &t2);
	template< typename T1 , typename T2 , typename T3 >
	inline Event(quint16 event_id, const T1 &t1, const T2 &t2, const T3 &t3);
	template< typename T1 , typename T2 , typename T3 >
	inline Event(const QString &event_id, const T1 &t1, const T2 &t2, const T3 &t3);
	template< typename T1 , typename T2 , typename T3 , typename T4 >
	inline Event(quint16 event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4);
	template< typename T1 , typename T2 , typename T3 , typename T4 >
	inline Event(const QString &event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4);
	template< typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
	inline Event(quint16 event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5);
	template< typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
	inline Event(const QString &event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5);
	template< typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
	inline Event(quint16 event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6);
	template< typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
	inline Event(const QString &event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6);
	// end of auto-generation

	template< typename T >
	inline const T &at(int i) const { return *reinterpret_cast<const T *>(args.at(i)); } // usually you don't need it
	template< typename T >
	inline T &at(int i) { return *reinterpret_cast<T *>(args[i]); }
	template< typename T >
	inline void append( const T &t ) { args.append(const_cast<T *>(&t)); }
	inline int size() { return args.size(); }
	inline bool send();
	inline operator qutim_sdk_0_2::Event &();
	inline operator const qutim_sdk_0_2::Event &() const;
	quint16 id;           /*!< Event type id */
	QVector<void *> args; /*!< List with pointers to event's arguments */
};

inline bool Event::send()
{
	return qutim_sdk_0_2::SystemsCity::PluginSystem()->sendEvent( *this );
}

struct AuthorInfo
{
	LocalizedString name;
	LocalizedString task;
	QString email_address;
	QString web_address;
};

struct PluginInfo
{
	// Localized authors
	QList<AuthorInfo> authors;
	// Localized name
	LocalizedString name;
	// Localized html description
	LocalizedString description;
	// Can be accessed by Icon( name, IconInfo::Plugin );
	QIcon icon;
	// Format is w[.x[.y[.z]]]
	QString version;
};

/*!
 * @brief item of the TreeModel, used to represent contact, message or somewhat
 */
struct TreeModelItem
{
    enum Type
    {
        Buddy = 0,
        Group,
        Account,
        Conference = 32,
        ConferenceItem
	};
	inline TreeModelItem() : type(0) {}
    QString protocol; /*!< Protocol name */
    QString account; /*!< Account name */
	QString name; /*!< Item name */
    QString parent; /*!< Parent item name */
	quint8  type; /*!< Item type. Enum is not used because of it's different size at different compilators and platforms */
	QString history;
	inline operator qutim_sdk_0_2::TreeModelItem &();
	inline operator const qutim_sdk_0_2::TreeModelItem &() const;
};
/*!
 * @brief item of the history
 *
 * In a few words - one separate message
 * HistoryItem structure is not fully understood by me, look at m_user and m_from
 */
struct HistoryItem
{
	inline HistoryItem() : in(true), type(0) {}
	QString message; /*!< Message */
	QDateTime time; /*!< Time of sending */
	TreeModelItem user; /*!< Contact's item */
	QString _unused; /*!< Deprecated */
	bool in;  /*!< FlagIncoming (true) or outgoing (false) message */
	qint8 type; /*!< 0 - system; 1Private; */
	inline operator qutim_sdk_0_2::HistoryItem &();
	inline operator const qutim_sdk_0_2::HistoryItem &() const;
};

// auto-generated code
inline Event::Event(const QString &event_id) : id(qutim_sdk_0_2::SystemsCity::PluginSystem()->registerEventHandler(event_id)) {}
template< typename T1 >
inline Event::Event(quint16 event_id, const T1 &t1) : id(event_id)
{
	args.reserve(1);
	args << const_cast<T1 *>( &t1 );
}
template< typename T1 >
inline Event::Event(const QString &event_id, const T1 &t1) : id(qutim_sdk_0_2::SystemsCity::PluginSystem()->registerEventHandler(event_id))
{
	args.reserve(1);
	args << const_cast<T1 *>( &t1 );
}
template< typename T1 , typename T2 >
inline Event::Event(quint16 event_id, const T1 &t1, const T2 &t2) : id(event_id)
{
	args.reserve(2);
	args << const_cast<T1 *>( &t1 ) << const_cast<T2 *>( &t2 );
}
template< typename T1 , typename T2 >
inline Event::Event(const QString &event_id, const T1 &t1, const T2 &t2) : id(qutim_sdk_0_2::SystemsCity::PluginSystem()->registerEventHandler(event_id))
{
	args.reserve(2);
	args << const_cast<T1 *>( &t1 ) << const_cast<T2 *>( &t2 );
}
template< typename T1 , typename T2 , typename T3 >
inline Event::Event(quint16 event_id, const T1 &t1, const T2 &t2, const T3 &t3) : id(event_id)
{
	args.reserve(3);
	args << const_cast<T1 *>( &t1 ) << const_cast<T2 *>( &t2 ) << const_cast<T3 *>( &t3 );
}
template< typename T1 , typename T2 , typename T3 >
inline Event::Event(const QString &event_id, const T1 &t1, const T2 &t2, const T3 &t3) : id(qutim_sdk_0_2::SystemsCity::PluginSystem()->registerEventHandler(event_id))
{
	args.reserve(3);
	args << const_cast<T1 *>( &t1 ) << const_cast<T2 *>( &t2 ) << const_cast<T3 *>( &t3 );
}
template< typename T1 , typename T2 , typename T3 , typename T4 >
inline Event::Event(quint16 event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4) : id(event_id)
{
	args.reserve(4);
	args << const_cast<T1 *>( &t1 ) << const_cast<T2 *>( &t2 ) << const_cast<T3 *>( &t3 ) << const_cast<T4 *>( &t4 );
}
template< typename T1 , typename T2 , typename T3 , typename T4 >
inline Event::Event(const QString &event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4) : id(qutim_sdk_0_2::SystemsCity::PluginSystem()->registerEventHandler(event_id))
{
	args.reserve(4);
	args << const_cast<T1 *>( &t1 ) << const_cast<T2 *>( &t2 ) << const_cast<T3 *>( &t3 ) << const_cast<T4 *>( &t4 );
}
template< typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
inline Event::Event(quint16 event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5) : id(event_id)
{
	args.reserve(5);
	args << const_cast<T1 *>( &t1 ) << const_cast<T2 *>( &t2 ) << const_cast<T3 *>( &t3 ) << const_cast<T4 *>( &t4 ) << const_cast<T5 *>( &t5 );
}
template< typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
inline Event::Event(const QString &event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5) : id(qutim_sdk_0_2::SystemsCity::PluginSystem()->registerEventHandler(event_id))
{
	args.reserve(5);
	args << const_cast<T1 *>( &t1 ) << const_cast<T2 *>( &t2 ) << const_cast<T3 *>( &t3 ) << const_cast<T4 *>( &t4 ) << const_cast<T5 *>( &t5 );
}
template< typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
inline Event::Event(quint16 event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6) : id(event_id)
{
	args.reserve(6);
	args << const_cast<T1 *>( &t1 ) << const_cast<T2 *>( &t2 ) << const_cast<T3 *>( &t3 ) << const_cast<T4 *>( &t4 ) << const_cast<T5 *>( &t5 ) << const_cast<T6 *>( &t6 );
}
template< typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
inline Event::Event(const QString &event_id, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6) : id(qutim_sdk_0_2::SystemsCity::PluginSystem()->registerEventHandler(event_id))
{
	args.reserve(6);
	args << const_cast<T1 *>( &t1 ) << const_cast<T2 *>( &t2 ) << const_cast<T3 *>( &t3 ) << const_cast<T4 *>( &t4 ) << const_cast<T5 *>( &t5 ) << const_cast<T6 *>( &t6 );
}
// end of auto-generation

}//end namespace qutim_sdk_0_3

inline qutim_sdk_0_2::Event::operator qutim_sdk_0_3::Event &()
{ return *reinterpret_cast<qutim_sdk_0_3::Event *>( this ); }

inline qutim_sdk_0_2::Event::operator const qutim_sdk_0_3::Event &() const
{ return *reinterpret_cast<const qutim_sdk_0_3::Event *>( this ); }

inline qutim_sdk_0_3::Event::operator qutim_sdk_0_2::Event &()
{ return *reinterpret_cast<qutim_sdk_0_2::Event *>( this ); }

inline qutim_sdk_0_3::Event::operator const qutim_sdk_0_2::Event &() const
{ return *reinterpret_cast<const qutim_sdk_0_2::Event *>( this ); }

inline qutim_sdk_0_2::TreeModelItem::operator qutim_sdk_0_3::TreeModelItem &()
{ return *reinterpret_cast<qutim_sdk_0_3::TreeModelItem *>( this ); }

inline qutim_sdk_0_2::TreeModelItem::operator const qutim_sdk_0_3::TreeModelItem &() const
{ return *reinterpret_cast<const qutim_sdk_0_3::TreeModelItem *>( this ); }

inline qutim_sdk_0_3::TreeModelItem::operator qutim_sdk_0_2::TreeModelItem &()
{ return *reinterpret_cast<qutim_sdk_0_2::TreeModelItem *>( this ); }

inline qutim_sdk_0_3::TreeModelItem::operator const qutim_sdk_0_2::TreeModelItem &() const
{ return *reinterpret_cast<const qutim_sdk_0_2::TreeModelItem *>( this ); }

inline qutim_sdk_0_2::HistoryItem::operator qutim_sdk_0_3::HistoryItem &()
{ return *reinterpret_cast<qutim_sdk_0_3::HistoryItem *>( this ); }

inline qutim_sdk_0_2::HistoryItem::operator const qutim_sdk_0_3::HistoryItem &() const
{ return *reinterpret_cast<const qutim_sdk_0_3::HistoryItem *>( this ); }

inline qutim_sdk_0_3::HistoryItem::operator qutim_sdk_0_2::HistoryItem &()
{ return *reinterpret_cast<qutim_sdk_0_2::HistoryItem *>( this ); }

inline qutim_sdk_0_3::HistoryItem::operator const qutim_sdk_0_2::HistoryItem &() const
{ return *reinterpret_cast<const qutim_sdk_0_2::HistoryItem *>( this ); }

/*
 * qutIM defines the current namespace
 */
#ifndef BUILD_QUTIM
#  ifndef qutIM
#    define qutIM qutim_sdk_0_2
#  endif
#  ifdef QUTIM
#    undef QUTIM
#  endif
#  define QUTIM qutIM
#endif

Q_DECLARE_INTERFACE(qutim_sdk_0_2::CmdArgsHandler, "org.qutim.cmdargshandler/0.2");
Q_DECLARE_INTERFACE(qutim_sdk_0_2::PluginInterface, "org.qutim.plugininterface/0.2");
Q_DECLARE_INTERFACE(qutim_sdk_0_2::PluginContainerInterface, "org.qutim.plugincontainerinterface/0.2");
Q_DECLARE_INTERFACE(qutim_sdk_0_2::SimplePluginInterface, "org.qutim.simpleplugininterface/0.2");
Q_DECLARE_INTERFACE(qutim_sdk_0_2::LayerPluginInterface, "org.qutim.layerplugininterface/0.2");
Q_DECLARE_INTERFACE(qutim_sdk_0_2::DeprecatedSimplePluginInterface, "org.qutim.deprecatedplugininterface/0.2");


#endif //#ifndef QUTIM_SDK_PLUGININTERFACE_H
