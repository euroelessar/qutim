/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2008 Remko Tronçon <remko@el-tramo.be>
** Copyright © 2011 Nicolay Izoderov <nico-izo@yandex.ru>
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
#include <QLibrary>

#ifdef Q_OS_SYMBIAN
#include <QTextCodec>
#endif

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#endif

#ifdef Q_OS_WIN
#include <windows.h>

// Nasty hack for VER_SUITE_PERSONAL =)
#ifndef VER_SUITE_PERSONAL
# define VER_SUITE_PERSONAL 0x00000200
#endif

// Nasty hack for VER_SUITE_WH_SERVER =)
#ifndef VER_SUITE_WH_SERVER
# define VER_SUITE_WH_SERVER 0x00008000
#endif

#endif

// Nasty hack for VER_NT_WORKSTATION.
//TODO: It is used even for *nix
#ifndef VER_NT_WORKSTATION
# define VER_NT_WORKSTATION 0x0000001
#endif

#ifdef Q_OS_MAC
#include <CoreServices/CoreServices.h>
#endif

// Haiku!
#if defined(Q_OS_HAIKU)
#include <sys/utsname.h>
#include <Path.h>
#include <FindDirectory.h>
#include <AppFileInfo.h>
#include <File.h>
#endif

#include "systeminfo.h"

//typedef QString (*_qutim_system_info_hook)(const QString &os_full, const QString &os_name, const QString &os_version);
//LIBQUTIM_EXPORT _qutim_system_info_hook qutim_system_info_hook = 0;

namespace qutim_sdk_0_3
{
struct SystemInfoPrivate
{
	SystemInfoPrivate();

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

#if defined(Q_OS_LINUX) && !defined(Q_OS_FREEBSD) && !defined(MEEGO_EDITION) && !defined(Q_OS_MAEMO_5) && !defined(Q_OS_HAIKU)
static bool lsbRelease(SystemInfoPrivate *d)
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
		return false;
	}

	QProcess process;
    process.start(found, QStringList(QStringLiteral("-dirs")), QIODevice::ReadOnly);

	if(!process.waitForStarted())
		return false;   // process failed to start

	QTextStream stream(&process);
	QString ret;

	while (process.waitForReadyRead())
		ret += stream.readAll();

    process.close();

    QStringList list = ret.split(QLatin1Char('\n'), QString::KeepEmptyParts);

    if (list.size() >= 3) {
        auto fixed = [] (QString value) {
            value = value.trimmed();
            if (value.startsWith(QLatin1Char('"')))
				value.remove(0, 1);
			if (value.endsWith(QLatin1Char('"')))
				value.chop(1);
            return value;
        };

        d->os_name = fixed(list[0]);
        d->os_full = fixed(list[1]);
        d->os_version = fixed(list[2]);
        return true;
    }

    return false;
}

enum OsFlags {
	OsUseName = 0,
	OsUseFile,
	OsAppendFile
};

struct OsInfo
{
	OsFlags flags;
	QString file;
	QString name;
};

struct OsRelease
{
	const char *fileName;
	const char *name;
	const char *version;
	const char *prettyName;
};

static bool osReleaseDetect(SystemInfoPrivate *d)
{
	OsRelease files[] = {
		{ "/etc/os-release",  "NAME",       "VERSION",         "PRETTY_NAME"         },
		{ "/etc/lsb-release", "DISTRIB_ID", "DISTRIB_RELEASE", "DISTRIB_DESCRIPTION" }
	};
	
	QFile releaseFile;
	for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); ++i) {
		OsRelease &release = files[i];
		releaseFile.setFileName(QLatin1String(release.fileName));
		if (!releaseFile.open(QFile::ReadOnly))
			continue;
		const QString content = QString::fromUtf8(releaseFile.readAll());
		foreach (const QString &line, content.split(QLatin1Char('\n'), QString::SkipEmptyParts)) {
			const QString name = line.section(QLatin1Char('='), 0, 0);
			QString value = line.section(QLatin1Char('='), 1);
			if (value.startsWith(QLatin1Char('"')))
				value.remove(0, 1);
			if (value.endsWith(QLatin1Char('"')))
				value.chop(1);
			
			if (name == QLatin1String(release.name))
				d->os_name = value;
			else if (name == QLatin1String(release.version))
				d->os_version = value;
			else if (name == QLatin1String(release.prettyName))
				d->os_full = value;
		}
		return true;
	}
	return false;
}

