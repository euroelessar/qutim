/*
    qutim

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
						  Dmitri Arekhta <DaemonES@gmail.com>
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

#include <QMutexLocker>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <QSettings>
#include <QMutex>
#include <QDebug>
#include <QMenu>

#include "aboutinfo.h"
//#include "qutimsettings.h"
#include "logindialog.h"

#include "qutim.h"

#include "abstractlayer.h"
//#include "abstractchatlayer.h"
#include "guisettingswindow.h"
#include "abstractcontextlayer.h"

#if defined(Q_OS_WIN32)
#include "windows.h"

#pragma comment(lib, "user32.lib")
#endif

#if defined(_MSC_VER)
#pragma warning (disable:4138)
#endif

bool eventEater::eventFilter(QObject *obj, QEvent *event)
{
	if ( event->type() == QEvent::MouseButtonDblClick ||
			event->type() == QEvent::MouseButtonPress ||
			event->type() == QEvent::MouseButtonRelease ||
			event->type() == QEvent::MouseMove ||
			event->type() == QEvent::MouseTrackingChange ||
			event->type() == QEvent::KeyPress ||
			event->type() == QEvent::KeyRelease ||
			event->type() == QEvent::KeyboardLayoutChange)
	{
		fChanged = true;
	}
	//qWarning() << event->type();
//	if(event->type() == QEvent::Resize || event->type() == QEvent::Move)
//	{
//		if(obj->objectName()=="qutIMClass")
//		{
//			AbstractContactList::instance().signalToDoScreenShot();
//		}
//        }
	return QObject::eventFilter(obj, event);
}


// Statics

qutIM				*qutIM::fInstance = NULL;
QMutex				qutIM::fInstanceGuard;

static QStringList getSystemLanguage()
{
	qDebug() << QLocale::languageToString(QLocale::system().language()) << QLocale::countryToString(QLocale::system().country());
	QString name = QLocale::system().name();
	if( name.indexOf('_') < 0 )
		return QStringList();
	QString lang = name.section( '_', 0, 0 );
	if( lang.trimmed().isEmpty() )
		return QStringList();
	qDebug() << (QStringList() << name << name.toLower() << lang);
	return QStringList() << name << name.toLower() << lang;
}

static QFileInfoList getTranslationFiles( const QStringList &langs, const QStringList &paths, QString &translation_path )
{
	QFileInfoList files;
	QStringList filter = QStringList() << "*.qm";
	foreach( const QString &possible, langs )
	{
		foreach( translation_path, paths )
		{
			QDir path = translation_path;
			files = QDir( path.filePath( "languages/" + possible ) ).entryInfoList( filter );
			if( !files.isEmpty() )
			{
				translation_path = translation_path + "/languages/" + possible;
				return files;
			}
		}
	}
	translation_path.clear();
	return files;
}


qutIM::qutIM(QObjectList plugins, QObject *parent) :
	QObject(parent),
	bShouldRun(true),
//	fWindowStyle(CLThemeBorder),
	m_iconManager (IconManager::instance()),
	m_abstract_layer(AbstractLayer::instance()),
//	m_contact_list(AbstractContactList::instance()),
        m_switch_user(false), unreadMessages(false)
{
//	qApp->installTranslator(&applicationTranslator);
	trayIcon = 0;
	trayMenu = 0;
	m_abstract_layer.setPointers(this);
	PluginSystem::instance().initPlugins(plugins);
	QStringList possible_langs = getSystemLanguage();
	if( !possible_langs.isEmpty() )
	{
		QStringList paths = PluginSystem::instance().getSharePaths();
		loadTranslation( getTranslationFiles( possible_langs, paths, m_translation_path ) );
	}
	if ( !m_abstract_layer.showLoginDialog() )
	{
		bShouldRun = false;
		QApplication::quit();
		return;
	}
	m_profile_name = m_abstract_layer.getCurrentProfile();
	reloadStyleLanguage();
	m_abstract_layer.loadCurrentProfile();
	qApp->setWindowIcon( Icon("qutim") );
	trayIcon = AbstractLayer::Tray();
	trayMenu = trayIcon->contextMenu();

//	m_abstract_layer->loadCurrentProfile();
	quitAction = NULL;
	autoHide = true;
	msgIcon = false;
	m_plugin_settings = 0;
//	timer = new QTimer(this);
//	connect(timer, SIGNAL(timeout()), this, SLOT(hide()));
//	ui.setupUi(this);
	createActions();
	createMainMenu();
//	ui.contactListView->setFocusProxy(this);
	eventObject = new eventEater(this);
	qApp->installEventFilter(eventObject);
	letMeQuit = false;
//	setAttribute(Qt::WA_AlwaysShowToolTips, true);
//	setFocus(Qt::ActiveWindowFocusReason);

//	if ( QSystemTrayIcon::isSystemTrayAvailable() )
//	{
//		createTrayIcon();
//		connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
//	             this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
//	}
//	else
//	{
//		trayIcon = NULL;
//		trayMenu = NULL;
//	}
//	if ( QSystemTrayIcon::isSystemTrayAvailable() )
//          trayIcon->show();
//        else
//          trayIcon = NULL;
	createTrayIcon();
	
	loadMainSettings();

	if (!bShouldRun)
		return;

	


//	if (trayIcon)
//		updateTrayStatus();


//	connect(timer, SIGNAL(timeout()), this, SLOT(hide()));
	connect(QApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(appQuit()));
	QTimer::singleShot(60000, this, SLOT(checkEventChanging()));
//	if (trayIcon)
//		updateTrayToolTip();
	aboutWindowOpen = false;
	connect(&fIdleDetector, SIGNAL(secondsIdle(int)), this, SLOT(onSecondsIdle(int)));

	// Activate idle detector
	fIdleDetector.start();

	//QMutexLocker locker(&fInstanceGuard);
	//fInstance = this;

	loadStyle();

	initIcons();
	
	m_abstract_layer.initializePointers(/*ui.contactListView, ui.horizontalLayout,*/0,0, trayMenu,
			settingsAction);
        AbstractContextLayer::instance().createActions();
	m_abstract_layer.addAccountMenusToTrayMenu(createMenuAccounts);
	m_abstract_layer.updateTrayIcon();

	m_event_qutim_settings = PluginSystem::instance().registerEventHandler("Core/OpenWidget/MainSettings", this);
	m_event_plugins_settings = PluginSystem::instance().registerEventHandler("Core/OpenWidget/PluginSettings", this);
	m_event_gui_settings = PluginSystem::instance().registerEventHandler("Core/OpenWidget/GuiSettings", this);
	m_event_switch_user = PluginSystem::instance().registerEventHandler("Core/OpenWidget/SwitchUser", this);
	m_event_about = PluginSystem::instance().registerEventHandler("Core/OpenWidget/About", this);
}

