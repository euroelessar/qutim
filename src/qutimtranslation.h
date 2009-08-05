#ifndef QUTIMTRANSLATION_H
#define QUTIMTRANSLATION_H

#include <qutim/plugininterface.h>

using namespace qutim_sdk_0_2;

class QutimTranslatorHook : public QTranslator
{
	virtual QString translate( const char *context, const char *sourceText, const char *comment = 0 ) const;
};

class QutimTranslation : public TranslatorInterface
{
	Q_DISABLE_COPY(QutimTranslation)
public:
	QutimTranslation( const QString &dir );
	virtual ~QutimTranslation();
	void init();
	void deinit();
	virtual QString translate( const char *context, const char *source_text, const char *comment = 0, int n = -1 ) const;
	virtual QString lang() const;
	virtual QLocale locale() const;
private:
	QString m_dir;
	QList<QTranslator *> *m_translators;
	QLocale *m_locale;
	QString *m_lang;
};

#endif // QUTIMTRANSLATION_H
