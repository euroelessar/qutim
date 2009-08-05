/*****************************************************************************
    Plugin System

    Copyright (c) 2008 by m0rph <m0rph.mailbox@gmail.com>
			 2008-2009 by Rustam Chakin <qutim.develop@gmail.com>
						  Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef PLUGINSYSTEM_H
#define PLUGINSYSTEM_H

#include <QHash>
#include <QList>
#include <QIcon>
#include <QPluginLoader>
#include <QMenu>
#include <QDateTime>
#include <QAction>
#include "../include/qutim/plugininterface.h"
#include "../include/qutim/protocolinterface.h"
#include "../include/qutim/layerinterface.h"
#include "pluginpointer.h"

using namespace qutim_sdk_0_2;

struct PluginInfo
{
	QString name;
	QString description;
	QString type;
	QIcon icon;
};

typedef QList<PluginInfo>               PluginInfoList;
typedef QList<PluginPointer>            PluginList;
typedef QList<QPointer<QPluginLoader> > QPluginLoaderList;

typedef QList<SimplePluginPointer> SimplePluginList;
//! List of protocol plug-ins, which is sorted by protocol name.
typedef QHash<QString, ProtocolPointer> ProtocolList;

//! There QString is a type of message, which can be processed from plug-ins of 'SimplePluginList'
typedef QHash<QString, SimplePluginList>	PluginEventLists;

class AbstractLayer;

class PluginSystem : public QObject, public PluginSystemInterface
{
	Q_OBJECT
public:
	static PluginSystem &instance();
	void init();
	void aboutToQuit();

	struct EventId;
	PluginInterface *loadPlugin(QObject *instance);
	QObjectList loadPlugins(int argc, char *argv[]);
	void initPlugins( QObjectList plugins );
	bool unloadPlugin(const QString &name);
	bool unloadPlugin(PluginInterface *object);
	int pluginsCount();
	SimplePluginInterface *getPluginByIndex(int index);
	PluginInfoList getPluginsByType(const QString &type);
	virtual bool setLayerInterface(LayerType type, LayerInterface *interface);
	LayerInterface *getLayerInterface(LayerType type);
	void initiateLayers(const QString &profile_name);
	void processEvent(PluginEvent &event);
	void registerEventHandler(const EventType &type, DeprecatedSimplePluginInterface *plugin);
	void releaseEventHandler(const QString &event_id, PluginInterface *plugin);
	QWidget *getLoginWidget(const QString &protocol_name);
	void removeLoginWidgetByName(const QString &protocol_name);
	void loadProfile(const QString &profile_name);
	//! It removes account with 'account_name' name by protocol with 'protocol_name' name
	//! \example plugin_system->removeAccoun("jabber", "qutimdevelop@jabber.ru");
	void removeAccount(const QString &protocol_name, const QString &account_name);
	//This plugin - system function for protocols will be emited if button 'OK' or 'Apply'
	//from settings dialog was pressed.
	void applySettingsPressed(const QString &protocol_name);
	//Plugin - system function for getting protocol statuses menu from all accounts and all protocols
	QList<QMenu*> addStatusMenuToTrayMenu();
	//Plugin - system function for giving QHBoxLayout pointer to protocol plugin.
	//Protocol plugin must add account button with status menu and etc to this layout.
	void addAccountButtonsToLayout(QHBoxLayout *);
	//Plugin - system function for saving all login protocol data from login widget.
	void saveLoginDataByName(const QString &);
	//Plugin - system function for getting protocol settings list by name.
	QList<SettingsStructure> getSettingsByName(const QString &) const;
	//Plugin - system function for deleting protocol settings from memory by name.
	void removeProtocolSettingsByName(const QString &);
	//Function return all accounts list from all loaded protocols
	QList<AccountStructure> getAccountsList() const;
	//Function return all accounts status icons list from all loaded protocols
	QList<AccountStructure> getAccountsStatusesList() const;
	//Function for updating status icon from accounts
	void updateStatusIcons();
	//Function notify about auto-away to all protocols
	void setAutoAway();
	//Function notify about changing status from auto-away to all protocols
	void setStatusAfterAutoAway();
	//Function for adding item to contact list
	bool addItemToContactList(const TreeModelItem &item, const QString &name = QString());
	//Function for removing item from contact list
	bool removeItemFromContactList(const TreeModelItem &item);
	//Function for moving item in contact list
	bool moveItemInContactList(const TreeModelItem &old_item, const TreeModelItem &new_item);
	//Function for renaming contact
	bool setContactItemName(const TreeModelItem &item, const QString &name);
	//Function for adding igon to contact (1,2 - left side; 3...12 - right side)
	bool setContactItemIcon(const TreeModelItem &item, const QIcon &icon, int position);
	//Function for adding text row below contact ( 1..3)
	bool setContactItemRow(const TreeModelItem &item, const QList<QVariant> &row, int position);
	//Function for settings contact status, text - status name for adium icons packs,
	//mass status mass, 1000 - offline, 999-offline separator, -1 - online separator
	bool setContactItemStatus(const TreeModelItem &item, const QIcon &icon, const QString &text, int mass);

	void itemActivated(const TreeModelItem &Item);
	
	void itemContextMenu(const QList<QAction*> &action_list,
			const TreeModelItem &item, const QPoint &menu_point);
	
	bool setStatusMessage(QString &status_message, bool &dshow);
	
	virtual void sendMessageToContact(const TreeModelItem &item, QString &message, int message_icon_position);
	void addMessageFromContact(const TreeModelItem &item, const QString &message, const QDateTime &message_date = QDateTime::currentDateTime());
	void addServiceMessage(const TreeModelItem &item, const QString &message);
	QStringList getAdditionalInfoAboutContact(const TreeModelItem &item);
	void showContactInformation(const TreeModelItem &item);
	void sendImageTo(const TreeModelItem &item, const QByteArray &image_raw);
	void addImage(const TreeModelItem &item, const QByteArray &image_raw);
        void sendFileTo(const TreeModelItem &item, const QStringList &file_names);
	void sendTypingNotification(const TreeModelItem &item, int notification_type);
	void contactTyping(const TreeModelItem &item, bool typing);
	void messageDelievered(const TreeModelItem &item, int message_position);
	void createChat(const TreeModelItem &item);
	bool checkForMessageValidation(const TreeModelItem &item, const QString &message,
			int message_type, bool special_status);
	void notifyAboutBirthDay(const TreeModelItem &item);
	void systemNotification(const TreeModelItem &item, const QString &message);
	void userNotification(TreeModelItem item, const QString, int);
	void customNotification(const TreeModelItem &item, const QString &message);
	QString getIconFileName(const QString & icon_name);
	QIcon getIcon(const QString & icon_name);
	QString getStatusIconFileName(const QString & icon_name, const QString & default_path);
	QIcon getStatusIcon(const QString & icon_name, const QString & default_path);
	void setAccountIsOnline(const TreeModelItem &Item, bool online);
	//do not forget to implement all below in ICQ
	void moveItemSignalFromCL(const TreeModelItem &old_item, const TreeModelItem &new_item);
	QString getItemToolTip(const TreeModelItem &item);
	void deleteItemSignalFromCL(const TreeModelItem& item);
	void getQutimVersion(quint8 &major, quint8 &minor, quint8 &secminor, quint16 &svn);

	void createConference(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name);
	void addMessageToConference(const QString &protocol_name,
			const QString &conference_name, const QString &account_name,
			const QString &from,
			const QString &message, const QDateTime &date, bool history);
	void sendMessageToConference(const QString &protocol_name,
			const QString &conference_name, const QString &account_name,
			const QString &message);
	void changeOwnConferenceNickName(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname);
	virtual void setConferenceTopic(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &topic);
	virtual void addSystemMessageToConference(const QString &protocol_name,
			const QString &conference_name, const QString &account_name,
			const QString &message, const QDateTime &date, bool history);
	void leaveConference(const QString &protocol_name,
			const QString &conference_name, const QString &account_name);
	void addConferenceItem(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname);
	void removeConferenceItem(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname);
	void renameConferenceItem(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname,
			const QString &new_nickname);
	void setConferenceItemStatus(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname,
			const QIcon &icon, const QString &status, int mass);
	void setConferenceItemIcon(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname,
			const QIcon &icon, int position);
	virtual void setConferenceItemRole(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname,
			const QIcon &icon, const QString &role, int mass);	
	virtual QStringList getConferenceItemsList(const QString &protocol_name,
			const QString &conference_name, const QString &account_name);
	virtual void conferenceItemActivated(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname);
	virtual void conferenceItemContextMenu(const QList<QAction*> &action_list,
			const QString &protocol_name, const QString &conference_name,
			const QString &account_name, const QString &nickname, const QPoint &menu_point);
	virtual QString getConferenceItemToolTip(const QString &protocol_name,
			const QString &conference_name,
			const QString &account_name,
			const QString &nickname);
	virtual void showConferenceContactInformation(const QString &protocol_name, const QString &conference_name,
			const QString &account_name, const QString &nickname);
	virtual void setItemVisible(const TreeModelItem &item, bool visible);
	virtual void setItemInvisible(const TreeModelItem &item, bool invisible);
	virtual void registerContactMenuAction(QAction *plugin_action, DeprecatedSimplePluginInterface *pointer_this = 0);
	void removePluginsSettingsWidget();
	void saveAllPluginsSettings();
	void setCurrentContextItemToPlugins(const TreeModelItem &item);
	virtual void sendCustomMessage(const TreeModelItem &item, const QString &message, bool silent = false);
	virtual void registerMainMenuAction(QAction *menu_action);
	void showTopicConfig(const QString &protocol_name, const QString &account_name, const QString &conference);
	void showConferenceMenu(const QString &protocol_name, const QString &account_name, const QString &conference_name,
			const QPoint &menu_point);
	virtual void redirectEventToProtocol(const QStringList &protocol_name, const QList<void*> &event);
	void sendMessageBeforeShowing(const TreeModelItem &item, QString &message);
	void pointersAreInitialized();
	void playSoundByPlugin(QString path);
	virtual void playSound(NotificationType event);
	virtual void playSound(const QString &file_name);
	void receivingMessageBeforeShowing(const TreeModelItem &item, QString &message);
	virtual QSettings::Format plistFormat(){ return m_plist_format; }

	void editAccount(const QString &protocol_name, const QString &account_name);
	virtual bool changeChatWindowID(const TreeModelItem &item, const QString &id);
	virtual quint16 registerEventHandler(const QString &event_id, EventHandler *handler = 0, quint16 priority = EventHandler::NormalPriority);
	virtual void removeEventHandler(quint16 id, EventHandler *handler);
	virtual bool sendEvent(Event &event);
	virtual void getSystemInfo(QString &version, QString &timezone, int &timezone_offset);

	virtual void setTrayMessageIconAnimating(bool animate);

	quint16 registerEventHandler(const QString &event_id, EventHandler *handler, EventHandlerFunc member, quint16 priority = EventHandler::NormalPriority);
	void removeEventHandler(quint16 id, EventHandler *handler, EventHandlerFunc member);
	quint16 registerEventHandler(const QString &event_id, ProcessEventFunc func, quint16 priority = EventHandler::NormalPriority);
	void removeEventHandler(quint16 id, ProcessEventFunc func);
	quint16 registerEventHandler(const QString &event_id, QObject *object, const char *member, quint16 priority = EventHandler::NormalPriority );
	void removeEventHandler(quint16 id, QObject *object, const char *member = 0);

	void removeEventHandler( EventHandler *handler );
	void removeEventHandler( ProcessEventFunc func );
	void removeEventHandler( EventHandler *handler, EventHandlerFunc member );
	void removeEventHandler( QObject *object );

	IconManagerInterface *getIconManager();


	virtual void chatWindowOpened(const TreeModelItem &item);
	virtual void chatWindowAboutToBeOpened(const TreeModelItem &item);
	virtual void chatWindowClosed(const TreeModelItem &item);

	QStringList getSharePaths() { return m_share_paths; }
	void appendSharePath( const QString &path ) { if( !m_share_paths.contains( path ) ) m_share_paths << path; }
	void removeSharePath( const QString &path ) { m_share_paths.removeOne( path ); }

	TranslatorInterface *getTranslator();
	TranslatorInterface *getTranslator( const QString &lang );
	QStringList availableTranslations();
	virtual SettingsInterface *getSettings( const TreeModelItem &item );
	virtual SettingsInterface *getSettings( const QString &name );
	virtual QString getProfilePath();
	virtual QDir getProfileDir();
	virtual void centerizeWidget(QWidget *widget);
	virtual QList<TreeModelItem> getItemChildren(const TreeModelItem &item = TreeModelItem());
	virtual void setItemVisibility(const TreeModelItem &item, int flags);
public slots:
	void showCL();
private:
	PluginSystem();
	~PluginSystem();
	PluginSystem(const PluginSystem &);
	QObjectList findPlugins(const QString &path, const QStringList &paths);
	QSettings::Format m_plist_format;
	ProtocolList m_protocols; //!< it has all protocol plug-ins
	PluginList m_plugins; //!< it is all non-protocol plug-ins
	SettingsLayerInterface *m_settings;

	PluginEventLists m_event_lists;
	QString m_profile_name;
	AbstractLayer &m_abstract_layer;
	QMultiHash<EventType, DeprecatedSimplePluginInterface *> m_registered_events_plugins;
	QHash<LayerType, LayerInterface *> m_layer_interfaces;
	struct Track
	{
		enum Type
		{
			Interface,
			Method,
			Func,
			Slot,
			Signal
		};
		inline Track( EventHandler *handler_ ) : type(Interface), handler(handler_), method(0), func(0), object(0), slot_id(-1) {}
		inline Track( EventHandler *handler_, EventHandlerFunc method_ ) : type(Method), handler(handler_), method(method_), func(0), object(0), slot_id(-1) {}
		inline Track( ProcessEventFunc func_ ) : type(Func), handler(0), method(0), func(func_), object(0), slot_id(-1) {}
		inline Track( QObject *object_, const char *member)
					: type((*member-'0')==QSLOT_CODE?Slot:Signal), handler(0), method(0), func(0), object(object_),
					slot_id(type==Slot?object->metaObject()->indexOfSlot(member+1):object->metaObject()->indexOfSignal(member+1)) {}
		const Type type;
		EventHandler *handler;
		EventHandlerFunc method;
		ProcessEventFunc func;
		QPointer<QObject> object;
		int slot_id;
	};
	typedef QPair<quint16, Track *> EventHandlerInfo;
	QVector<QVector<EventHandlerInfo> > m_event_handlers;
	QHash<QString, quint16> m_events_id;
	quint16 m_new_event_id;
	EventId *m_events;
	QStringList m_share_paths;
};


#endif