qutIM::~qutIM()
{
	// Stop idle detector
	fIdleDetector.stop();
}

qutIM *qutIM::instance()
{
	QMutexLocker locker(&fInstanceGuard);
	return fInstance;
}

void qutIM::createTrayIcon()
{
//	trayMenu = new QMenu(this);
    QList<QAction *> actions = trayMenu->actions();
    if(actions.size())
    {
        QAction *upper = actions[0];
        trayMenu->insertAction(upper, settingsAction);
        trayMenu->insertAction(upper, switchUserAction);
        trayMenu->insertSeparator(upper);
    }
    else
    {
        trayMenu->addAction(settingsAction);
        trayMenu->addAction(switchUserAction);
    }
//    trayIcon->setClickedAction(this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
//
//#if !defined(Q_OS_MAC)
//	// Mac OS has it's own quit trigger in Dock menu
//	trayMenu->addSeparator();
//	trayMenu->addAction(quitAction);
//#endif
//
//#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
//	trayIcon = new ExSysTrayIcon(this);
//#else
//	trayIcon = new QSystemTrayIcon(this);
//#endif
//
//	trayIcon->setIcon(QIcon(":/icons/qutim.png"));
//
//	// It's very unlikely for Mac OS X application to have
//	// tray menu and reactions to the tray action at the same time.
//	// So we decided to add tray menu to Dock
//#if !defined(Q_OS_MAC)
//	trayIcon->setContextMenu(trayMenu);
//#else
//	qt_mac_set_dock_menu(trayMenu);
//#endif
}

