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

#ifndef QUTIM_H_
#define QUTIM_H_
#include <QString>
#include <QDir>
#include "../../include/qutim/historymanager.h"
#include <qutim/json.h>

namespace HistoryManager {

class qutim : public HistoryImporter
{
public:
	enum Type
	{
		Xml  = 0x01,
		Bin  = 0x02,
		Json = 0x04
	};
	Q_DECLARE_FLAGS(Types, Type);
	qutim();
	virtual ~qutim();
	bool guessXml(const QString &path, QFileInfoList &files, int &num);
	bool guessBin(const QString &path, QFileInfoList &files, int &num);
	bool guessJson(const QString &path, QFileInfoList &files, int &num);
	void loadXml(const QFileInfoList &files);
	void loadBin(const QFileInfoList &files);
	static QString quote(const QString &str);
	static QString unquote(const QString &str);
	void loadJson(const QFileInfoList &files);
	virtual void loadMessages(const QString &path);
	virtual bool validate(const QString &path);
	virtual QString name();
	virtual QIcon icon();
private:
	int m_value;
};

class qutimExporter : public qutim, public HistoryExporter
{
public:
	virtual QString name();
	virtual void writeMessages(const QHash<QString, Protocol> &data);
};

}

#endif /*QUTIM_H_*/

