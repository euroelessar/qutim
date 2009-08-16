#include "modulemanagerimpl.h"
#include "libqutim/jsonfile.h"
#include <QVariant>
#include <QFile>
#include <QDebug>
#include <QScriptValue>
#include <QScriptEngine>

namespace Core
{
	ModuleManagerImpl::ModuleManagerImpl()
	{
//		QVariantMap map;
//		{
//			QVariantMap profiles;
//			profiles.insert("list", QVariantList()
//							<< "234" << "EuroElessar" << "EuroElessar2"
//							<< "EuroElessar3" << "EuroElessar4");
//			profiles.insert("current", 1);
//			map.insert("profiles", profiles);
//		}
//		{
//			QVariantMap general;
//			general.insert("libpaths", QVariantList() << "/home/elessar/qutim/plugins");
//			map.insert("general", general);
//		}
//		QVariant object = map;
//		JsonFile file("/home/elessar/.config/qutim/settings.json");
//		file.save(object);
//		file.load(object);
//		qDebug() << object.toMap()["profiles"].toMap()["list"].toStringList();
//		QScriptEngine engine;
//		QFile file("/home/elessar/.config/qutim/settings.json");
//		file.open(QIODevice::ReadOnly);
//		QScriptValue object = engine.evaluate(QString::fromUtf8(file.readAll()));
//		qDebug() << object.toString();
	}

	QList<ExtensionInfo> ModuleManagerImpl::coreExtensions() const
	{
		return extensions();
	}

	void ModuleManagerImpl::initExtensions()
	{
		ModuleManager::initExtensions();
	}
}
