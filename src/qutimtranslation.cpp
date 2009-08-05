#include "qutimtranslation.h"
#include <QDir>
#include <QTranslator>

QString QutimTranslatorHook::translate( const char *context, const char *sourceText, const char *comment ) const
{
	TranslatorInterface *translator = SystemsCity::Translator();
	if(!translator)
		return QString();
	QString result;
	if(context && context[0])
		result = translator->translate(context, sourceText, comment);
	if(result.isEmpty())
		result = translator->translate(0, sourceText, comment);
	return result;
}

QutimTranslation::QutimTranslation( const QString &dir ) : m_dir(dir), m_translators(0), m_locale(0), m_lang(0)
{
}

QutimTranslation::~QutimTranslation()
{
	deinit();
	if(m_lang)
		delete m_lang;
	if(m_locale)
		delete m_locale;
}

void QutimTranslation::init()
{
	if(m_translators)
		deinit();
	static const QStringList filter = QStringList() << "*.qm";
	QFileInfoList files = QDir(m_dir).entryInfoList(filter, QDir::Files);
	foreach(const QFileInfo &info, files)
	{
		qDebug() << info.absoluteFilePath();
		QTranslator *translator = new QTranslator(this);
		translator->load(info.absoluteFilePath());
		*m_translators << translator;
	}
}

void QutimTranslation::deinit()
{
	if(!m_translators)
		return;
	QList<QTranslator *> translators = *m_translators;
	m_translators->clear();
	qDeleteAll(*translators);
}


QString QutimTranslation::translate( const char *context, const char *source_text, const char *comment = 0, int n = -1 ) const
{
	if(!m_translators)
		init();
	QString result;
	for(int i=0; i<m_translators->size(); i++)
	{
		result = m_translators->at(i)->translate(context, source_text, comment, n);
		if(!result.isEmpty())
			break;
	}
	return result;
}

QString QutimTranslation::lang()
{
	if(!m_lang)
		m_lang = new QString(QDir(m_dir).dirName());
	return *m_lang;
}

QLocale QutimTranslation::locale()
{
	if(!m_locale)
		m_locale = new QLocale(lang());
	return *m_locale;
}
