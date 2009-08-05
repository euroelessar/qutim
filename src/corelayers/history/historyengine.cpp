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

#include "historyengine.h"

HistoryEngine::HistoryEngine()
{
}

HistoryEngine::~HistoryEngine()
{
}
bool HistoryEngine::saveHistoryMessage(const HistoryItem &item)
{
	/*QString line = item.m_time.toString("|yyyy-MM-ddThh:mm:ss|");
	line+=QString().setNum(item.m_type)+(item.m_in?"|in|":"|out|")+QString(item.m_message).replace("\\","\\\\").replace("\n","\\n")+"\n";
	QFile file(accountDir(item.m_user).filePath(fileName(item)));
	QFileInfo info(file);*/
	QFile file(accountDir(item.m_user).filePath(fileName(item)));
	if(!file.open(QIODevice::Append | QIODevice::WriteOnly))// | QIODevice::Text))
		return false;
	QDataStream out(&file);
	out << item.m_time << item.m_type << item.m_in << item.m_message;
#if 0
	QFile file(accountDir(item.m_user).filePath(fileName(item)));
	if(!file.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
		return false;
	file.write("{\n  \"datetime\": \"");
	file.write(item.m_time.toString(Qt::ISODate).toLatin1());
	file.write("\",\n  \"type\": ");
	file.write(QString::number(item.m_type).toLatin1());
	file.write(",\n  \"in\": ");
	file.write(item.m_in ? "true" : "false");
	file.write(",\n  \"text\": ");
	file.write(K8JSON::quote(item.m_message).toUtf8());
	file.write("\n}\n");
	file.close();
//  It will produce something like this:
//	{
//		"datetime": "2009-06-20T01:42:22",
//		"type": 1,
//		"in": true,
//		"text": "some cool text"
//	}
#endif
	/*QTextStream out(&file);
	out.setAutoDetectUnicode(false);
	out.setCodec("UTF-8");
	out << line;*/
	return true;
}
bool HistoryEngine::saveSystemMessage(const HistoryItem &item)
{
	//QString line = item.m_time.toString("|yyyy-MM-ddThh:mm:ss|");
	//line+=QString().setNum(item.m_type)+"|"+hex(item.m_user.m_item_name)+"|"+QString(item.m_message).replace("\\","\\\\").replace("\n","\\n");
	QFile file(accountDir(item.m_user).filePath("sys."+item.m_time.toString(".yyyyMM")+".log"));
	if(!file.open(QIODevice::Append | QIODevice::WriteOnly))// | QIODevice::Text))
		return false;
	QDataStream out(&file);
	out << item.m_time << item.m_type << item.m_user.m_item_name << item.m_message;
	/*QTextStream out(&file);
	out.setAutoDetectUnicode(false);
	out.setCodec("UTF-8");
	out << line;*/
	return true;
}
QList<HistoryItem> HistoryEngine::loadHistoryMessage(const TreeModelItem &item, int n, const QDateTime &date_last)
{

	int num=0;
	QVector<HistoryItem> vector(n);
	QDir dir = accountDir(item);
	QStringList files = fileNames(item);

	if(files.size()<1)
		return QList<HistoryItem>();
	for(int month=files.size()==1?1:2;month>0;month--)
	{	
		QFile file(dir.filePath(files[files.size()-month]));

		if (file.open(QIODevice::ReadOnly))
		{	
			QDataStream in(&file);
			HistoryItem history;
			while(!file.atEnd())
			{
				in >> history.m_time >> history.m_type >> history.m_in >> history.m_message;
				if(history.m_time < date_last)
				{
					vector[num%n] = history;
					num++;
				}
			}

		}
	}
	QList<HistoryItem> list;

	if(num>n)
		for(int i=num%n;i<n;i++)
			list.append(vector[i]);
	for(int i=0;i<num%n;i++)
		list.append(vector[i]);
	return list;

}
void HistoryEngine::setHistoryPath(const QString &path)
{
	m_history_path = QDir(path);
}
QString HistoryEngine::fileName(const HistoryItem &item)
{
	return hex(item.m_user.m_item_name) + item.m_time.toString(".yyyyMM") + ".log";
}
QStringList HistoryEngine::fileNames(const TreeModelItem &item)
{
	QDir dir = accountDir(item);
	if(!dir.exists())
		return QStringList();
	QStringList filters;
	filters << hex(item.m_item_name)+".*.log";
	return dir.entryList(filters, QDir::Readable | QDir::Files | QDir::NoDotAndDotDot,QDir::Name);
}
QString HistoryEngine::hex(const QString &text)
{
	return QString(text.toUtf8().toHex());
}
QDir HistoryEngine::accountDir(const TreeModelItem &item)
{
	QString path = item.m_protocol_name+"."+hex(item.m_account_name);
	QDir dir(m_history_path.filePath(path));
	if(!dir.exists())
		m_history_path.mkpath(path);
	return dir;
}
