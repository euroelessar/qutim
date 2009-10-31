#ifndef KDEEMOTICONSLAYER_H
#define KDEEMOTICONSLAYER_H

#include <QObject>
#include <qutim/layerinterface.h>
#include <kemoticons.h>

using namespace qutIM;

class KdeEmoticonsLayer : public EmoticonsLayerInterface  {
    Q_DISABLE_COPY(KdeEmoticonsLayer)
public:
	KdeEmoticonsLayer();
	KEmoticons *self();
	virtual bool init(PluginSystemInterface *plugin_system);
	virtual void release() {}
	virtual void setProfileName(const QString &profile_name);
	virtual void setLayerInterface( LayerType, LayerInterface *) {}

	virtual void saveLayerSettings() {}
	virtual QList<SettingsStructure> getLayerSettingsList() { return m_settings; }
	virtual void removeLayerSettings() {}

	virtual void saveGuiSettingsPressed();
	virtual QList<SettingsStructure> getGuiSettingsList() { return m_gui_settings; }
	virtual void removeGuiLayerSettings() {}
	virtual QHash<QString,QStringList> getEmoticonsList();
	virtual void checkMessageForEmoticons( QString &message );
	virtual QString getEmoticonsPath();
private:
	KEmoticonsTheme m_theme;
};

#endif // KDEEMOTICONSLAYER_H
