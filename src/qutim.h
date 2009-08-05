/*
    qutim

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
				  2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#ifndef QUTIM_H
#define QUTIM_H

//#include <memory>
#include <QObject>
#include <QTranslator>
#include <QSettings>
#include <QFile>
#include "../include/qutim/layerinterface.h"

#include "iconmanager.h"
#include "pluginsettings.h"
//#include "ui_qutim.h"
#include "idle/idle.h"
//#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
//	#include "ex/exsystrayicon.h"
//#else
        #include <QSystemTrayIcon>
//#endif

class QCloseEvent;
class QMutex;
class QMenu;
class aboutInfo;
class AbstractLayer;

class eventEater : public QObject
{
	Q_OBJECT

public:
	eventEater(QObject *parent = 0) : QObject(parent), fChanged(false) { }

	inline bool getChanged() const { return fChanged; }
	inline void setChanged(const bool changed) { fChanged = changed; }

protected:
	bool fChanged;
	bool eventFilter(QObject *obj, QEvent *event);

};


class qutIM : public QObject, public EventHandler
{
    Q_OBJECT

public:
	qutIM(QObjectList plugins, QObject *parent = 0);
	virtual ~qutIM();

	static qutIM *instance();
	bool isShouldRun() { return bShouldRun; }
	void reloadGeneralSettings();
	void updateTrayIcon(const QIcon &);
	void animateNewMessageInTray();
	void stopNewMessageAnimation();
	void showBallon(const QString &title, const QString &message, int time);
	void reloadStyleLanguage();
	void addActionToList(QAction *);
	void processEvent(Event &e);
	inline const QString &translationPath() const { return m_translation_path; }
private slots:
//	void trayActivated(QSystemTrayIcon::ActivationReason);

	void appQuit();
	void qutimSettingsMenu();
	void qutimPluginSettingsMenu();//!< Now it only shows plug-in settings window.
	void pluginSettingsDeleted(QObject *);
	void destroySettings();
	void switchUser();
	void openGuiSettings();
	void guiSettingsDeleted(QObject *);
	void checkEventChanging();
	void updateTrayToolTip();
	void on_infoButton_clicked();
	void infoWindowDestroyed(QObject *);
//	void on_showHideButton_clicked();
	void onSecondsIdle(int);
	void updateMessageIcon();
//	void on_showHideGroupsButton_clicked();
//	void on_soundOnOffButton_clicked();
//	void on_toolButton_clicked();
//protected:
//	virtual void resizeEvent ( QResizeEvent * event );
//	virtual void closeEvent(QCloseEvent *event);
//	virtual void focusOutEvent( QFocusEvent * event );
//	virtual void focusInEvent ( QFocusEvent * event );
//	void keyPressEvent ( QKeyEvent *event );
private:
	static qutIM		*fInstance;
	static QMutex		fInstanceGuard;

//    Ui::qutIMClass ui;
    QList<QAction *> m_plugin_actions;
    QAction *quitAction;
    QAction *settingsAction;
    QAction *m_pluginSettingsAction;//!< This action connected with qutimPluginSettingsMenu();
    QAction *switchUserAction;
    QAction *m_gui_settings_action;
    QMenu *trayMenu;
    QMenu *mainMenu;

	bool bShouldRun;
    bool createMenuAccounts;
	bool letMeQuit;
    IconManager& m_iconManager;//!< use it to get icons from file or program
    bool unreadMessages;
    bool autoHide;
    int hideSec;
    bool m_auto_away;
    bool msgIcon;
    quint32 m_auto_away_minutes;

	/*
	 * Event Id's
	 */
	quint16 m_event_qutim_settings;
	quint16 m_event_plugins_settings;
	quint16 m_event_gui_settings;
	quint16 m_event_switch_user;
	quint16 m_event_show_menu;
	quint16 m_event_about;

    TrayLayerInterface *trayIcon;
//#ifndef Q_OS_WIN32
//    QSystemTrayIcon *trayIcon;
//#else
//    ExSysTrayIcon *trayIcon;
//#endif

    QTimer *timer;
    eventEater *eventObject;
    aboutInfo *infoWindow;
	bool aboutWindowOpen;
	QList<QTranslator *> m_translators;
	QString m_translation_path;

    QIcon tempIcon;
	// Idle detector
	Idle fIdleDetector;
    void createTrayIcon();
    void createActions();
	void createMainMenu();
    void saveMainSettings();
    void loadMainSettings();
    void updateTrayStatus();
    void readMessages();
    void loadStyle();
	void loadTranslation( const QFileInfoList &files );
    void loadTranslation(const int);
    void readTranslation();

    void initIcons();//!< It loads all icons by icon manager.
    
	AbstractLayer &m_abstract_layer;
    QString m_profile_name;
    bool m_switch_user;
    PluginSettings *m_plugin_settings;
signals:
	void updateTranslation();

};

#endif // QUTIM_H
