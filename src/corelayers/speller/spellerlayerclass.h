#ifndef SPELLERLAYERCLASS_H
#define SPELLERLAYERCLASS_H

#include "include/qutim/layerinterface.h"

#if 0
#include "spellerhighlighter.h"
#endif

using namespace qutim_sdk_0_2;

class SpellerLayerClass :
#if 0
		public QObject,
#endif
		public SpellerLayerInterface
{
#if 0
	Q_OBJECT
#endif
public:
    SpellerLayerClass();
	virtual bool init(PluginSystemInterface *plugin_system);
	virtual void release();
	virtual void setProfileName(const QString &profile_name);
	virtual void setLayerInterface( LayerType type, LayerInterface *layer_interface);

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
#if 0
private slots:
	void onDestruction( QObject *object );
private:
	QHash<QTextEdit *, SpellerHighlighter *> m_highlighters;
#endif
};

#endif // SPELLERLAYERCLASS_H
