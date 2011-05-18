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
