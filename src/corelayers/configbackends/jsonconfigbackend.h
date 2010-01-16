/*
	json backend for qutIM
	Copyright (C) <2010>  <Sidorov Aleksey sauron@citadelspb.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef JSONCONFIGBACKEND_H
#define JSONCONFIGBACKEND_H

#include "libqutim/configbackend.h"

using namespace qutim_sdk_0_3;

namespace Core
{
	class JsonConfigBackend : public ConfigBackend
	{
		Q_OBJECT
		Q_CLASSINFO("Extension", "json")
	public:
		Q_INVOKABLE JsonConfigBackend() {}
		virtual ConfigEntry::Ptr parse(const QString &file);
		virtual void generate(const QString &file, const ConfigEntry::Ptr &entry);
	protected:
		ConfigEntry::Ptr generateConfigEntry (const QVariant &val);
		QVariant generateQVariant(const ConfigEntry::Ptr& entry);
	};
}
using namespace Core;

#endif // JSONCONFIGBACKEND_H
