 /*
    AbstractEmoticonsLayer

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef ABSTRACTEMOTICONSLAYER_H_
#define ABSTRACTEMOTICONSLAYER_H_
#include "../../../include/qutim/plugininterface.h"
#include "../../../include/qutim/layerinterface.h"
using namespace qutim_sdk_0_2;

namespace CoreEmoticons
{

class AbstractEmoticonsLayer: public EmoticonsLayerInterface, public EventHandler
{
public:
	AbstractEmoticonsLayer();
	virtual ~AbstractEmoticonsLayer() {}
	void loadSettings();
	virtual void release() {}
	virtual void setProfileName(const QString &profile_name);
	virtual void setLayerInterface( LayerType type, LayerInterface *linterface){}

	virtual void saveLayerSettings() {}
	virtual QList<SettingsStructure> getLayerSettingsList()
	{
		QList<SettingsStructure> list;
		return list;
	}
	virtual void removeLayerSettings() {}

	virtual bool init(PluginSystemInterface *plugin_system) {return true;}

	virtual void saveGuiSettingsPressed() {}

	virtual void removeGuiLayerSettings() {}

	void processEvent(Event &e) {}

	virtual QHash<QString,QStringList> getEmoticonsList() { return m_emoticon_list; }
	virtual void checkMessageForEmoticons(QString &message);
	static bool lengthLessThan(const QString &s1, const QString &s2);
	virtual QString getEmoticonsPath() { return m_dir_path; }
private:
    void setEmoticonPath(const QString &path);
    QString m_profile_name;
    QString m_emoticons_path;
	QHash<QString, QStringList> m_emoticon_list;
    QHash<QString, QString> m_urls;
	QList<QPair<QString,QString> > m_emoticons;
    QString m_dir_path;

};

}

#endif /*ABSTRACTEMOTICONSLAYER_H_*/