static QString unixHeuristicDetect(SystemInfoPrivate *d)
{
	QString ret;

	struct utsname u;
	uname(&u);
	d->os_name = QLatin1String(u.sysname);
	d->os_version = QLatin1String(u.release);

	OsInfo osInfo[] = {
		{ OsUseFile,		"/etc/altlinux-release",	"Alt Linux"		},
		{ OsUseFile,		"/etc/mandrake-release",	"Mandrake Linux"	},
		{ OsAppendFile,		"/etc/debian_version",		"Debian GNU/Linux"	},
		{ OsUseFile,		"/etc/gentoo-release",		"Gentoo Linux"		},
		{ OsAppendFile,		"/etc/mopslinux-version",	"MOPSLinux"		},
		{ OsAppendFile,		"/etc/slackware-version",	"Slackware Linux"	},
		{ OsUseFile,		"/etc/pld-release",			"PLD Linux"		},
		{ OsUseName,		"/etc/aurox-release",		"Aurox Linux"		},
		{ OsUseFile,		"/etc/arch-release",		"Arch Linux"		},
		{ OsAppendFile,		"/etc/lfs-release",			"LFS Linux"		},

		// untested
		{ OsUseFile,		"/etc/SuSE-release",		"SuSE Linux"		},
		{ OsUseFile,		"/etc/conectiva-release",	"Conectiva Linux"	},
		{ OsUseFile,		"/etc/.installed",			"Caldera Linux"		},
		{ OsAppendFile,		"/etc/agilialinux-version",	"AgiliaLinux"		},
		// from libastral
		{ OsUseName,		"/etc/exherbo-release",		"Exherbo Linux"	 },


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

			QString desc = QString::fromUtf8(buffer).simplified();
			
			d->os_name = osInfo[i].name;
			d->os_version.clear();

			switch ( osInfo[i].flags ) {
			case OsUseFile:
				d->os_full = desc;
				if(!d->os_full.isEmpty())
					break;
			case OsUseName:
				d->os_full = d->os_name;
				break;
			case OsAppendFile:
				d->os_version = desc;
				d->os_full = d->os_name + " " + d->os_version;
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

SystemInfoPrivate::SystemInfoPrivate() : dirs(SystemInfo::SystemShareDir + 1)
{
    auto d = this;
	//		QDateTime tmp_datetime = QDateTime::currentDateTime().toLocalTime();
	//		d->timezone_offset = tmp_datetime.utcOffset();
	// Initialize
	d->dirs[SystemInfo::ConfigDir]         = QDir::homePath() % QLatin1Literal("/.qutim/profiles/default/config");
	d->dirs[SystemInfo::HistoryDir]        = QDir::homePath() % QLatin1Literal("/.qutim/profiles/default/history");
	d->dirs[SystemInfo::ShareDir]          = QDir::homePath() % QLatin1Literal("/.qutim/share");
#if defined(QUTIM_SHARE_DIR)
	d->dirs[SystemInfo::SystemConfigDir]   = qApp->applicationDirPath() % QLatin1Literal("/../") % QLatin1Literal(QUTIM_SHARE_DIR) % QLatin1Literal("/config");
	d->dirs[SystemInfo::SystemShareDir]    = qApp->applicationDirPath() % QLatin1Literal("/../") % QLatin1Literal(QUTIM_SHARE_DIR);
#elif defined(Q_OS_SYMBIAN)
	d->dirs[SystemInfo::SystemConfigDir]   = QLatin1String(":/config");
	d->dirs[SystemInfo::SystemShareDir]    = QLatin1String("e:/system/apps/qutim/share");
#else
# error QUTIM_SHARE_DIR undefined!
#endif
	d->timezone_offset = 0;
	d->timezone_str = "N/A";
	d->os_full = "Unknown";
#if defined(Q_OS_WINCE)
	d->os_type_id = SystemInfo::WinCE;
#elif defined(Q_OS_WIN32)
	d->os_type_id = SystemInfo::Win32;
#elif defined(Q_OS_LINUX)
	d->os_type_id = SystemInfo::Linux;
#elif defined(Q_OS_MAC)
	d->os_type_id = SystemInfo::MacOSX;
#elif defined(Q_OS_SYMBIAN)
	d->os_type_id = SystemInfo::Symbian;
#elif defined(Q_OS_UNIX)
	d->os_type_id = SystemInfo::Unix;
#else
	d->os_type_id = '\0';
#endif
	d->os_version_id = 0;

	// Detect
#ifdef Q_OS_UNIX
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
		d->timezone_offset = offset;
	}
	strcpy(fmt, "%Z");
	strftime(str, 256, fmt, localtime(&x));
	if(strcmp(fmt, str))
		d->timezone_str = str;
#endif

#if defined(Q_OS_FREEBSD)
	QProcess processUname;
	processUname.start(QLatin1String("uname"),
					   QStringList() << QLatin1String("-r") << QLatin1String("-p"));
	if(!processUname.waitForFinished(1000))
		return;
	d->os_name = QLatin1String("FreeBSD");
	d->os_version = processUname.readAll();
	d->os_full = d->os_name % QLatin1Char(' ') % d->os_version;
#elif defined(Q_OS_HAIKU)
	// Native Haiku detect, using libBe
	BPath path;
	if ( find_directory(B_BEOS_LIB_DIRECTORY, &path) == B_OK ) {
		path.Append("libbe.so");

		BAppFileInfo appFileInfo;
		version_info versionInfo;
		BFile file;
		if ( file.SetTo(path.Path(), B_READ_ONLY) == B_OK
			 && appFileInfo.SetTo(&file) == B_OK
			 && appFileInfo.GetVersionInfo(&versionInfo, B_APP_VERSION_KIND) == B_OK
			 && versionInfo.short_info[0] != '\0') {
			d->os_name = versionInfo.short_info;
		}
	}

	utsname uname_info;
	if (uname(&uname_info) == 0) {
		d->os_full = uname_info.sysname;
		long revision = 0;
		if (sscanf(uname_info.version, "r%ld", &revision) == 1 ||
				sscanf(uname_info.version, "hrev%ld", &revision) == 1) {
			
			char version[16];
			snprintf(version, sizeof(version), "%ld", revision);
			d->os_full += " (" + d->os_name + " Rev. ";
			d->os_full += version;
			d->os_full += ")";
		}
	}
#elif defined(Q_OS_MAEMO_5)
	d->os_full="Maemo 5 Nokia N900";
	d->os_name="Maemo";
	d->os_version="5";
#elif defined(MEEGO_EDITION)
	d->os_name = QLatin1String("MeeGo");
	d->os_version = QString(QLatin1String("%1.%2"))
					.arg(MEEGO_VERSION_MAJOR)
					.arg(MEEGO_VERSION_MINOR);
	//	        .arg(MEEGO_VERSION_PATCH)
#ifdef MEEGO_EDITION_HARMATTAN
	d->os_version += QLatin1String(" Harmattan");
#endif
	d->os_full = d->os_name + ' ' + d->os_version;
#elif defined(Q_OS_LINUX)

	// Firstly try to get info from "/etc/os-release" or compatible as it's faster then invoking lsb_release
	if (!osReleaseDetect(d)) {
		// attempt to get LSB version before trying the distro-specific approach
        if (!lsbRelease(d))
            unixHeuristicDetect(d);
	}


#elif defined(Q_OS_MAC)
	SInt32 minor_version, major_version, bug_fix;
	Gestalt(gestaltSystemVersionMajor, &major_version);
	Gestalt(gestaltSystemVersionMinor, &minor_version);
	Gestalt(gestaltSystemVersionBugFix, &bug_fix);
	d->os_version_id = (quint8(major_version) << 24) | (quint8(minor_version) << 16) | (quint8(bug_fix) << 8);
	d->os_name = "MacOS X";
	d->os_version = QString("%1.%2.%3").arg(major_version).arg(minor_version).arg(bug_fix);
	d->os_full = d->os_name;
	d->os_full += " ";
	d->os_full += d->os_version;
#endif

#if defined(Q_OS_WIN)
	TIME_ZONE_INFORMATION i;
	//GetTimeZoneInformation(&i);
	//d->timezone_offset = (-i.Bias) / 60;
	memset(&i, 0, sizeof(i));
	bool inDST = (GetTimeZoneInformation(&i) == TIME_ZONE_ID_DAYLIGHT);
	int bias = i.Bias;
	if(inDST)
		bias += i.DaylightBias;
	d->timezone_offset = -bias;
	d->timezone_str = "";
	for(int n = 0; n < 32; ++n) {
		int w = inDST ? i.DaylightName[n] : i.StandardName[n];
		if(w == 0)
			break;
		d->timezone_str += QChar(w);
	}
	d->os_full = QString();
	d->os_name = QLatin1String("Windows");
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
	d->os_version_id = (quint8(osvi.dwMajorVersion) << 24) | (quint8(osvi.dwMinorVersion) << 16)
					   | (quint8(osvi.wProductType) << 8)  | special_info;

	d->os_full = SystemInfo::systemID2String(d->os_type_id, d->os_version_id);
	d->os_version = d->os_full.section(' ', 1);
#endif
#ifdef Q_OS_SYMBIAN
	//		QLibrary hal("hal.dll");
	//		typedef TInt (halGet_*)(HALData::TAttribute,TInt&);
	//		halGet_ halGet = (halGet_) QLibrary::resolve("hal.dll", "HAL::Get");
	//		if (halGet) {
	//			d->os_version_id = (*halGet)()
	//		}
	d->os_name = QLatin1String("Symbian");
	QFile productFile(QLatin1String("z:/resource/versions/product.txt"));
	if (productFile.open(QFile::ReadOnly)) {
		const QTextCodec * const codec = QTextCodec::codecForName("utf-16");
		QByteArray data = productFile.readAll();
		QString text = codec->toUnicode(data, data.size());
		QString manufacturer;
		QString model;
		foreach (QString line, text.split(QLatin1Char('\n'))) {
			if (line.endsWith(QLatin1Char('\r')))
				line.chop(1);
			if (line.startsWith(QLatin1String("Manufacturer=")))
				manufacturer = line.section(QLatin1Char('='), 1);
			else if (line.startsWith(QLatin1String("Model=")))
				model = line.section(QLatin1Char('='), 1);
		}
		d->os_version = manufacturer;
		if (!d->os_version.isEmpty())
			d->os_version += QLatin1String(" ");
		d->os_version += model;
	}
	if (d->os_version.isEmpty())
		d->os_full = d->os_name;
	else
		d->os_full = d->os_name + " (" + d->os_version + ")";
#endif
}

Q_GLOBAL_STATIC(SystemInfoPrivate, d_func)

LIBQUTIM_EXPORT QVector<QDir> *system_info_dirs()
{ return &d_func()->dirs; }


QString SystemInfo::getFullName()
{
	Q_D(SystemInfo);
	return d->os_full;
}

QString SystemInfo::getName()
{
	Q_D(SystemInfo);
	return d->os_name;
}

QString SystemInfo::getVersion()
{
	Q_D(SystemInfo);
	return d->os_version.isEmpty() ? (d->os_version = systemID2String(d->os_type_id, d->os_version_id)) : d->os_version;
	//		return d->os_version;
}

quint32 SystemInfo::getSystemVersionID()
{
	Q_D(SystemInfo);
	return d->os_version_id;
}

quint8 SystemInfo::getSystemTypeID()
{
	Q_D(SystemInfo);
	return d->os_type_id;
}

QString SystemInfo::systemID2String(quint8 type, quint32 id)
{
	QString str;
	switch(type)
	{
	case 'm':
		if (id) {
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
		switch (version) {
		case 0x0500:
			str += " 2000";
			break;
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
				str += " Server 2003";
			break;
		case 0x0600:
			if( product == VER_NT_WORKSTATION) {
				str += " Vista";
				if(winflag & 0x01)
					str += " Home";
			}
			else
				str += " Server 2008";
			break;
		case 0x0601:
			if(product == VER_NT_WORKSTATION)
				str += " 7";
			else
				str += " Server 2008 R2";
			break;
        case 0x0602:
			if(product == VER_NT_WORKSTATION)
				str += " 8";
			else
				str += " Server 2012";
			break;
        case 0x0603:
			if(product == VER_NT_WORKSTATION)
				str += " 8.1";
			else
				str += " Server 2012 R2";
			break;
		default:
			str += " NT ";
			str += QString::number(version >> 8);
			str += ".";
			str += QString::number(version & 0xff);
			break;
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
	Q_D(SystemInfo);
	return d->timezone_str;
}

int SystemInfo::getTimezoneOffset()
{
	Q_D(SystemInfo);
	return d->timezone_offset;
}

QDir SystemInfo::getDir(DirType type)
{
	Q_D(SystemInfo);
	if(type >= d->dirs.size())
		return QDir();
	return d->dirs.at(type);
}

QString SystemInfo::getPath(DirType type)
{
	Q_D(SystemInfo);
	if(type >= d->dirs.size())
		return QString();
	return d->dirs.at(type).absolutePath();
}
}

