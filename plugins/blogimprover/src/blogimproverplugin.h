/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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

#ifndef BLOGIMPROVERPLUGIN_H
#define BLOGIMPROVERPLUGIN_H
#include <qutim/plugin.h>

#include <qutim/debug.h>
#include <qutim/config.h>
#include <qutim/settingslayer.h>
#include <qutim/chatsession.h>

#include "blogimproversettings.h"
#include "blogimproverhandler.h"

namespace qutim_sdk_0_3
{
	class SettingsItem;
}

namespace BlogImprover {

class BlogImproverHandler;
class BlogImproverPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
	Q_CLASSINFO("DebugName", "BlogImprover")
	Q_CLASSINFO("Uses", "ChatLayer")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
private:
	QScopedPointer<BlogImproverHandler> m_handler;
	QScopedPointer<qutim_sdk_0_3::SettingsItem> m_settingsItem;
};

} // namespace BlogImprover

#endif // BLOGIMPROVERPLUGIN_H
