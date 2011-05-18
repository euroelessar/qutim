#ifndef PLUGLIBINFO_H
#define PLUGLIBINFO_H

#include <QString>
#include "utils/plugversion.h"
#include "plugpackage.h"

class plugLibInfo
{
public:
	enum Type { QtLib, QtPlugin, QutimPlugin, Library, Invalid };
	plugLibInfo(const QString &lib_path);
	inline Type type() const { return m_type; }
	inline bool isValid() const { return type() != Invalid; }
	packageInfo toInfo() const;
private:
	Type m_type;
	QString m_magic_type;
	QString m_lib_path;
	QString m_description;
	QString m_name;
	QString m_build_key;
	bool m_debug;
	plugVersion m_qt_version;
};

#endif // PLUGLIBINFO_H
