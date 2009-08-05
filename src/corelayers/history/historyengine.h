/*
    History Engine

    Copyright (c) 2008 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef HISTORYENGINE_H_
#define HISTORYENGINE_H_
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <qutim/plugininterface.h>

using namespace qutim_sdk_0_2;

class HistoryEngine
{
public:
	HistoryEngine();
	virtual ~HistoryEngine();
	bool saveHistoryMessage(const HistoryItem &item);
	bool saveSystemMessage(const HistoryItem &item);
	QList<HistoryItem> loadHistoryMessage(const TreeModelItem &item, int n, const QDateTime &date_last);
	void setHistoryPath(const QString &path);
private:
	QString fileName(const HistoryItem &item);
	QStringList fileNames(const TreeModelItem &item);
	QString hex(const QString &text);
	QDir m_history_path;
	QDir accountDir(const TreeModelItem &item);
};

#endif /*HISTORYENGINE_H_*/
