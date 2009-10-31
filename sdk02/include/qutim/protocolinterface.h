/*!
 * ProtocolInteface
 *
 * @author m0rph
 * @author Rustam Chakin
 * Copyright (c) 2008 by m0rph <m0rph.mailbox@gmail.com>,
 * 			             Rustam Chakin <qutim.develop@gmail.com>
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
#ifndef QUTIM_SDK_PROTOCOLINTERFACE_H
#define QUTIM_SDK_PROTOCOLINTERFACE_H

#include <qutim/plugininterface.h>

class QMenu;
class QHBoxLayout;
class QAction;

namespace qutim_sdk_0_2 {

/*!
 * @brief Account structure
 *
 * Account name, protocol used and icon
 */
struct AccountStructure
{
	QIcon protocol_icon; /*!< Protocol icon */
	QString protocol_name; /*!< Protocol name */
	QString account_name; /*!< Account name */
};
/*!
 * @brief protocol implementation interface
 *
 * PluginInteface extension
 * It's aimed to implement protocols
 *
 * @see PluginInterface
 */
// TODO: SDK 0.3 Remove most of functions, move them to Event system
class ProtocolInterface : public PluginInterface
{
public:
	/*!
	 * @brief plugin type
	 *
	 * Function, returning plugin type.
	 * For protocols it must always be "protocol"
	 *
	 * @return QString("protocol")
	 */
	virtual QString type() { return "protocol"; }

	// TODO: SDK 0.3 Add functions for dynamic loading and unloading plugins
	/*!
	 * @brief check if plugin can be stopped
	 */
//	virtual bool canBeUnloaded() { return false; }

	/*!
	 * @brief remove target account, using this protocol
	 *
	 * @param account_name - name of the account
	 */
	virtual void removeAccount(const QString &account_name) = 0;

	/*!
	 * @brief widget, representing login window
	 *
	 * @return QWidget
	 */
	virtual QWidget *loginWidget() = 0;

	/*!
	 * @brief remove login widget after usage
	 */
	virtual void removeLoginWidget() = 0;

	/*!
	 * @brief called, when "OK" or "Apply" button is pressed in the settings dialog
	 */
	virtual void applySettingsPressed() = 0;

	/*!
	 * @brief protocol statuses menu
	 *
	 * Function for getting protocol statuses menu from all accounts
	 *
	 * @return QList<QMenu*> statuses menu
	 */
	virtual QList<QMenu*> getAccountStatusMenu() = 0;

	/*!
	 * @brief add stuff to target layout
	 *
	 * Protocol plugin adds account button, status menu etc. to this layout
	 *
	 * @param QHBoxLayout* - layout, which is to be used
	 */
	virtual void addAccountButtonsToLayout(QHBoxLayout *) = 0;

	/*!
	 * @brief save all login data from login widget
	 */
	virtual void saveLoginDataFromLoginWidget() = 0;

	/*!
	 * @brief protocol settings widget and some additional data
	 *
	 * Return SettingsStructure QList with protocol settings and
	 * some other stuff (AntiSpam etc.)
	 *
	 * @return QList<SettingsStructure> with all settings
	 * @see SettingsStructure
	 */
	virtual QList<SettingsStructure> getSettingsList() = 0;

	/*!
	 * @brief remove protocol settings from memory
	 */
	virtual void removeProtocolSettings()  = 0;

	/*!
	 * @brief return account list of this protocol
	 *
	 * @return QList<AccountStructure>
	 * @see AccountStructure
	 */
	virtual QList<AccountStructure> getAccountList() = 0;
	
	/*!
	 * @brief get current statuses for all accounts of this protocol
	 *
	 * @return QList<AccountStructure>
	 * @see AccountStructure
	 */
	virtual QList<AccountStructure> getAccountStatuses() = 0;

	/*!
	 * @brief set auto-away status
	 *
	 * This func is called by kernel when PC is idle
	 */
	virtual void setAutoAway() = 0;

	/*!
	 * @brief set status when user returns from auto-away
	 *
	 * This func is called by kernel when PC returnes from idle
	 */
	virtual void setStatusAfterAutoAway() = 0;
	