//void qutIM::resizeEvent(QResizeEvent * )
//{
//	if (fWindowStyle == CLBorderLessWindow)
//	{
//		QRegion visibleRegion(ui.contactListView->geometry().left(), ui.contactListView->geometry().top(),
//								ui.contactListView->geometry().width(), ui.contactListView->geometry().height());
//		setMask(visibleRegion);
//	}
//}
//
//void qutIM::closeEvent(QCloseEvent *event)
//{
//	hide();
//
//	// I dunno why, but it works ok in such way, but accordingly to QT Docs, QEvent::spontaneous() should return true if event generated by system...
//	if (event->spontaneous())
//		event->ignore();
//	else
//	{
//		appQuit();
//	}
//}

//void qutIM::trayActivated(QSystemTrayIcon::ActivationReason reason)
//{
//	switch(reason)
//	{
//	case QSystemTrayIcon::Trigger:
//		if (! unreadMessages )
//		{
//			if (this->isVisible())
//				this->setVisible(false);
//			else
//			{
//				this->setVisible(true);
//				this->activateWindow();
//			}
//		}
//		else
//		{
////			stopNewMessageAnimation();
//			AbstractChatLayer &acl = AbstractChatLayer::instance();
//			acl.readAllUnreadedMessages();
//		}
//		break;
//	default:
//		break;
//	}
//}

