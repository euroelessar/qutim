/****************************************************************************
 * kdespellerlayer.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef KDESPELLERLAYER_H
#define KDESPELLERLAYER_H

#include <QObject>
#include <qutim/layerinterface.h>
#include <sonnet/speller.h>
#include <sonnet/highlighter.h>

using namespace qutIM;
using namespace Sonnet;

class KdeSpellerLayer : public QObject, public SpellerLayerInterface
{
	Q_OBJECT
public:
	KdeSpellerLayer();
	virtual bool init( PluginSystemInterface *plugin_system );
	virtual void release();
	virtual void setProfileName( const QString &profile_name );
	virtual void setLayerInterface( LayerType type, LayerInterface *layer_interface );

	void loadSettings();

	virtual void saveLayerSettings();
	virtual QList<SettingsStructure> getLayerSettingsList();
	virtual void removeLayerSettings();

	virtual void saveGuiSettingsPressed();
	virtual QList<SettingsStructure> getGuiSettingsList();
	virtual void removeGuiLayerSettings();

	virtual void startSpellCheck( QTextEdit *document );
	virtual void stopSpellCheck( QTextEdit *document );
	virtual bool isCorrect( const QString &word ) const;
	virtual bool isMisspelled( const QString &word ) const;
	virtual QStringList suggest( const QString &word ) const;
private:
	Speller *m_speller;
	QMap<QTextEdit *, QPointer<Highlighter> > m_highlighters;
	QString m_profile_name;
	PluginSystemInterface *m_plugin_system;
	bool m_autodetect;
	QString m_dictionary;
};

#endif // KDESPELLERLAYER_H
