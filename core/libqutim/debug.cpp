/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "debug.h"
#include "config.h"
#include "objectgenerator.h"
#include "metaobjectbuilder.h"
#include <QTime>

namespace qutim_sdk_0_3
{
class NoDebugStream: public QIODevice
{
public:
	NoDebugStream() { open(WriteOnly); }
	bool isSequential() const { return true; }
	qint64 readData(char *, qint64) { return 0; /* eof */ }
	qint64 readLineData(char *, qint64) { return 0; /* eof */ }
	qint64 writeData(const char *, qint64 len) { return len; }
};
Q_GLOBAL_STATIC(NoDebugStream, devnull)

struct DebugData
{
	enum Level {
		Default = -1,
		Info = DebugInfo,
		Verbose = DebugVerbose,
		VeryVerbose = DebugVeryVerbose
	};

	inline DebugData() : meta(0), level(Default) {}
	Level fixedLevel() const;

	const QMetaObject *meta;
	QByteArray name;
	Level level;
};

typedef QMap<quint64, DebugData*> DebugMap;
Q_GLOBAL_STATIC(DebugMap, debugAreaMap)

struct GlobalDebugData {
	GlobalDebugData() : level(DebugData::Default) {}

	operator DebugData::Level()
	{
		if (level == DebugData::Default) {
			QByteArray environment = qgetenv("QUTIM_DEBUG");
			bool ok = true;
			level = static_cast<DebugData::Level>(environment.toInt(&ok));
			if (!ok)
				level = DebugData::VeryVerbose;
		}
		return level;
	}

	DebugData::Level &operator =(DebugData::Level l)
	{
		return (level = l);
	}

	bool operator ==(DebugData::Level l)
	{
		return level == l;
	}

	DebugData::Level level;
} debugLevel;

inline DebugData::Level DebugData::fixedLevel() const
{
	return debugLevel == Default ? VeryVerbose : debugLevel;
}

inline void init_core_data(DebugData *data)
{
	data->name = "[Core]:";
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(DebugData, coreData, init_core_data(x.data()))

QDebug debug_helper(quint64 debugId, DebugLevel level, QtMsgType type)
{
	const DebugData * const data = debugAreaMap()->value(debugId, coreData());
	if (data->fixedLevel() <= level) {
		return (QDebug(type)
				<< qPrintable(QTime::currentTime().toString(QLatin1String("[hh:mm:ss]")))
				<< data->name.constData());
	} else {
		return QDebug(devnull());
	}
}

void debugAddPluginId(quint64 debugId, const QMetaObject *meta)
{
	DebugData * &data = (*debugAreaMap())[debugId];
	if (!data) {
		data = new DebugData();
		data->meta = meta;
		data->name = "[" + QByteArray(meta->className()) + "]:";
	}
}

void debugClearConfig()
{
	DebugMap::iterator it = debugAreaMap()->begin();
	DebugMap::iterator end = debugAreaMap()->end();
	QString levelStr = QLatin1String("level");
	Config config;
	config.beginGroup(QLatin1String("debug"));
	debugLevel = DebugData::Default;
	debugLevel = config.value<DebugData::Level>(levelStr, debugLevel);
	for (; it != end; it++) {
		DebugData *data = it.value();
		config.beginGroup(QLatin1String(data->meta->className()));
		const char *name = MetaObjectBuilder::info(data->meta, "DebugName");
		if (!name)
			name = data->meta->className();
		data->name = "[" + QByteArray(name) + "]:";
		data->level = config.value(levelStr, DebugData::Default);
		config.endGroup();
	}
}

}