void qutIM::createActions()
{
	quitAction = new QAction(m_iconManager.getIcon("exit"),
			tr("&Quit"), this);
	settingsAction = new QAction(m_iconManager.getIcon("settings"),
			tr("&Settings..."), this);
	m_gui_settings_action = new QAction(m_iconManager.getIcon("gui"),
			tr("&User interface settings..."), this);
	m_pluginSettingsAction = new QAction(m_iconManager.getIcon("additional"),
			tr("Plug-in settings..."), this);
	switchUserAction = new QAction(m_iconManager.getIcon("switch_user"),
			tr("Switch profile"), this);
	connect(quitAction            , SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
	connect(settingsAction        , SIGNAL(triggered()), this, SLOT(qutimSettingsMenu()));
	connect(m_pluginSettingsAction, SIGNAL(triggered()), this, SLOT(qutimPluginSettingsMenu()));
	connect(switchUserAction      , SIGNAL(triggered()), this, SLOT(switchUser()));
	connect(m_gui_settings_action , SIGNAL(triggered()), this, SLOT(openGuiSettings()));
}

void qutIM::appQuit()
{
	saveMainSettings(); // save all main settings on exit
	PluginSystem::instance().aboutToQuit();
//	AbstractContactList::instance().saveSettings();
//	letMeQuit = true;
//	QCoreApplication::exit(0);
//	close();

}

void qutIM::saveMainSettings()
{
//	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
//
//	//window size and position saving
//	//save if "save size and position" feature is enabled
//	if ( settings.value("general/savesizpos", false).toBool())
//	{
//		QDesktopWidget desktop;
//		settings.beginGroup("mainwindow");
//		// Save current contact list' screen
//		settings.setValue("screenNum", desktop.screenNumber(this));
//		// Save size
//		settings.setValue("size", this->size());
//		// Save position on the screen
//		settings.setValue("position", this->pos());
//		settings.endGroup();
//	}
}

void qutIM::loadMainSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
//	QDesktopWidget desktop;
//
//	//load size and position if "save size and position" feature is enabled
//	//int numScreens = desktop.numScreens();
//	int mainWindowScreen = settings.value("mainwindow/screenNum", desktop.primaryScreen()).toInt();
//
//	// Check wheather target screen is connected
//	if (mainWindowScreen > desktop.numScreens())
//	{
//		mainWindowScreen = desktop.primaryScreen();
//		settings.setValue("mainwindow/screenNum", QVariant(mainWindowScreen));
//	}
//
//	int screenX = desktop.availableGeometry(mainWindowScreen).x();
//	int screenY = desktop.availableGeometry(mainWindowScreen).y();
//	int screenHeight = desktop.availableGeometry(mainWindowScreen).height();
//
//	/*for (int i = 0; i < desktop.numScreens(); i++)
//	{
//		QRect avGeom = desktop.availableGeometry(i);
//		QRect scGeom = desktop.screenGeometry(i);
//	}*/
//
//	this->move(screenX, screenY);
//	//this->mapTo(desktop.screen(mainWindowScreen), desktop.screen(mainWindowScreen)->pos());
//	//QPoint offs = this->mapFromGlobal(desktop.screen(mainWindowScreen)->pos());
//	QPoint offs = mapFromGlobal(pos());
//	QPoint moveTo(desktop.availableGeometry(mainWindowScreen).right() - 150, screenY);
//	this->resize(150, screenHeight);
//
//	if (settings.value("general/savesizpos", true).toBool())
//	{
//		QSize sze = settings.value("mainwindow/size", QSize(0,0)).toSize();
//		moveTo = settings.value("mainwindow/position", moveTo).toPoint();
//
//		if (sze.width() >= sizeHint().width() || sze.height() >= sizeHint().height())
//			this->resize(sze);
//	}
//
//	/*if (moveTo.x() + frameGeometry().width() > desktop.availableGeometry(mainWindowScreen).right())
//		moveTo.setX(desktop.availableGeometry(mainWindowScreen).right() - frameGeometry().width() + 1);
//	if (moveTo.x() < desktop.availableGeometry(mainWindowScreen).left())
//		moveTo.setX(desktop.availableGeometry(mainWindowScreen).left());
//	if (moveTo.y() + frameGeometry().height() > desktop.availableGeometry(mainWindowScreen).bottom())
//		moveTo.setY(desktop.availableGeometry(mainWindowScreen).bottom() - 2*frameGeometry().height()
//			+ height());
//	if (moveTo.y() < desktop.availableGeometry(mainWindowScreen).top())
//		moveTo.setY(desktop.availableGeometry(mainWindowScreen).top());*/
//
//	if (frameGeometry().height() > desktop.availableGeometry(mainWindowScreen).bottom())
//	{
//		resize(width(), desktop.availableGeometry(mainWindowScreen).bottom() + height() - frameGeometry().height() + 1);
//	}
//
////	moveTo += offs;
//	// Rellocating contact list' window
//	#if defined(Q_OS_WIN32)
//		this->move(moveTo + QPoint(-8,-52));
//	#else
//		this->move(moveTo);
//	#endif
	createMenuAccounts = settings.value("general/accountmenu", true).toBool();

	autoHide = settings.value("general/autohide",false).toBool();
	hideSec = settings.value("general/hidesecs",60).toInt();
//	Qt::WindowFlags flags = 0;
//
//	if ( settings.value("general/ontop", false).toBool())
//		flags |= Qt::WindowStaysOnTopHint;
//	else
//	    flags &= ~Qt::WindowStaysOnTopHint;
//	setWindowFlags(flags);
//
//	setVisible(!settings.value("general/hidestart", false).toBool());
	m_abstract_layer.setCurrentAccountIconName(settings.value("general/currentaccount", "").toString());

	m_auto_away = settings.value("general/autoaway", true).toBool();
	m_auto_away_minutes = settings.value("general/awaymin", 10).toUInt();
//
////	if(!settings.value("contactlist/showoffline",true).toBool())
////	{
////		ui.showHideButton->setIcon(m_iconManager.getIcon("hideoffline"));
////		ui.showHideButton->setChecked(true);
////	}
////	else
////	{
////		ui.showHideButton->setIcon(m_iconManager.getIcon("shhideoffline"));
////		ui.showHideButton->setChecked(true);
////	}
////	if(!(settings.value("contactlist/modeltype",0).toInt()&2))
////		ui.showHideGroupsButton->setChecked(true);
////	ui.soundOnOffButton->setChecked(!settings.value("enable",true).toBool());
}

void qutIM::qutimSettingsMenu()
{
	settingsAction->setDisabled(true);
	switchUserAction->setDisabled(true);
//	ui.showHideButton->setEnabled(false);
//	ui.showHideGroupsButton->setEnabled(false);
//	ui.soundOnOffButton->setEnabled(false);
	m_abstract_layer.openSettingsDialog();
}


void qutIM::qutimPluginSettingsMenu()
{
	m_plugin_settings = new PluginSettings;
	connect(m_plugin_settings, SIGNAL(destroyed ( QObject * )),
			this, SLOT(pluginSettingsDeleted(QObject *)));
	m_plugin_settings->show();
	m_pluginSettingsAction->setEnabled(false);
}



void qutIM::destroySettings()
{
	settingsAction->setDisabled(false);
	switchUserAction->setDisabled(false);
//	ui.showHideButton->setEnabled(true);
//	ui.showHideGroupsButton->setEnabled(true);
//	ui.soundOnOffButton->setEnabled(true);
}

