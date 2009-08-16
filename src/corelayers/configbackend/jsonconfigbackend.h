/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
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
	public:
		Q_INVOKABLE JsonConfigBackend();
		virtual ConfigEntry::Ptr parse(const QString& file);
		virtual void generate(const QString& file, const qutim_sdk_0_3::ConfigEntry::Ptr& entry);
        private:
                QVariant genetateQVariant (const qutim_sdk_0_3::ConfigEntry::Ptr& entry); //recursive
                qutim_sdk_0_3::ConfigEntry::Ptr generateConfigEntry ( const QVariant &val);
        };

}
using namespace Core;


#endif // JSONCONFIGBACKEND_H
