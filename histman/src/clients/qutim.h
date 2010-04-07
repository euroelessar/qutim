/****************************************************************************
 *  qutim.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef QUTIM_H_
#define QUTIM_H_
#include <QString>
#include <QDir>
#include "include/qutim/historymanager.h"
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