void qutIM::reloadGeneralSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
//
//	autoHide = settings.value("contactlist/autohide",false).toBool();
//	hideSec = settings.value("contactlist/hidesecs",60).toUInt();
	bool cr_m_acc = settings.value("general/accountmenu", true).toBool();
	if ( cr_m_acc != createMenuAccounts )
	{
		createMenuAccounts = cr_m_acc;
		m_abstract_layer.addAccountMenusToTrayMenu(createMenuAccounts);
	}
//	if ( !autoHide )
//		timer->stop();
//	bool visible_now = isVisible();
//
//	Qt::WindowFlags flags = windowFlags();
//	if ( settings.value("general/ontop", false).toBool())
//		flags |= Qt::WindowStaysOnTopHint;
//	else
//	    flags &= ~Qt::WindowStaysOnTopHint;
//	setWindowFlags(flags);
//
//	setVisible(visible_now);
	m_abstract_layer.setCurrentAccountIconName(settings.value("general/currentaccount", "").toString());
	m_auto_away = settings.value("general/autoaway", true).toBool();
	m_auto_away_minutes = settings.value("general/awaymin", 10).toUInt();
}

void qutIM::switchUser()
{
	if( QMessageBox::question( 0, tr("Switch profile"), tr("Do you really want to switch profile?"),
							   QMessageBox::Ok | QMessageBox::Cancel,
							   QMessageBox::Cancel ) != QMessageBox::Ok )
		return;
	m_switch_user = true;
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");
	settings.setValue("general/switch", true);
	QProcess::startDetached(qApp->applicationFilePath());
	QCoreApplication::quit();
}

void qutIM::updateTrayStatus()
{

}

void qutIM::updateTrayIcon(const QIcon &statusIcon)
{
	if (trayIcon)
		trayIcon->setIcon(statusIcon);
	tempIcon = statusIcon;
}

void qutIM::readMessages()
{

	
}




//void qutIM::focusOutEvent ( QFocusEvent */*event*/ )
//{
//	if ( autoHide )
//	{
//		qDebug()<<"hide after"<<hideSec;
//		timer->start(hideSec * 1000);
//	}
//
//}
//
//void qutIM::focusInEvent(QFocusEvent */*event*/)
//{
//	timer->stop();
//
//}
//
//void qutIM::keyPressEvent( QKeyEvent *event )
//{
//    if (event->key() == Qt::Key_Escape)
//	hide();
//}


void qutIM::checkEventChanging()
{
	if ( eventObject->getChanged() )
	{
		eventObject->setChanged(false);
	}
	QTimer::singleShot(60000, this, SLOT(checkEventChanging()));
}

void qutIM::updateTrayToolTip()
{
//	if (!trayIcon) return;
//	QString toolTip;
//	foreach(icqAccount *account, icqList)
//	{
//		toolTip.append(tr("<img src='%1'>  %2<br>").arg(account->currentIconPath).arg(account->getIcquin()));
//	}
//	toolTip.chop(4);
//	trayIcon->setToolTip(toolTip);
}

void qutIM::createMainMenu()
{
	mainMenu = new QMenu();
	if ( m_plugin_actions.count() > 0)
	{
		foreach(QAction *action, m_plugin_actions)
		{
			mainMenu->addAction(action);
		}
		mainMenu->addSeparator();
	}
	mainMenu->addAction(settingsAction);
	mainMenu->addAction(m_pluginSettingsAction);
	mainMenu->addAction(m_gui_settings_action);
	mainMenu->addAction(switchUserAction);
	mainMenu->addSeparator();
	mainMenu->addAction(quitAction);
//	ui.menuButton->setMenu(mainMenu);
	AbstractLayer::ContactList()->setMainMenu(mainMenu);
}

