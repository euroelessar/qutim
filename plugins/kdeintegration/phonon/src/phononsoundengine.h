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

#ifndef PHONONSOUNDENGINE_H
#define PHONONSOUNDENGINE_H

#include "soundenginelayer.h"
#include <shared/shareddata.h>

using namespace qutim_sdk_0_2;

class PhononSoundEngine : public QObject, public LayerPluginInterface {
    Q_OBJECT
    Q_INTERFACES(qutim_sdk_0_2::PluginInterface)
    Q_DISABLE_COPY(PhononSoundEngine)
public:
    explicit PhononSoundEngine();
    virtual bool init(PluginSystemInterface *plugin_system)
	{
		PluginInterface::init(plugin_system);
		KdeIntegration::ensureActiveComponent();
        m_sound_layer = new SoundEngineLayerImpl();
        if(!plugin_system->setLayerInterface(SoundEngineLayer, m_sound_layer))
        {
            delete m_sound_layer;
            m_sound_layer = 0;
            return false;
        }
        return true;
    }
    virtual void release() {}
	virtual QString name() { return "KDE Phonon"; }
    virtual QString description() { return "Sound engine based on Phonon"; }
    virtual QIcon *icon() { return &m_icon; }
    virtual void setProfileName(const QString &) {}

protected:
        SoundEngineLayerImpl *m_sound_layer;
        QIcon m_icon;
};

#endif // PHONONSOUNDENGINE_H