	/*!
	 * @brief action on double-click on item
	 *
	 * This func is called by kernel when user double-clicks on contact
	 *
	 * @param account_name - name of the account to use
	 * @param contact_name - contact activated
	 */
	// TODO: SDK 0.3 remove, it isn't used now
	virtual void itemActivated(const QString &account_name, const QString &contact_name) = 0;

	/*!
	 * @brief action on right-click on item
	 * 
	 * This func is called by kernel when user right-clicks on contact
	 * It raises context menu
	 *
	 * @param account_name - account to use
	 * @param contact_name - contact clicked
	 * @param item_type - item type: 0 - buddy, 1 - group, 2 - account
	 * @param menu_point - place, where to raise a menu
	 */
	virtual void itemContextMenu(const QList<QAction*> &action_list,
			const QString &account_name, const QString &contact_name, int item_type, const QPoint &menu_point) = 0;

	/*!
	 * @brief send message to contact
	 *
	 * @param account_name - name of the sender account
	 * @param contact_name - name of the target contact
	 * @param item_type - item type: 0 - buddy; 1 - group; 2 - account
	 * @param message - message to send
	 * @param message_icon_position - internal message number, got from kernel
	 */
	virtual void sendMessageTo(const QString &account_name, const QString &contact_name, int item_type, const QString& message, int message_icon_position) = 0;

	/*!
	 * @brief get some additional info about contact
	 *
	 * @param account_name - account to use
	 * @param item_name - contact to get info about
	 * @param item_type - item type: 0 - buddy; 1 - group; 2 - account 
	 * @return QStringList with additional info
	 */
	virtual QStringList getAdditionalInfoAboutContact(const QString &account_name, const QString &item_name, int item_type ) const = 0;

	/*!
	 * @brief show information about target contact
	 *
	 * @param account_name - account to use
	 * @param item_name - target contact
	 * @param item_type - item type: 0 - buddy; 1 - group; 2 - account
	 */
	virtual void showContactInformation(const QString &account_name, const QString &item_name, int item_type ) = 0;

	/*!
	 * @brief send raw image to target contact
	 *
	 * @param account_name - account to use
	 * @param item_name - target contact
	 * @param item_type - item type: 0 - buddy; 1 - group; 2 - account
	 * @param image_raw - raw image to send
	 */
	virtual void sendImageTo(const QString &account_name, const QString &item_name, int item_type, const QByteArray &image_raw ) = 0;

	/*!
	 * @brief send file to target contact
	 *
	 * @param account_name - account to use
	 * @param item_name - target contact
	 * @param item_type - item type: 0 - buddy; 1 - group; 2 - account item
	 */
        virtual void sendFileTo(const QString &account_name, const QString &item_name, int item_type, const QStringList &file_names) = 0;

	/*!
	 * @brief send notitication about typing
	 *
	 * @param account_name - account to use
	 * @param item_name - contact, to whom send a notification
	 * @param item_type - item type: 0 - buddy, 1 - group, 2 - account
	 * @param notification_type - type of notification: 0 - typing stop; 1 - typing; 2 - typing start
	 */
	virtual void sendTypingNotification(const QString &account_name, const QString &item_name, int item_type, int notification_type) = 0;

	/*
	 * @brief signal about item in CL movement
	 *
	 * This func is called, when item is drag'n'dropped inside the CL
	 *
	 * @param old_item - item's old state
	 * @param new_item - item's new state
	 */
	virtual void moveItemSignalFromCL(const TreeModelItem &old_item, const TreeModelItem &new_item) = 0;

	/*!
	 * @brief get html tooltip for the contact under mouse
	 *
	 * This func is called, when pointer is over contact, it gets html tooltip for it
	 *
	 * @param account_name - account to use
	 * @param contact_name - contact, for which tooltip is generated
	 * @return QString with html info
	 */
	virtual QString getItemToolTip(const QString &account_name, const QString &contact_name) = 0;