void qutIM::loadStyle()
{
//	//Disable theme engine for Mac OS X
//#if !defined(Q_OS_MAC)
//	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "mainsettings");
//
//	currentStyle = settings.value("style/path", ":/qss/default.qss").toString();
//	QDir styleDir(currentStyle);
//	QStringList filters;
//	filters << "*.qss";
//	QStringList styleList = styleDir.entryList(filters);
//	if ( styleList.count())
//	{
//		QFile file(currentStyle + "/" +styleList.at(0));
//	    if(file.open(QFile::ReadOnly))
//	    {
//	    	qApp->setStyleSheet("");
//		    QString styleSheet = QLatin1String(file.readAll());
//		    qApp->setStyleSheet(styleSheet.replace("%path%", currentStyle));
//		    file.close();
//	    }
//	} else
//		qApp->setStyleSheet("");
//#endif
}

void qutIM::on_infoButton_clicked()
{
	if ( !aboutWindowOpen )
	{
		aboutWindowOpen = true;
		infoWindow = new aboutInfo;
		connect(infoWindow, SIGNAL(destroyed ( QObject * )),
				this, SLOT(infoWindowDestroyed(QObject *)));
		infoWindow->show();
	}
}

void qutIM::infoWindowDestroyed(QObject *)
{
	aboutWindowOpen = false;

}

void qutIM::loadTranslation(int index)
{
//	qApp->removeTranslator(&applicationTranslator);
//	switch(index)
//	{
////	case 0:
////		applicationTranslator.load("");
////		break;
//	case 1:
//		applicationTranslator.load(":/translation/qutim_lt.qm");
//		break;
//	case 2:
//		applicationTranslator.load(":/translation/qutim_ru.qm");
//		break;
//	case 3:
//		applicationTranslator.load(":/translation/qutim_ua.qm");
//		break;
//	default:
//		applicationTranslator.load("");
//	}
//	qApp->installTranslator(&applicationTranslator);
//
//	if (quitAction)
//	{
//		if (trayIcon) updateTrayToolTip();
//		quitAction->setText(tr("&Quit"));
//		settingsAction->setText(tr("&Settings..."));
//		switchUserAction->setText(tr("Switch user"));
//	}
//	emit updateTranslation();
}

void qutIM::readTranslation()
{
//	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "mainsettings");
//	currentTranslation = settings.value("style/tr", 0).toInt();
//	loadTranslation(currentTranslation);
}

//void qutIM::on_showHideButton_clicked()
//{
//	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
//	settings.beginGroup("contactlist");
//	bool show = settings.value("showoffline",true).toBool();
//	settings.setValue("showoffline",!show);
//	settings.endGroup();
//	AbstractContactList &acl = AbstractContactList::instance();
//	if(show)
//		ui.showHideButton->setIcon(m_iconManager.getIcon("hideoffline"));
//	else
//		ui.showHideButton->setIcon(m_iconManager.getIcon("shhideoffline"));
//	acl.loadSettings();
//}


void qutIM::initIcons()
{
//	ui.menuButton    ->setIcon(m_iconManager.getIcon("menu"         ));
//	ui.showHideGroupsButton    ->setIcon(m_iconManager.getIcon("folder"         ));
//	ui.soundOnOffButton    ->setIcon(m_iconManager.getIcon("player_volume"         ));
//	ui.showHideButton->setIcon(m_iconManager.getIcon("shhideoffline"));
//	ui.infoButton    ->setIcon(m_iconManager.getIcon("info"         ));
}

void qutIM::onSecondsIdle(int seconds)
{
	// Autoaway is disabled, do not touch anything
	if (!m_auto_away)
		return ;
	
	// if activity is detected
	if (seconds == 0)
	{
		m_abstract_layer.setStatusAfterAutoAway();
	}

	// If idle time is bigger than autoaway time
	if (seconds > static_cast<int>(m_auto_away_minutes) * 60)
	{
		m_abstract_layer.setAutoAway();
	}
}

void qutIM::animateNewMessageInTray()
{
	if (trayIcon)
	{
		if ( !unreadMessages )
		{
			unreadMessages = true;
			msgIcon = true;
			updateMessageIcon();
		}
	}
}

void qutIM::updateMessageIcon()
{
	// Check wheather we have unread messaged
	if ( unreadMessages )
	{
		// And tray icon is enabled
		if (trayIcon)
		{
			// We want to make some blink effect, so change one icon with other
			if ( msgIcon )
				trayIcon->setIcon(m_iconManager.getIcon("message"));
			else 
				trayIcon->setIcon(QIcon());
			//trayIcon->setIcon(tempIcon);
		}
		msgIcon = !msgIcon;
		QTimer::singleShot(500,this, SLOT(updateMessageIcon()));
	}
	else if (trayIcon)
		trayIcon->setIcon(tempIcon);
}

