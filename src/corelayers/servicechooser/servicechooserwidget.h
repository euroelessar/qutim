/****************************************************************************
 *  servicechooserwidget.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef SERVICECHOOSERWIDGET_H
#define SERVICECHOOSERWIDGET_H
#include <libqutim/settingswidget.h>
#include <QHash>

namespace qutim_sdk_0_3 
{
	class ExtensionInfo;
}

namespace Ui
{
	class ServiceChooser;
}

class QStandardItem;
class QStandardItemModel;
namespace Core
{

	class ServiceItem;
	using namespace qutim_sdk_0_3;

	class ServiceChooserWidget : public SettingsWidget
	{
		Q_OBJECT
	public:
		ServiceChooserWidget();
		virtual void loadImpl();
		virtual void saveImpl();
		virtual void cancelImpl();
		virtual ~ServiceChooserWidget();
	private slots:
		void onItemChanged(QStandardItem*);
	private:
		void clear();
		Ui::ServiceChooser *ui;
		QStandardItemModel *m_model; 
		QHash<QByteArray, ServiceItem *> m_service_items;
	};

}
#endif // SERVICECHOOSERWIDGET_H
