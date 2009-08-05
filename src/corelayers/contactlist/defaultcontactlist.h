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

#ifndef DEFAULTCONTACTLIST_H
#define DEFAULTCONTACTLIST_H

#include <QtGui/QWidget>
#include <QTreeView>
#include <QAbstractItemModel>
#include <QDebug>
#include <QToolTip>
#include "contactlistproxymodel.h"
#include "treecontactlistmodel.h"
#include "contactlistitemdelegate.h"
#include "src/iconmanager.h"
#include "include/qutim/layerinterface.h"

class QtCustomWidget;

enum CLWindowStyle
{
	CLRegularWindow = 0,
	CLThemeBorder,
	CLBorderLessWindow,
	CLToolWindow
};

namespace Ui {
    class DefaultContactList;
}

using namespace qutim_sdk_0_2;

class DefaultContactListView : public QTreeView
{
public:
	DefaultContactListView(QWidget *parent);
protected:
	virtual void drawRow(QPainter *painter,
						 const QStyleOptionViewItem &options,
						 const QModelIndex &index) const;
	virtual void drawBranches(QPainter *painter,
							  const QRect &rect,
							  const QModelIndex &index) const;
};

class DefaultContactList : public QWidget, public ContactListLayerInterface, public EventHandler{
	Q_OBJECT
public:
	DefaultContactList(QWidget *parent = 0);
    virtual ~DefaultContactList();
	virtual void addItem(const TreeModelItem &item, const QString &name = "");
	virtual void removeItem(const TreeModelItem &item);
	virtual void moveItem(const TreeModelItem & old_item, const TreeModelItem & new_item);
	virtual void setItemName(const TreeModelItem &item, const QString & name);
	virtual void setItemIcon(const TreeModelItem &item, const QIcon &icon, int position);
	virtual void setItemStatus(const TreeModelItem &item, const QIcon &icon, const QString &id, int mass);
	virtual void setItemText(const TreeModelItem &item, const QVector<QVariant> &text);
	virtual void setItemVisibility(const TreeModelItem &item, int flags);
	virtual void setItemAttribute(const TreeModelItem &item, ItemAttribute type, bool on);
	virtual QList<TreeModelItem> getItemChildren(const TreeModelItem &item = TreeModelItem());
	virtual const ItemData *getItemData(const TreeModelItem &item);
	virtual QHBoxLayout *getAccountButtonsLayout();
	virtual void setMainMenu(QMenu *menu);

	virtual bool init(PluginSystemInterface *plugin_system);
	virtual void release() { saveSettings(); }
	virtual void setProfileName(const QString &profile_name);
	virtual void setLayerInterface( LayerType type, LayerInterface *interface);

	virtual void saveLayerSettings();
	virtual QList<SettingsStructure> getLayerSettingsList();
	virtual void removeLayerSettings();

	virtual void saveGuiSettingsPressed();
	virtual void removeGuiLayerSettings() {}

	virtual void processEvent(Event &e);

public slots:
	void on_showHideButton_clicked();
	void on_showHideGroupsButton_clicked();
	void on_soundOnOffButton_clicked();
	void on_infoButton_clicked();

protected slots:
	void loadSettings();
	void loadGuiSettings();

protected:
	void setItemHasUnviewedContent(const TreeModelItem &item, bool has_content) {}
	void setItemIsTyping(const TreeModelItem &item, bool is_typing) {}
	void saveSettings();
	void reloadCLWindowStyle(const QSettings &settings);
	bool eventFilter(QObject *obj, QEvent *event);
	void hideEvent( QHideEvent *event );
	void showEvent( QShowEvent *event );
    virtual void changeEvent(QEvent *e);
	TreeContactListModel *m_item_model;
	ContactListProxyModel *m_proxy_model;
	ContactListItemDelegate *m_item_delegate;
	QString m_profile_name;
	IconManager &m_icon_manager;
	CLWindowStyle fWindowStyle;
	ItemData m_current_item_data;
	QTimer m_autohide_timer;
	bool m_autohide;
	bool m_groupexpandoneclick;
	int m_autohide_sec;
	Qt::WindowFlags m_default_flags;
	quint16 m_show_hide_cl;
	quint16 m_event_about;
	quint16 m_event_typng;
	quint16 m_event_unviewed;
	quint16 m_event_set_sound_enable;
	quint16 m_event_sound_enabled;

private:
    PluginSystemInterface *m_ps;
    Ui::DefaultContactList *m_ui;
    ChatLayerInterface *m_chat_layer;
	QtCustomWidget *m_custom;
	QTreeView *m_contactListView;
};

#endif // DEFAULTCONTACTLIST_H
