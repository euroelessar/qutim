/****************************************************************************
 *  kopeteemoticonsprovider.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef KOPETEEMOTICONSPROVIDER_H
#define KOPETEEMOTICONSPROVIDER_H
#include "kopeteemoticonsbackend.h"

class KopeteEmoticonsProvider : public EmoticonsProvider
{
public:
	KopeteEmoticonsProvider(const QString& themePath);
	KopeteEmoticonsProvider();
    virtual bool addEmoticon(const QString& imgPath, const QStringList& codes);
    virtual bool removeEmoticon(const QStringList& codes);
    virtual bool saveTheme();
    virtual QString themeName() const;
	void loadTheme(const QString &themePath);
	void loadTheme();
	void setThemePath(const QString& themePath);
private:
	void getThemeName();
	QString m_theme_name;
	QString m_theme_path;
};

#endif // KOPETEEMOTICONSPROVIDER_H
