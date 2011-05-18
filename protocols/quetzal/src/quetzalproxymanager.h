///****************************************************************************
// *
// *  This file is part of qutIM
// *
// *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
// *
// ***************************************************************************
// *                                                                         *
// *   This file is part of free software; you can redistribute it and/or    *
// *   modify it under the terms of the GNU General Public License as        *
// *   published by the Free Software Foundation; either version 2 of the    *
// *   License, or (at your option) any later version.                       *
// *                                                                         *
// ***************************************************************************
// ****************************************************************************/

//#ifndef QUETZALPROXYMANAGER_H
//#define QUETZALPROXYMANAGER_H

//#include <qutim/networkproxy.h>
//#include <qutim/objectgenerator.h>
//#include <purple.h>

//class QuetzalProtocol;
//class QuetzalProtocolGenerator;

//class QuetzalProxyInfo : public qutim_sdk_0_3::NetworkProxyInfo
//{
//public:
//	QuetzalProxyInfo(PurpleProxyType type);
//	~QuetzalProxyInfo();
//	virtual QString name();
//	PurpleProxyType type() { return m_type; }
//	virtual qutim_sdk_0_3::LocalizedString description();
//	virtual qutim_sdk_0_3::DataItem settings(const qutim_sdk_0_3::Config &config);
//	virtual void saveSettings(qutim_sdk_0_3::Config config, const qutim_sdk_0_3::DataItem &settings);
//private:
//	PurpleProxyType m_type;
//};

//class QuetzalProxyManager : public QObject, public qutim_sdk_0_3::NetworkProxyManager
//{
//	Q_OBJECT
//public:
//    QuetzalProxyManager(QuetzalProtocol *protocol);
//	virtual ~QuetzalProxyManager();
	
//	virtual QList<qutim_sdk_0_3::NetworkProxyInfo*> proxies();
//	virtual void setProxy(qutim_sdk_0_3::Account *account, qutim_sdk_0_3::NetworkProxyInfo *proxy,
//						  const qutim_sdk_0_3::DataItem &settings);
//};

//class QuetzalProxyManagerGenerator : public qutim_sdk_0_3::ObjectGenerator
//{
//public:
//	QuetzalProxyManagerGenerator(QuetzalProtocolGenerator *protocol);
//	virtual ~QuetzalProxyManagerGenerator();
//	virtual const QMetaObject *metaObject() const
//	{
//		return m_meta;
//	}
//	virtual const char *iid() const
//	{
//		return 0;
//	}
//	virtual bool hasInterface(const char *id) const
//	{
//		Q_UNUSED(id);
//		return false;
//	}
//protected:
//	virtual QObject *generateHelper() const;
//private:
//	QMetaObject *m_meta;
//	QuetzalProtocolGenerator *m_protocol;
//};

//#endif // QUETZALPROXYMANAGER_H
