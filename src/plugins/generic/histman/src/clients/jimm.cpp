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
//
//#include "jimm.h"
//#include <QString>
//#include <QDir>
//#include <QFile>
//#include <QTextStream>
//#include <QProgressBar>
//#include <QDateTime>
//#include "dbh.h"
//
//jimm::jimm()
//{
//}
//
//jimm::~jimm()
//{
//}
//
//void jimm::load(QString path,DataBase *data,QByteArray /*defCharset*/,QString /*nickName*/,QProgressBar *bar)
//{
//	QDir dir(path);
//	QFileInfoList files = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
//	QString account = "jimm uin";
//	bar->setMaximum(files.size());
//	for(int i=0;i<files.size();i++)
//	{
//		bar->setValue(i);
//		if(files[i].fileName().toLower().compare("_srvlog.txt")!=0&&files[i].fileName().toLower().compare("_botlog.txt")!=0)
//		{
//			QFile file(files[i].absoluteFilePath());
//			if (file.open(QIODevice::ReadOnly | QIODevice::Text))
//			{
//				DataBase::Message message;
//				message.type=1;
//				QTextStream in(&file);
//				in.setAutoDetectUnicode(false);
//				in.setCodec("cp1251");
//				QString uin = files[i].fileName();
//				uin=uin.remove(0,uin.lastIndexOf("_")+1);
//				uin.truncate(uin.size()-4);
//				message.in=true;
//				bool first=true;
//				bool point=false;
//				QStringList all = in.readAll().split("\n");
//				foreach(QString line, all)
//				{
//					if(line.compare("------------------------------------>>>-")==0||line.compare("------------------------------------<<<-")==0)
//					{
//						if(!first)
//						{
//							message.msg.truncate(message.msg.size()-2);
//							data->append("ICQ",account,uin,message);
//						}
//						else
//							first=false;
//						if(line[38]=='<')
//							message.in=true;
//						else
//							message.in=false;
//						point=true;
//						message.msg="";
//					}
//					else if(point)
//					{
//						message.date = QDateTime().fromString(line.section(' ',-2),"(dd.MM.yyyy hh:mm:ss):").toTime_t();
//						point=false;
//					}
//					else
//						message.msg+=line+"\n";
//				}
//			}
//		}
//	}
//}

