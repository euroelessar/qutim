/*****************************************************************************
	DefaultContactList

	Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include <QContextMenuEvent>
#include <QHelpEvent>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QDesktopWidget>

#include "defaultcontactlist.h"
#include "solutions/qtcustomwidget.h"
#include "ui_defaultcontactlist.h"

//#include "src/abstractchatlayer.h"
#include "src/abstractcontextlayer.h"
#include "contactlistsettings.h"

DefaultContactListView::DefaultContactListView(QWidget *parent)
{
	setHeaderHidden(true);
	setIndentation(0);
	setExpandsOnDoubleClick(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setDragEnabled(true);
	setAcceptDrops(true);
	setDropIndicatorShown(true);
}

void DefaultContactListView::drawRow(QPainter *painter,
						 const QStyleOptionViewItem &options,
						 const QModelIndex &index) const
{
	QStyleOptionViewItem mod_options = options;
	mod_options.showDecorationSelected = false;
	QTreeView::drawRow(painter, mod_options, index);
}

void DefaultContactListView::drawBranches(QPainter *painter,
							  const QRect &rect,
							  const QModelIndex &index) const
{
}

DefaultContactList::DefaultContactList(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::DefaultContactList), m_icon_manager(IconManager::instance())
{
	m_groupexpandoneclick = false;
	setWindowFlags( 0 );
	m_default_flags = windowFlags();
	m_ui->setupUi(this);
	m_contactListView = new DefaultContactListView(this);
	m_ui->verticalLayout->insertWidget(1, m_contactListView);
	QStringList headers;
	headers<<"1";
	m_item_model = new TreeContactListModel (headers);
	m_proxy_model = new ContactListProxyModel();
	QObject::connect(m_item_model, SIGNAL(itemInserted(QModelIndex)), m_proxy_model, SLOT(insertItem(QModelIndex)));
	QObject::connect(m_item_model, SIGNAL(itemRemoved(QModelIndex)), m_proxy_model, SLOT(removeItem(QModelIndex)));
	QObject::connect(m_item_model, SIGNAL(checkItem(QModelIndex)), m_proxy_model, SLOT(checkItem(QModelIndex)));
	QObject::connect(m_item_model, SIGNAL(itemNameChanged(QModelIndex,QString)), m_proxy_model, SLOT(setName(QModelIndex,QString)));
	QObject::connect(m_item_model, SIGNAL(itemStatusChanged(QModelIndex,QIcon,QString,int)), m_proxy_model, SLOT(setStatus(QModelIndex,QIcon,QString,int)));
	m_contactListView->setModel(m_proxy_model);
	m_proxy_model->setModel(m_item_model);
	m_proxy_model->setTreeView(m_contactListView);
	m_item_delegate = new ContactListItemDelegate();
	m_contactListView->setItemDelegate(m_item_delegate);
	m_item_delegate->setTreeView(m_contactListView);
	QObject::connect(m_item_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), m_proxy_model, SLOT(newData(QModelIndex,QModelIndex)));
	m_contactListView->installEventFilter(this);
	m_contactListView->findChild<QObject *>("qt_scrollarea_viewport")->installEventFilter(this);
	setVisible(false);
	setAttribute(Qt::WA_AlwaysShowToolTips, true);
	m_autohide = false;
	m_autohide_timer.setSingleShot( true );
	connect( &m_autohide_timer, SIGNAL(timeout()), SLOT(hide()) );
	m_custom = new QtCustomWidget(this);
	setWindowRole("contactlist");
}

DefaultContactList::~DefaultContactList()
{
	delete m_ui;
}

void DefaultContactList::addItem(const TreeModelItem &item, const QString &name)
{
	m_contactListView->setUpdatesEnabled(false);
	QModelIndex parent_index = m_proxy_model->mapFromSource(m_item_model->findIndex(item)).parent();
	bool result = false;
	switch(item.m_item_type)
	{
	case 0: // buddy item
		result = m_item_model->addBuddy(item, name.isEmpty()?item.m_item_name:name);
		break;
	case 1: // group item
		result = m_item_model->addGroup(item, name.isEmpty()?item.m_item_name:name);
		break;
	case 2: // account item
		result = m_item_model->addAccount(item, name.isEmpty()?item.m_account_name:name);
		break;
	}
	if(parent_index.isValid() && result)
		if(m_contactListView->isExpanded(parent_index))
		{
			m_contactListView->setExpanded(parent_index,false);
			m_contactListView->setExpanded(parent_index,true);
		}
	m_contactListView->setUpdatesEnabled(true);
}

void DefaultContactList::removeItem(const TreeModelItem &item)
{
	bool result=false;
	QModelIndex parent_index = m_proxy_model->mapFromSource(m_item_model->findIndex(item)).parent();
	m_contactListView->setUpdatesEnabled(false);
	switch(item.m_item_type)
	{
	case 0: // buddy item
		result = m_item_model->removeBuddy(item);
		break;
	case 1: // group item
		result = m_item_model->removeGroup(item);
		break;
	case 2: // account item
		result = m_item_model->removeAccount(item);
		break;
	}
	if(parent_index.isValid() && result)
		if(m_contactListView->isExpanded(parent_index))
		{
			m_contactListView->setExpanded(parent_index,false);
			m_contactListView->setExpanded(parent_index,true);
		}
	m_contactListView->setUpdatesEnabled(true);
}

void DefaultContactList::moveItem(const TreeModelItem & old_item, const TreeModelItem & new_item)
{
	if(old_item.m_item_type!=0)
		return;
	TreeItem *tree_item = m_item_model->findItem(old_item);
	if(!tree_item)
		return;
	QVariant display = tree_item->data(Qt::DisplayRole);
	QList<QVariant> decoration = *reinterpret_cast<QList<QVariant> *>(tree_item->data(Qt::DecorationRole).value<qptrdiff>());
	int mass = tree_item->data(Qt::UserRole+1).toInt();
	QList<QVariant> rows = *reinterpret_cast<QList<QVariant> *>(tree_item->data(Qt::UserRole+2).value<qptrdiff>());
	QString status = tree_item->data(Qt::UserRole+3).toString();
	bool always_visible = tree_item->getAlwaysVisible();
	bool always_invisible = tree_item->getAlwaysInvisible();
	if(m_item_model->removeBuddy(old_item))
	{
		if(m_item_model->addBuddy(new_item,display.toString()))
		{
			m_contactListView->setUpdatesEnabled(false);
			tree_item = m_item_model->findItem(new_item);
			tree_item->setData(decoration,Qt::DecorationRole);
//			for (int i = 0; i < rows.size(); i++)
			setItemText(new_item,rows[0].toList().toVector());
//			setItemVisible(new_item,always_visible);
//			setItemInvisible(new_item,always_invisible);
			setItemStatus(new_item,qvariant_cast<QIcon>(decoration[0]),status,mass);
		}
	}
}

void DefaultContactList::setItemName(const TreeModelItem &item, const QString & name)
{
	m_contactListView->setUpdatesEnabled(false);
	m_item_model->setItemName(item, name);
	m_contactListView->setUpdatesEnabled(true);
}

void DefaultContactList::setItemIcon(const TreeModelItem &item, const QIcon &icon, int position)
{
	m_item_model->setItemIcon(item, icon, position);

	if(position==12)
		m_chat_layer->contactChangeHisClient(item);
}

void DefaultContactList::setItemStatus(const TreeModelItem &item, const QIcon &icon, const QString &id, int mass)
{
	if(item.m_item_type == 0)
	{

		m_chat_layer->contactChangeHisStatus(item,icon);
		m_contactListView->setUpdatesEnabled(false);
		m_item_model->setItemStatus(item, icon, id, mass);
		m_contactListView->setUpdatesEnabled(true);



	}
	else if(item.m_item_type == 2)
	{
		m_item_model->setItemIsOnline(item, mass < 1000);
	}
}

void DefaultContactList::setItemText(const TreeModelItem &item, const QVector<QVariant> &text)
{
	m_contactListView->setUpdatesEnabled(false);
	m_item_model->setItemRow(item, text.toList(), 0);
	m_contactListView->setUpdatesEnabled(true);
}

void DefaultContactList::setItemVisibility(const TreeModelItem &item, int flags)
{
	m_item_model->setItemVisibility(item, flags);
}

void DefaultContactList::setItemAttribute(const TreeModelItem &item, ItemAttribute type, bool on)
{
	Event e(-1, 2, &item, &on);
	switch(type)
	{
	case ItemHasUnreaded:
		e.id = m_event_unviewed;
		m_item_model->setItemHasUnviewedContent(item, on);
		break;
	case ItemIsTyping:
		e.id = m_event_typng;
		m_item_model->setItemIsTyping(item, on);
		break;
	default:
		return;
	}
	PluginSystem::instance().sendEvent(e);
}

QList<TreeModelItem> DefaultContactList::getItemChildren(const TreeModelItem &item)
{
	if(m_item_model)
		return m_item_model->getItemChildren(item);
	else
		return QList<TreeModelItem>();
}

const ItemData *DefaultContactList::getItemData(const TreeModelItem &item)
{
	Q_UNUSED(item);	
	TreeItem *tree_item = m_item_model->findItem(item);
	if(tree_item)
	{
		tree_item->getData(m_current_item_data);
		m_current_item_data.attributes = (m_item_model->getItemHasUnviewedContent(tree_item)?ItemHasUnreaded:0)
										 | (m_item_model->getItemIsTyping(tree_item)?ItemIsTyping:0);
		return &m_current_item_data;
	}
	return 0;
}

QHBoxLayout *DefaultContactList::getAccountButtonsLayout()
{
	return m_ui->horizontalLayout;
}

void DefaultContactList::setMainMenu(QMenu *menu)
{
	m_ui->menuButton->setMenu(menu);
}

bool DefaultContactList::init(PluginSystemInterface *plugin_system)
{
	m_event_about = PluginSystem::instance().registerEventHandler("Core/OpenWidget/About");
	m_show_hide_cl = PluginSystem::instance().registerEventHandler("Core/ContactList/ShowHide", this);
	m_event_typng = PluginSystem::instance().registerEventHandler("Core/ContactList/ItemIsTyping");
	m_event_unviewed = PluginSystem::instance().registerEventHandler("Core/ContactList/ItemHasUnreaded");
	m_event_set_sound_enable = plugin_system->registerEventHandler("Core/Notification/SetSoundIsEnabled");
	m_event_sound_enabled = plugin_system->registerEventHandler("Core/Notification/SoundIsEnabled", this);
	m_ps = plugin_system;
	return true;
}

void DefaultContactList::setProfileName(const QString &profile_name)
{
	m_profile_name = profile_name;
	setWindowIcon(m_icon_manager.getIcon("qutim"));
	setWindowTitle("qutIM");
	m_ui->menuButton->setIcon(m_icon_manager.getIcon("menu"));
	m_ui->showHideGroupsButton->setIcon(m_icon_manager.getIcon("folder"));
	m_ui->soundOnOffButton->setIcon(m_icon_manager.getIcon("player_volume"));
	m_ui->showHideButton->setIcon(m_icon_manager.getIcon("shhideoffline"));
	m_ui->infoButton->setIcon(m_icon_manager.getIcon("info"));
	m_item_model->loadSettings(m_profile_name);
	m_proxy_model->loadProfile(m_profile_name);
//	AbstractThemeEngine::instance().loadProfile(profile_name);


	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	QDesktopWidget &desktop = *QApplication::desktop();

	int main_window_screen = settings.value("mainwindow/screenNum", desktop.primaryScreen()).toInt();

	// Check wheather target screen is connected
	if (main_window_screen > desktop.numScreens())
	{
		main_window_screen = desktop.primaryScreen();
		settings.setValue("mainwindow/screenNum", QVariant(main_window_screen));
	}

	int screenX = desktop.availableGeometry(main_window_screen).x();
	int screenY = desktop.availableGeometry(main_window_screen).y();
	int screenHeight = desktop.availableGeometry(main_window_screen).height();

	this->move(screenX, screenY);
	QPoint offs = mapFromGlobal(pos());
	QPoint moveTo(desktop.availableGeometry(main_window_screen).right() - 150, screenY);
	this->resize(150, screenHeight);

	if (settings.value("general/savesizpos", true).toBool())
	{
		QSize sze = settings.value("mainwindow/size", QSize(0,0)).toSize();
		moveTo = settings.value("mainwindow/position", moveTo).toPoint();

		if (sze.width() >= sizeHint().width() || sze.height() >= sizeHint().height())
			this->resize(sze);
	}

//	if (frameGeometry().height() > desktop.availableGeometry(main_window_screen).bottom())
//	{
//		resize(width(), desktop.availableGeometry(main_window_screen).bottom() + height() - frameGeometry().height() + 1);
//	}

	// Rellocating contact list' window
//	#if defined(Q_OS_WIN32)
//		this->move(moveTo + QPoint(-8,-52));
//	#else
//		this->move(moveTo);
//	#endif
//	createMenuAccounts = settings.value("general/accountmenu", true).toBool();
//
//	Qt::WindowFlags flags = 0;
//
//	if ( settings.value("general/ontop", false).toBool())
//		flags |= Qt::WindowStaysOnTopHint;
//	else
//		flags &= ~Qt::WindowStaysOnTopHint;
//
//	setWindowFlags(flags);
//	m_abstract_layer.setCurrentAccountIconName(settings.value("general/currentaccount", "").toString());

//	m_auto_away = settings.value("general/autoaway", true).toBool();
//	m_auto_away_minutes = settings.value("general/awaymin", 10).toUInt();

	loadGuiSettings();
	loadSettings();

	this->move(moveTo);

	if( !settings.value("general/hidestart", false).toBool())
	{
		show();
		setFocus(Qt::ActiveWindowFocusReason);
	}
}

void DefaultContactList::setLayerInterface( LayerType type, LayerInterface *layer_interface)
{

    switch(type)
    {
	case ChatLayer:m_chat_layer = reinterpret_cast<ChatLayerInterface *>(layer_interface);
	default:
	    Q_UNUSED(type);
	    Q_UNUSED(layer_interface);
    }


}

void DefaultContactList::saveLayerSettings()
{
	foreach(const SettingsStructure &settings, m_settings)
	{
		ContactListSettings *cls = dynamic_cast<ContactListSettings *>(settings.settings_widget);
		if(cls)
			cls->saveSettings();
	}
	loadGuiSettings();
	loadSettings();
}

void DefaultContactList::saveGuiSettingsPressed()
{
	QTimer::singleShot( 0, this, SLOT(loadGuiSettings()) );
}

QList<SettingsStructure> DefaultContactList::getLayerSettingsList()
{
	m_ui->showHideButton->setEnabled(false);
	m_ui->showHideGroupsButton->setEnabled(false);
	m_ui->soundOnOffButton->setEnabled(false);
	SettingsStructure settings;
	settings.settings_item = new QTreeWidgetItem();
	settings.settings_item ->setText(0, QObject::tr("Contact List"));
	settings.settings_item ->setIcon(0, IconManager::instance().getIcon("contactlist"));
	settings.settings_widget = new ContactListSettings(m_profile_name);
	m_settings.append(settings);
	return m_settings;
}

void DefaultContactList::removeLayerSettings()
{
	foreach(const SettingsStructure &settings, m_settings)
	{
		delete settings.settings_item;
		delete settings.settings_widget;
	}
	m_settings.clear();
	m_ui->showHideButton->setEnabled(true);
	m_ui->showHideGroupsButton->setEnabled(true);
	m_ui->soundOnOffButton->setEnabled(true);
}

void DefaultContactList::loadGuiSettings()
{
	m_contactListView->setStyleSheet("");
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	m_contactListView->setUpdatesEnabled(false);
	bool show_window = !this->isHidden();
	reloadCLWindowStyle(settings);
	settings.beginGroup("contactlist");
	double opacity = settings.value("opacity",1).toDouble();
	int window_style = settings.value("windowstyle",0).toInt();
	settings.endGroup();
	settings.beginGroup("gui");
	m_contactListView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	bool style_result = m_item_delegate->setThemeStyle(settings.value("listtheme",":/style/cl/default.ListQutim").toString());
	if(settings.value("listtheme", ":/style/cl/default.ListQutim").toString().endsWith(".ListTheme") && style_result && window_style==2)
	{
		m_contactListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else
	{
		this->setWindowOpacity(opacity);
		opacity=1;
		if(show_window)
			this->show();
	}
	settings.endGroup();
	m_contactListView->setUpdatesEnabled(true);
}

void DefaultContactList::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	m_autohide = settings.value( "general/autohide",false ).toBool();
	m_autohide_sec = settings.value( "general/hidesecs",60 ).toInt();
	m_autohide_timer.setInterval( m_autohide_sec * 1000 );
	if( m_autohide && isVisible() )
		m_autohide_timer.start();
	m_contactListView->setUpdatesEnabled(false);
	settings.beginGroup("contactlist");
	m_groupexpandoneclick = settings.value("groupexpandoneclick", false).toBool();
	double opacity = settings.value("opacity",1).toDouble();
	m_contactListView->setAlternatingRowColors(settings.value("alternatingrc",false).toBool());
	QList<bool> show_icons;
	show_icons.append(settings.value("showicon0",true).toBool());
	show_icons.append(settings.value("showicon1",false).toBool());
	for(int i=2;i<12;i++)
		show_icons.append(settings.value("showicon"+QString::number(i),true).toBool());
	show_icons.append(settings.value("showicon12",false).toBool());
	m_item_delegate->setSettings(show_icons, opacity);
	int model_type = settings.value("modeltype",0).toInt();
	bool show_offline=settings.value("showoffline",false).toBool();
	bool show_empty_group=settings.value("showempty",false).toBool();
	bool sort_status=settings.value("sortstatus",false).toBool();
	bool show_separator=settings.value("showseparator",false).toBool();
	QString family=settings.value("accountfontfml",QFont().family()).toString();
	int size=settings.value("accountfontpnt",QFont().pointSize()).toInt();
	size=qBound(6, size, 24);
	QVariant account_font=settings.value("useaccountfont",false).toBool()?QFont(family,size):QVariant();
	family=settings.value("groupfontfml",QFont().family()).toString();
	size=settings.value("groupfontpnt",QFont().pointSize()).toInt();
	size=qBound(6, size, 24);
	QVariant group_font=settings.value("usegroupfont",false).toBool()?QFont(family,size):QVariant();
	family=settings.value("onlinefontfml",QFont().family()).toString();
	size=settings.value("onlinefontpnt",QFont().pointSize()).toInt();
	size=qBound(6, size, 24);
	QVariant online_font=settings.value("useonlinefont",false).toBool()?QFont(family,size):QVariant();
	family=settings.value("offlinefontfml",QFont().family()).toString();
	size=settings.value("offlinefontpnt",QFont().pointSize()).toInt();
	size=qBound(6, size, 24);
	QVariant offline_font=settings.value("useofflinefont",false).toBool()?QFont(family,size):QVariant();
	family=settings.value("separatorfontfml",QFont().family()).toString();
	size=settings.value("separatorfontpnt",QFont().pointSize()).toInt();
	size=qBound(6, size, 24);
	QVariant separator_font=settings.value("useseparatorfont",false).toBool()?QFont(family,size):QVariant();
	QVariant account_color=settings.value("useaccountfont",false).toBool()?settings.value("accountcolor",QVariant()):QVariant();
	QVariant group_color=settings.value("usegroupfont",false).toBool()?settings.value("groupcolor",QVariant()):QVariant();
	QVariant online_color=settings.value("useonlinefont",false).toBool()?settings.value("onlinecolor",QVariant()):QVariant();
	QVariant offline_color=settings.value("useofflinefont",false).toBool()?settings.value("offlinecolor",QVariant()):QVariant();
	QVariant separator_color=settings.value("useseparatorfont",false).toBool()?settings.value("separatorcolor",QVariant()):QVariant();
	m_proxy_model->setSettings(model_type, show_offline, show_empty_group, sort_status, show_separator, account_font, group_font, online_font, offline_font, separator_font, account_color, group_color, online_color, offline_color, separator_color);
	settings.endGroup();
	m_contactListView->setUpdatesEnabled(true);
	m_ui->showHideButton->setChecked( show_offline );
	m_ui->showHideGroupsButton->setChecked( model_type & 2 );
}

void DefaultContactList::saveSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");

	//window size and position saving
	//save if "save size and position" feature is enabled
	if ( settings.value("general/savesizpos", false).toBool())
	{
		QDesktopWidget &desktop = *QApplication::desktop();
		settings.beginGroup("mainwindow");
		// Save current contact list' screen
		settings.setValue("screenNum", desktop.screenNumber(this));
		// Save size
		settings.setValue("size", this->size());
		// Save position on the screen
		settings.setValue("position", this->pos());
		settings.endGroup();
	}
	if( m_item_delegate )
		delete m_item_delegate;
	if( m_proxy_model )
	{
		m_proxy_model->saveSettings();
		delete m_proxy_model;
	}
	if( m_item_model )
	{
		m_item_model->saveSettings();
		delete m_item_model;
	}
	m_item_model = 0;
	m_proxy_model = 0;
	m_item_delegate = 0;
}

bool DefaultContactList::eventFilter(QObject *obj, QEvent *event)
{
	switch( event->type() )
	{
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove:
	case QEvent::MouseTrackingChange:
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	case QEvent::KeyboardLayoutChange:
	case QEvent::HoverMove:
	case QEvent::Wheel:
		if( m_autohide_timer.isActive() )
			m_autohide_timer.start();
	default:
		break;
	}
	if(event->type() == QEvent::ContextMenu)
	{
		QContextMenuEvent *menu_event = static_cast<QContextMenuEvent *>(event);
		QTreeView *tree_view = dynamic_cast<QTreeView *>(obj->parent());
		if(!tree_view)
			return QObject::eventFilter(obj, event);
		QModelIndex index = tree_view->indexAt(menu_event->pos());
		if(index.isValid())
		{
			QModelIndex source_index = static_cast<ProxyModelItem*>(index.internalPointer())->getSourceIndex();
			if(!source_index.isValid())
				return QObject::eventFilter(obj, event);
			TreeModelItem item = static_cast<TreeItem*>(source_index.internalPointer())->getStructure();
			AbstractContextLayer::instance().itemContextMenu(item, menu_event->globalPos());
		}
	}
	if(m_groupexpandoneclick && event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);
		QTreeView *tree_view = dynamic_cast<QTreeView *>(obj->parent());
		if(!tree_view)
			return QObject::eventFilter(obj, event);
		QModelIndex index = tree_view->indexAt(mouse_event->pos());
		if(index.isValid())
		{
			QModelIndex source_index = static_cast<ProxyModelItem*>(index.internalPointer())->getSourceIndex();
			if(!source_index.isValid())
				return QObject::eventFilter(obj, event);
			TreeModelItem item = static_cast<TreeItem*>(source_index.internalPointer())->getStructure();
			if(item.m_item_type == 1)
				tree_view->setExpanded(index, !tree_view->isExpanded(index));
		}
	}
	if(event->type() == QEvent::MouseButtonDblClick)
	{
		QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);
		QTreeView *tree_view = dynamic_cast<QTreeView *>(obj->parent());
		if(!tree_view)
			return QObject::eventFilter(obj, event);
		QModelIndex index = tree_view->indexAt(mouse_event->pos());
		if(index.isValid())
		{
			QModelIndex source_index = static_cast<ProxyModelItem*>(index.internalPointer())->getSourceIndex();
			if(!source_index.isValid())
				return QObject::eventFilter(obj, event);
			TreeModelItem item = static_cast<TreeItem*>(source_index.internalPointer())->getStructure();
			if(item.m_item_type==0)
			{
				if ( m_chat_layer ) m_chat_layer->createChat(item);
			    /*AbstractChatLayer &acl = AbstractChatLayer::instance();
				acl.createChat(item);
				setItemHasUnviewedContent(item, false);*/
			}
		}
		event->accept();
	}
	if(event->type() == QEvent::KeyPress)
	{
		QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
		if(!key_event->isAutoRepeat())
		{
			if(key_event->key() == Qt::Key_F2)
			{
				QModelIndexList list = m_contactListView->selectionModel()->selectedIndexes();
				if( list.size() )
					m_contactListView->edit( list.first() );
			}
			else if(key_event->key() == Qt::Key_Delete)
			{
				QModelIndexList list = m_contactListView->selectionModel()->selectedIndexes();
				foreach(QModelIndex index, list)
				{
					QModelIndex source_index = static_cast<ProxyModelItem*>(index.internalPointer())->getSourceIndex();
					if(!source_index.isValid())
						return QObject::eventFilter(obj, event);
					const TreeModelItem &item = static_cast<TreeItem*>(source_index.internalPointer())->getStructure();
					if(item.m_item_type==0||item.m_item_type==1)
					{
						PluginSystem::instance().deleteItemSignalFromCL(item);
					}
				}
			}
			else if(key_event->key() == Qt::Key_Enter || key_event->key() == Qt::Key_Return)
			{
				QTreeView *tree_view = dynamic_cast<QTreeView *>(obj);
				if(!tree_view)
					return QObject::eventFilter(obj, event);
				QModelIndexList list = tree_view->selectionModel()->selectedIndexes();
				foreach(QModelIndex index, list)
				{
					QModelIndex source_index = static_cast<ProxyModelItem*>(index.internalPointer())->getSourceIndex();
					if(!source_index.isValid())
						return QObject::eventFilter(obj, event);
					TreeModelItem item = static_cast<TreeItem*>(source_index.internalPointer())->getStructure();
					if(item.m_item_type==0)
					{
						m_chat_layer->createChat(item);
						setItemHasUnviewedContent(item, false);
					}
				}
			}
			else
				return QObject::eventFilter(obj, event);
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}

void DefaultContactList::hideEvent( QHideEvent * )
{
	if( m_autohide_timer.isActive() )
		m_autohide_timer.stop();
}

void DefaultContactList::showEvent( QShowEvent * )
{
	if( m_autohide )
		m_autohide_timer.start();
}

void DefaultContactList::reloadCLWindowStyle(const QSettings &settings)
{
	Qt::WindowFlags flags = m_default_flags;
	flags &= ~Qt::Tool;
	flags |= Qt::Window;

	if ( settings.value("general/ontop", false).toBool())
		flags |= Qt::WindowStaysOnTopHint;
	else
		flags &= ~Qt::WindowStaysOnTopHint;

#if defined(Q_OS_WIN32)
        fWindowStyle = static_cast<CLWindowStyle>(settings.value("contactlist/windowstyle", CLThemeBorder).toInt());
#else
        fWindowStyle = static_cast<CLWindowStyle>(settings.value("contactlist/windowstyle", CLRegularWindow).toInt());
#endif


#if defined(Q_WS_MAC)
	if(fWindowStyle==CLThemeBorder)
		fWindowStyle = CLRegularWindow;
#endif

	switch( fWindowStyle )
	{
	case CLToolWindow:
		flags |= Qt::Tool;
	default:
	case CLRegularWindow:
		m_custom->stop();
//		AbstractThemeEngine::instance().stopSkiningCl();
		clearMask();
		setContentsMargins( 0, 0, 0, 0 );
		break;
	case CLThemeBorder:
		if(flags & Qt::WindowStaysOnTopHint)
			flags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;
		else
			flags = Qt::FramelessWindowHint;
		m_custom->start(settings.value("gui/borders", ":/style/border").toString());
//		AbstractThemeEngine::instance().reloadContent();
//		AbstractThemeEngine::instance().setCLBorder( this );
		break;
	case CLBorderLessWindow:
		m_custom->stop();
//		AbstractThemeEngine::instance().stopSkiningCl();
		// Strange behaviour happens on Mac OS X, so we will disable this flag for a while
#if !defined(Q_OS_MAC)
		flags |= Qt::FramelessWindowHint;
#endif
		setMask( QRegion( m_contactListView->geometry().left(), m_contactListView->geometry().top(),
								m_contactListView->geometry().width(), m_contactListView->geometry().height() ) );
		break;
	}

	if( flags != windowFlags() )
		setWindowFlags( flags );
}

void DefaultContactList::processEvent(Event &e)
{
	if(e.id == m_show_hide_cl)
	{
		if( e.size() && e.at<bool>(0) == ( isVisible() && !isMinimized() ) )
			return;
        if( isVisible() && !isMinimized() )
            QTimer::singleShot( 0, this, SLOT(hide()) );
		else
		{
			show();
			setWindowState(windowState() & ~Qt::WindowMinimized);
			activateWindow();
			raise();
		}
	}
	else if(e.id == m_event_sound_enabled)
	{
        m_ui->soundOnOffButton->setChecked( e.at<bool>(0) );
	}
}

void DefaultContactList::on_showHideButton_clicked()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("contactlist");
	bool show = settings.value("showoffline",true).toBool();
	settings.setValue("showoffline",!show);
	settings.endGroup();
	QTimer::singleShot( 0, this, SLOT(loadSettings()) );
}

void DefaultContactList::on_showHideGroupsButton_clicked()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("contactlist");
	int type = settings.value("modeltype",0).toInt();
	if( type&2 )
		type-=2;
	else
		type+=2;
	settings.setValue("modeltype",type);
	settings.endGroup();
	QTimer::singleShot( 0, this, SLOT(loadSettings()) );
}

void DefaultContactList::on_infoButton_clicked()
{
	Event e(m_event_about);
	PluginSystem::instance().sendEvent(e);
}

void DefaultContactList::on_soundOnOffButton_clicked()
{
	bool sound_on = m_ui->soundOnOffButton->isChecked();
	Event ev(m_event_set_sound_enable, 1, &sound_on);
	PluginSystem::instance().sendEvent(ev);
}

void DefaultContactList::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
