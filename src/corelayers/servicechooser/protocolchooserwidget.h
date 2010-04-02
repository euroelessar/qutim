/****************************************************************************
 *  protocolchooserwidget.h
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

#ifndef PROTOCOLCHOOSERWIDGET_H
#define PROTOCOLCHOOSERWIDGET_H
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

	class ProtocolChooserWidget : public SettingsWidget
	{
		Q_OBJECT
	public:
		ProtocolChooserWidget();
		virtual void loadImpl();
		virtual void saveImpl();
		virtual void cancelImpl();
		virtual ~ProtocolChooserWidget();
	private slots:
		void onItemChanged(QStandardItem*);
	private:
		QString html(const ExtensionInfo &info);
		void clear();
		Ui::ServiceChooser *ui;
		QStandardItemModel *m_model; 
		QHash<QString, ServiceItem *> m_protocol_items;
	};

}
#endif // PROTOCOLCHOOSERWIDGET_H
