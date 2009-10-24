/*****************************************************************************
	System Info

	Copyright (c) 2007-2008 by Remko Tronçon
				  2008-2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QSysInfo>
#include <QProcess>
#include <QTextStream>
#include <QDateTime>
#include <QVector>
#include <QStringBuilder>
#include <QDesktopServices>
#include <QDebug>

#if defined(Q_WS_X11) || defined(Q_WS_MAC)
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#endif

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#ifdef Q_WS_MAC
#include <CoreServices/CoreServices.h>
#endif

#include "systeminfo.h"

namespace qutim_sdk_0_3
{
	SystemInfoPrivate *SystemInfo::p = 0;

	struct SystemInfoPrivate
	{
		inline SystemInfoPrivate() : dirs(SystemInfo::SystemShareDir + 1) {}
		QString os_full;
		QString os_name;
		QString os_version;
		quint8 os_type_id;
		quint32 os_version_id;
		QString timezone_str;
		int timezone_offset;
		QVector<QDir> dirs;
	};

	enum WinFlag
	{
		SuitePersonal   = 0x01,
		SuiteHomeServer = 0x02
	};

	#if defined(Q_WS_X11)
	static QString lsbRelease(const QStringList& args)
	{
		QStringList path = QString(qgetenv("PATH")).split(':');
		QString found;

		foreach(QString dirname, path) {
			QDir dir(dirname);
			QFileInfo cand(dir.filePath("lsb_release"));
			if (cand.isExecutable()) {
				found = cand.absoluteFilePath();
				break;
			}
		}

		if (found.isEmpty()) {
			return QString();
		}

		QProcess process;
		process.start(found, args, QIODevice::ReadOnly);

		if(!process.waitForStarted())
			return QString();   // process failed to start

		QTextStream stream(&process);
		QString ret;

		while(process.waitForReadyRead())
		   ret += stream.readAll();

		process.close();
		return ret.trimmed();
	}


	static QString unixHeuristicDetect() {

		QString ret;

		struct utsname u;
		uname(&u);
    ret.sprintf("%s", u.sysname);

		enum OsFlags {
			OsUseName = 0,
			OsUseFile,
			OsAppendFile
		};

    struct OsInfo {
			OsFlags flags;
			QString file;
			QString name;
		} osInfo[] = {
      { OsUseFile,		"/etc/altlinux-release",	"Alt Linux"			},
      { OsUseFile,		"/etc/mandrake-release",	"Mandrake Linux"	},
      { OsAppendFile,	"/etc/debian_version",		"Debian GNU/Linux"	},
      { OsUseFile,		"/etc/gentoo-release",		"Gentoo Linux"		},
      { OsAppendFile,	"/etc/mopslinux-version",	"MOPSLinux"},
      { OsAppendFile,	"/etc/slackware-version",	"Slackware Linux"	},
	  { OsUseFile,		"/etc/pld-release",			"PLD Linux"			},
	  { OsUseName,		"/etc/aurox-release",		"Aurox Linux"		},
	  { OsUseFile,		"/etc/arch-release",		"Arch Linux"		},
	  { OsAppendFile,	"/etc/lfs-release",			"LFS Linux"			},

			// untested
	  { OsUseFile,		"/etc/SuSE-release",		"SuSE Linux"		},
      { OsUseFile,		"/etc/conectiva-release",	"Conectiva Linux"	},
	  { OsUseFile,		"/etc/.installed",			"Caldera Linux"		},

			// many distros use the /etc/redhat-release for compatibility, so RedHat will be the last :)
	  { OsUseFile,		"/etc/redhat-release",		"RedHat Linux"		}
		};

    for (int i = 0, size = sizeof(osInfo)/sizeof(OsInfo); i < size; i++) {
			QFileInfo fi( osInfo[i].file );
			if ( fi.exists() ) {
				char buffer[128];

				QFile f( osInfo[i].file );
        if(!f.open( QIODevice::ReadOnly ))
          continue;

        f.readLine( buffer, 128 );

        QString desc = QString::fromUtf8(buffer).simplified ();//stripWhiteSpace ();

				switch ( osInfo[i].flags ) {
					case OsUseFile:
						ret = desc;
						if(!ret.isEmpty())
							break;
					case OsUseName:
						ret = osInfo[i].name;
						break;
					case OsAppendFile:
						ret = osInfo[i].name + " (" + desc + ")";
						break;
				}

				break;
			}
		}
		return ret;
	}
	#endif

	SystemInfo::SystemInfo()
	{
	}

	SystemInfo::~SystemInfo()
	{
	}

	void system_info_ensure_private_helper(SystemInfoPrivate * &p)
	{
		if(p) return;
//		QDateTime tmp_datetime = QDateTime::currentDateTime().toLocalTime();
//		p->timezone_offset = tmp_datetime.utcOffset();
		// Initialize
		qDebug() << QDesktopServices::storageLocation(QDesktopServices::DataLocation);
		p = new SystemInfoPrivate;
		p->dirs[SystemInfo::ConfigDir]         = QDir::homePath() % QLatin1Literal("/.qutim/profiles/default/config");
		p->dirs[SystemInfo::HistoryDir]        = QDir::homePath() % QLatin1Literal("/.qutim/profiles/default/history");
		p->dirs[SystemInfo::ShareDir]          = QDir::homePath() % QLatin1Literal("/.qutim/share");
		p->dirs[SystemInfo::SystemConfigDir]   = QLatin1String("/etc/qutim");
		p->dirs[SystemInfo::SystemShareDir]    = QDir::currentPath() % QLatin1Literal("/../share");
		p->timezone_offset = 0;
		p->timezone_str = "N/A";
		p->os_full = "Unknown";
#if defined(Q_OS_WINCE)
		p->os_type_id = 'c';
#elif defined(Q_OS_WIN32)
		p->os_type_id = 'w';
#elif defined(Q_OS_LINUX)
		p->os_type_id = 'l';
#elif defined(Q_OS_MAC)
		p->os_type_id = 'm';
#elif defined(Q_OS_SYMBIAN)
		p->os_type_id = 's';
#elif defined(Q_OS_UNIX)
		p->os_type_id = 'u';
#else
		p->os_type_id = '\0';
#endif

		// Detect
#if defined(Q_WS_X11) || defined(Q_WS_MAC)
		time_t x;
		time(&x);
		char str[256];
		char fmt[32];
		strcpy(fmt, "%z");
		strftime(str, 256, fmt, localtime(&x));
		if(strcmp(fmt, str)) {
			int offset;
			QString s = str;
			if(s.at(0) == '+')
			{
				s.remove(0,1);
				offset = 1;
			}
			else if(s.at(0) == '-')
			{
				s.remove(0,1);
				offset = -1;
			}
			else
				offset = 1;
			int tmp = s.toInt();
			offset *= (tmp/100)*60 + tmp%100;
			p->timezone_offset = offset;
		}
		strcpy(fmt, "%Z");
		strftime(str, 256, fmt, localtime(&x));
		if(strcmp(fmt, str))
			p->timezone_str = str;
#endif
#if defined(Q_WS_X11)
		// attempt to get LSB version before trying the distro-specific approach

		p->os_full = lsbRelease(QStringList() << "--description" << "--short");

		if(p->os_full.isEmpty()) {
			p->os_full = unixHeuristicDetect();
		}
		else {
			p->os_name = lsbRelease(QStringList() << "--short" << "--id");
			p->os_version = lsbRelease(QStringList() << "--short" << "--release");;
		}

#elif defined(Q_WS_MAC)
		SInt32 minor_version, major_version, bug_fix;
		Gestalt(gestaltSystemVersionMajor, &major_version);
		Gestalt(gestaltSystemVersionMinor, &minor_version);
		Gestalt(gestaltSystemVersionBugFix, &bug_fix);
		p->os_version_id = (quint8(major_version) << 24) | (quint8(minor_version) << 16) | (quint8(bug_fix) << 8);
		p->os_name = "MacOS X";
		p->os_version = QString("%1.%2.%3").arg(major_version, minor_version, bug_fix);
		p->os_full = p->os_name;
		p->os_full += " ";
		p->os_full += p->os_version;
#endif

#if defined(Q_WS_WIN)
		TIME_ZONE_INFORMATION i;
		//GetTimeZoneInformation(&i);
		//p->timezone_offset = (-i.Bias) / 60;
		memset(&i, 0, sizeof(i));
		bool inDST = (GetTimeZoneInformation(&i) == TIME_ZONE_ID_DAYLIGHT);
		int bias = i.Bias;
		if(inDST)
			bias += i.DaylightBias;
		p->timezone_offset = -bias;
		p->timezone_str = "";
		for(int n = 0; n < 32; ++n) {
			int w = inDST ? i.DaylightName[n] : i.StandardName[n];
			if(w == 0)
				break;
			p->timezone_str += QChar(w);
		}
		p->os_full = QString();
		p->os_name = "Windows";
		OSVERSIONINFOEX osvi;
		BOOL bOsVersionInfoEx;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
		{
			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) )
				return;
		}
		quint8 special_info = 0;
		if(osvi.wSuiteMask & VER_SUITE_PERSONAL)
			special_info |= SuitePersonal;
		if(osvi.wSuiteMask & VER_SUITE_WH_SERVER)
			special_info |= SuiteHomeServer;
		p->os_version_id = (quint8(osvi.dwMajorVersion) << 24) | (quint8(osvi.dwMinorVersion) << 16)
						   | (quint8(osvi.wProductType) << 8)  | special_info;
#endif
	}

	inline void system_info_ensure_private(SystemInfoPrivate * &p)
	{ if(!p) system_info_ensure_private_helper(p); }

	QString SystemInfo::getFullName()
	{
		system_info_ensure_private(p);
		return p->os_full;
	}

	QString SystemInfo::getName()
	{
		system_info_ensure_private(p);
		return p->os_name;
	}

	QString SystemInfo::getVersion()
	{
		system_info_ensure_private(p);
		return p->os_version.isEmpty() ? (p->os_version = systemID2String(p->os_type_id, p->os_version_id)) : p->os_version;
//		return p->os_version;
	}

	quint32 SystemInfo::getSystemVersionID()
	{
		system_info_ensure_private(p);
		return p->os_version_id;
	}

	quint8 SystemInfo::getSystemTypeID()
	{
		system_info_ensure_private(p);
		return p->os_type_id;
	}

	QString SystemInfo::systemID2String(quint8 type, quint32 id)
	{
		system_info_ensure_private(p);
		QString str;
		quint8 VER_NT_WORKSTATION = 0x01;
		switch(type)
		{
		case 'm':
			if(id)
			{
				quint8 major, minor, bugfix;
				major = (id >> 24) & 0xff;
				minor = (id >> 16) & 0xff;
				bugfix = (id >> 8) & 0xff;
				str = QString("MacOS X %1.%2.%3").arg(QString::number(major), QString::number(minor), QString::number(bugfix));
			}
			else
				str += "MacOS X";
			break;
		case 'c':
			str += "Windows CE";
			break;
		case 'l':
			str += "Linux";
			break;
		case 's':
			str += "Symbian";
			break;
		case 'u':
			str += "*nix";
			break;
		case 'w': {
			str = "Windows";
			quint16 version = (id >> 16) & 0xffff;
			quint8 product  = (id >> 8) & 0xff;
			quint8 winflag  = id & 0xff;
			switch(version)
			{
			case 0x0500:
				str += " 2000";
			case 0x0501:
				str += " XP";
				if(winflag & 0x01)
					str += " Home Edition";
				else
					str += " Professional";
				break;
			case 0x0502:
				if(winflag & 0x02)
					str += " Home Server";
				else
					str += " Server 200";
			case 0x0600:
				if(product == VER_NT_WORKSTATION)
				{
					str += " Vista";
					if(winflag & 0x01)
						str += " Home";
				}
				else
					str += " Server 2008";
			case 0x0601:
				if(product == VER_NT_WORKSTATION)
					str += " 7";
				else
					str += " Server 2008 R2";
				break;
			default:
				str += " NT ";
				str += QString::number(version >> 8);
				str += ".";
				str += QString::number(version & 0xff);
			case 0x0000:
				break;
			}
			break; }
		default:
			str = "Unknown";
		}
		return str;
	}

	QString SystemInfo::getTimezone()
	{
		system_info_ensure_private(p);
		return p->timezone_str;
	}

	int SystemInfo::getTimezoneOffset()
	{
		system_info_ensure_private(p);
		return p->timezone_offset;
	}

	QDir SystemInfo::getDir(DirType type)
	{
		system_info_ensure_private(p);
		if(type >= p->dirs.size())
			return QDir();
		return p->dirs.at(type);
	}

	QString SystemInfo::getPath(DirType type)
	{
		system_info_ensure_private(p);
		if(type >= p->dirs.size())
			return QString();
		return p->dirs.at(type).absolutePath();
	}
}