	/*!
	 * @brief signal about item deletion
	 *
	 * This func is called, when contact is deleted from CL
	 *
	 * @param account_name - account to use
	 * @param item_name - deleted item name
	 * @param type - item type: 0 - buddy, 1 - group, 2 - contact
	 */
	virtual void deleteItemSignalFromCL(const QString &account_name, const QString &item_name, int type) = 0;

	/*!
	 * @brief signal about chat window has been opened
	 *
	 * This func is called, when user starts chat with somebody
	 *
	 * @param account_name - account to use
	 * @param item_name - contact with whom chat has been started
	 */
	virtual void chatWindowOpened(const QString &account_name, const QString &item_name) = 0;

	/*!
	 * @brief signal about chat window is going to be opened
	 *
	 * This func is called, just before user starts chat with somebody
	 *
	 * @param account_name - account to use
	 * @param item_name - contact with whom chat has been started
	 */
	virtual void chatWindowAboutToBeOpened(const QString &account_name, const QString &item_name) = 0;

	/*!
	 * @brief signal about chat window has been closed
	 *
	 * This func is called, when user stops chat with somebody
	 *
	 * @param account_name - account to use
	 * @param item_name - contact with whom chat has been started
	 */
	virtual void chatWindowClosed(const QString &account_name, const QString &item_name) = 0;

	/*!
	 * @brief send message to target conference
	 *
	 * @param conference_name - conference, which is message sent to
	 * @param account_name - account, sending message
	 * @param message - message to send
	 */
	virtual void sendMessageToConference(const QString &conference_name, const QString &account_name,
			const QString &message) = 0;

	/*!
	 * @brief leave target conference
	 *
	 * @param conference_name - conference to leave
	 * @param account_name - account, which must leave conference
	 */
	virtual void leaveConference(const QString &conference_name, const QString &account_name) = 0;
	
	/*!
	 * @brief action on double-click on item from conference
	 *
	 * This func is called by kernel when user double-clicks on contact
	 *
	 * @param conference_name - name of the conference
	 * @param account_name - name of the account
	 * @param nickname - nickname of activated contact
	 */
	virtual void conferenceItemActivated(const QString &conference_name, const QString &account_name, 
			const QString &nickname) = 0;

	/*!
	 * @brief action on right-click on item from conference
	 * 
	 * This func is called by kernel when user right-clicks on contact
	 * It raises context menu
	 *
	 * @param conference_name - name of the conference
	 * @param account_name - name of the account
	 * @param nickname - nickname of clicked contact
	 * @param menu_point - place, where to raise a menu
	 */
	virtual void conferenceItemContextMenu(const QList<QAction*> &action_list, const QString &conference_name,
			const QString &account_name, const QString &nickname, const QPoint &menu_point) = 0;

	/*!
	 * @brief get html tooltip for the conference's contact under mouse
	 *
	 * This func is called, when pointer is over contact, it gets html tooltip for it
	 *
	 * @param conference_name - name of the conference
	 * @param account_name - name of the account
	 * @param nickname - nickname of contact, for which tooltip is generated
	 * @return QString with html info
	 */
	virtual QString getConferenceItemToolTip(const QString &conference_name, const QString &account_name, const QString &nickname) = 0;

	/*!
	 * @brief show information about target contact
	 *
	 * @param conference_name - name of the conference
	 * @param account_name - name of the account
	 * @param nickname - nickname of contact
	 */
	virtual void showConferenceContactInformation(const QString &conference_name, const QString &account_name, const QString &nickname) = 0;
	
	virtual void showConferenceTopicConfig(const QString &conference_name, const QString &account_name) = 0;
	virtual void showConferenceMenu(const QString &conference_name, const QString &account_name, 
			const QPoint &menu_point) = 0;
	virtual void getMessageFromPlugins(const QList<void *> &event) = 0;
	virtual void editAccount(const QString &account_name) = 0;
};

}//end namespace qutim_sdk_0_2


Q_DECLARE_INTERFACE(qutim_sdk_0_2::ProtocolInterface, "org.qutim.protocolinterface/0.2");

#endif //#ifndef QUTIM_SDK_PROTOCOLINTERFACE_H
