/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#include "pluglibinfo.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QLibrary>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QVector>
#include <QtCore/QDebug>
#include <qutim/plugininterface.h>

/*
  to avoid conflicts with Qt
*/
namespace PlugMan {

struct qt_token_info
{
	qt_token_info(const char *f, const ulong fc)
		: fields(f), field_count(fc), results(fc), lengths(fc)
	{
		results.fill(0);
		lengths.fill(0);
	}

	const char *fields;
	const ulong field_count;

	QVector<const char *> results;
	QVector<ulong> lengths;
};

/*
  return values:
	   1 parse ok
	   0 eos
	  -1 parse error
*/
static int qt_tokenize(const char *s, ulong s_len, ulong *advance,
						qt_token_info &token_info)
{
	ulong pos = 0, field = 0, fieldlen = 0;
	char current;
	int ret = -1;
	*advance = 0;
	for (;;) {
		current = s[pos];

		// next char
		++pos;
		++fieldlen;
		++*advance;

		if (! current || pos == s_len + 1) {
			// save result
			token_info.results[(int)field] = s;
			token_info.lengths[(int)field] = fieldlen - 1;

			// end of string
			ret = 0;
			break;
		}

		if (current == token_info.fields[field]) {
			// save result
			token_info.results[(int)field] = s;
			token_info.lengths[(int)field] = fieldlen - 1;

			// end of field
			fieldlen = 0;
			++field;
			if (field == token_info.field_count - 1) {
				// parse ok
				ret = 1;
			}
			if (field == token_info.field_count) {
				// done parsing
				break;
			}

			// reset string and its length
			s = s + pos;
			s_len -= pos;
			pos = 0;
		}
	}

	return ret;
}

/*
  returns true if the string s was correctly parsed, false otherwise.
*/
static bool qt_parse_pattern(const char *s, uint *version, bool *debug, QByteArray *key)
{
	bool ret = true;

	qt_token_info pinfo("=\n", 2);
	int parse;
	ulong at = 0, advance, parselen = qstrlen(s);
	do {
		parse = qt_tokenize(s + at, parselen, &advance, pinfo);
		if (parse == -1) {
			ret = false;
			break;
		}

		at += advance;
		parselen -= advance;

		if (qstrncmp("version", pinfo.results[0], pinfo.lengths[0]) == 0) {
			// parse version string
			qt_token_info pinfo2("..-", 3);
			if (qt_tokenize(pinfo.results[1], pinfo.lengths[1],
							  &advance, pinfo2) != -1) {
				QByteArray m(pinfo2.results[0], pinfo2.lengths[0]);
				QByteArray n(pinfo2.results[1], pinfo2.lengths[1]);
				QByteArray p(pinfo2.results[2], pinfo2.lengths[2]);
				*version  = (m.toUInt() << 16) | (n.toUInt() << 8) | p.toUInt();
			} else {
				ret = false;
				break;
			}
		} else if (qstrncmp("debug", pinfo.results[0], pinfo.lengths[0]) == 0) {
			*debug = qstrncmp("true", pinfo.results[1], pinfo.lengths[1]) == 0;
		} else if (qstrncmp("buildkey", pinfo.results[0],
							  pinfo.lengths[0]) == 0){
			// save buildkey
			*key = QByteArray(pinfo.results[1], pinfo.lengths[1]);
		}
	} while (parse == 1 && parselen > 0);

	return ret;
}

}

plugLibInfo::plugLibInfo(const QString &lib_path) : m_lib_path(lib_path)
{
	QLibrary lib(lib_path);
	lib.load();
#  ifdef Q_CC_BOR
	typedef const char * __stdcall (*QtPluginQueryVerificationDataFunction)();
#  else
	typedef const char * (*QtPluginQueryVerificationDataFunction)();
#  endif
	QtPluginQueryVerificationDataFunction qtPluginQueryVerificationDataFunction =
		(QtPluginQueryVerificationDataFunction) lib.resolve("qt_plugin_query_verification_data");
	QByteArray key;
	uint qt_version;
	if (!qtPluginQueryVerificationDataFunction
		|| !PlugMan::qt_parse_pattern(qtPluginQueryVerificationDataFunction(), &qt_version, &m_debug, &key)) {
		m_type = Invalid;
	} else {
		m_build_key = QString::fromLatin1(key);
		typedef QObject *(*QtPluginInstanceFunction)();
		QtPluginInstanceFunction instance = (QtPluginInstanceFunction) lib.resolve("qt_plugin_instance");
		if(instance)
		{
			m_type = QtPlugin;
			qutim_sdk_0_2::Plugin *plugin = qobject_cast<qutim_sdk_0_2::Plugin *>(instance());
			if(plugin)
			{
				m_type = QutimPlugin;
				m_name = plugin->name();
				m_description = plugin->description();
				m_magic_type = plugin->type();
			}
			delete plugin;
		}
		else
			m_type = Invalid;
	}
	lib.unload();
	m_qt_version.setVersion((qt_version & 0xff0000) >> 16, (qt_version & 0xff00) >> 8, qt_version & 0xff);
}

packageInfo plugLibInfo::toInfo() const
{
	packageInfo info;
	if(!isValid())
		return info;
	info.properties["name"] = m_name;
	{
		QString file = "plugins/";
		file += QFileInfo(m_lib_path).fileName();
		info.files << file;
	}
	info.properties["category"] = type() == QutimPlugin ? "plugin" : "lib";
	info.properties["type"] = m_magic_type;
	info.properties["description"] = m_description;
	info.properties["platform"] = m_build_key;
	info.properties["license"] = "GPL";
	info.dependencyList["qt"] = m_qt_version;
	return info;
}