void qutIM::stopNewMessageAnimation()
{
	unreadMessages = false;
	updateMessageIcon();
}

void qutIM::openGuiSettings()
{
	GuiSetttingsWindow *gui_window = new GuiSetttingsWindow(m_profile_name);
	m_gui_settings_action->setEnabled(false);
	gui_window->show();
	connect(gui_window, SIGNAL(destroyed(QObject *)), this, 
			SLOT(guiSettingsDeleted(QObject *)));
}

void qutIM::guiSettingsDeleted(QObject *)
{
	m_gui_settings_action->setEnabled(true);
}

//void qutIM::on_showHideGroupsButton_clicked()
//{
//	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
//	settings.beginGroup("contactlist");
//	int type = settings.value("modeltype",0).toInt();
//	if((type&2)==2)
//		type-=2;
//	else
//		type+=2;
//	settings.setValue("modeltype",type);
//	settings.endGroup();
//	AbstractContactList &acl = AbstractContactList::instance();
//	acl.loadSettings();
//}

void qutIM::showBallon(const QString &title, const QString &message, int time)
{
	trayIcon->showMessage(title, 
                        message, time);
}

//void qutIM::on_soundOnOffButton_clicked()
//{
//	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
//	settings.beginGroup("sounds");
//	bool enable = settings.value("enable",true).toBool();
//	settings.setValue("enable",!enable);
//	settings.endGroup();
//	AbstractSoundLayer::instance().loadSettings();
//}

void qutIM::loadTranslation( const QFileInfoList &files )
{
	m_translators.clear();
	foreach(const QFileInfo &info, files)
	{
		qDebug() << info.absoluteFilePath();
		QTranslator *translator = new QTranslator(this);
		translator->load(info.absoluteFilePath());
		qApp->installTranslator(translator);
		m_translators.append(translator);
	}
}

void qutIM::reloadStyleLanguage()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	foreach(QTranslator *translator, m_translators)
	{
		qApp->removeTranslator(translator);
		delete translator;
	}
	QFileInfoList files;
	if( settings.contains( "gui/language" ) )
	{
		m_translation_path = settings.value("gui/language", "").toString();// + "/main.qm";
		files = QDir(m_translation_path).entryInfoList( QStringList() << "*.qm" );
	}
	else
	{
		QStringList possible_langs = getSystemLanguage();
		if( !possible_langs.isEmpty() )
		{
			QStringList paths = PluginSystem::instance().getSharePaths();
			files = getTranslationFiles( possible_langs, paths, m_translation_path );
		}
	}
	QFileInfo file( m_translation_path );
	QString name = (file.isDir() && !files.isEmpty()) ? file.completeBaseName() : "en";
	Q_REGISTER_EVENT(event_language,"Core/Language/Changed");
	Event( event_language, 1, &name ).send();
	qDebug() << name;
	loadTranslation( files );


#if defined(Q_OS_WIN32)
	QString current_style = settings.value("gui/style", ":/style/style/qutim.qss").toString();
#else
	QString current_style = settings.value("gui/style", "").toString();
#endif


	if ( !current_style.isEmpty() )
	{
		QString path_to_style = current_style.section("/", 0, -2);
		QFile file(current_style);
		if(file.open(QFile::ReadOnly))
		{
			qApp->setStyleSheet("");
			QString styleSheet = QLatin1String(file.readAll());
			qApp->setStyleSheet(styleSheet.replace("%path%", path_to_style));
			file.close();
		}
	}
	else
		qApp->setStyleSheet("");
}

void qutIM::pluginSettingsDeleted(QObject *)
{
	m_pluginSettingsAction->setEnabled(true);
}

void qutIM::addActionToList(QAction *action)
{
	m_plugin_actions.append(action);
}

void qutIM::processEvent(Event &e)
{
	if(e.id == m_event_qutim_settings)
		qutimSettingsMenu();
	else if(e.id == m_event_plugins_settings)
		qutimPluginSettingsMenu();
	else if(e.id == m_event_gui_settings)
		openGuiSettings();
	else if(e.id == m_event_switch_user)
		switchUser();
	else if(e.id == m_event_about)
		on_infoButton_clicked();
}
