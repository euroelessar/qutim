/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Evgeniy Degtyarev <degtep@gmail.com>
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
#ifndef EMOTICONSWRAPPER_H
#define EMOTICONSWRAPPER_H

#include <QObject>
#include <qutim/emoticons.h>
#include <QVariant>

namespace MeegoIntegration
{
class EmoticonsWrapper : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString currentThemeName READ currentThemeName WRITE setCurrentTheme NOTIFY currentThemeNameChanged)
	Q_PROPERTY(QStringList themes READ themes)
	Q_PROPERTY(QStringList currentEmoticons READ currentEmoticons NOTIFY currentEmoticonsChanged)
	Q_PROPERTY(bool isCurrentEmoticonsAvailable READ isCurrentEmoticonsAvailable NOTIFY availableChanged)
public:
	EmoticonsWrapper(QObject *parent = 0);
	~EmoticonsWrapper();

	void setCurrentTheme(const QString &themeName);
	QString currentThemeName();
	QStringList themes();
	QStringList currentEmoticons();
	bool isCurrentEmoticonsAvailable();
	Q_INVOKABLE QStringList emoticons(const QString &themeName);
	Q_INVOKABLE QString currentEmoticonString(const QString &emoticonPath);

private:
	QHash<QString, QStringList> m_emoticonsMap;

signals:
	void currentThemeNameChanged(const QString &themeName);
	void currentEmoticonsChanged(const QStringList &emoticons);
	void availableChanged(bool available);
};
}

#endif // EMOTICONSWRAPPER_H
