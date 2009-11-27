#ifndef KOPETEEMOTICONSPROVIDER_H
#define KOPETEEMOTICONSPROVIDER_H
#include "kopeteemoticonsbackend.h"

class KopeteEmoticonsProvider : public EmoticonsProvider
{
public:
	KopeteEmoticonsProvider(const QString& themeName);
    virtual bool addEmoticon(const QString& imgPath, const QStringList& codes);
    virtual bool removeEmoticon(const QStringList& codes);
    virtual bool saveTheme();
    virtual QString themeName() const;	
	void loadTheme(const QString &themePath);
	void loadTheme();
private:
	void getThemeName();
	QString m_theme_name;
	QString m_theme_path;
};

#endif // KOPETEEMOTICONSPROVIDER_H
