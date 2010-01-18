#include "plughacks.h"
#include <QDir>
#include <QString>
#include <qutim/plugininterface.h>

using namespace qutim_sdk_0_2;
namespace plugPathes {
    
    QString getConfigPath()
    {
        QDir dir = SystemsCity::PluginSystem()->getProfileDir();
        dir.cdUp();
        return dir.absolutePath().append("/");
    }
    
    QDir getConfigDir()
    {
        QDir dir = SystemsCity::PluginSystem()->getProfileDir();
        dir.cdUp();
        return dir;
    }
    
    QString getCachePath()
    {
        return getConfigPath().append("/plugman/cache/");
    }
    
    
}
