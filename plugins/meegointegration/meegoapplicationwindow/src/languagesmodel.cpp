/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "languagesmodel.h"
#include <qutim/settingslayer.h>
#include <qutim/systeminfo.h>
#include <qutim/icon.h>
#include <qutim/configbase.h>
#include <qutim/thememanager.h>
#include <QtCore/QLocale>
#include <QtCore/QCoreApplication>
#include <QtCore/QResource>
#include <QtCore/QLibraryInfo>
#include <QtCore/QTranslator>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;
enum {
	LanguageRole = Qt::UserRole,
	LanguageNameRole,
	LanguageCountryRole
};

LanguagesModel::LanguagesModel(QObject *parent) :
    QAbstractListModel(parent)
{
	QHash<int, QByteArray> roleNames;
	roleNames.insert(LanguageRole, "lang");
	roleNames.insert(LanguageNameRole, "name");
	roleNames.insert(LanguageCountryRole, "country");
	roleNames.insert(Qt::DisplayRole, "fullName");
	setRoleNames(roleNames);

	QStringList langs = ThemeManager::list("languages");


	if (!langs.contains(QLatin1String("en_EN"))) {
		beginInsertRows(QModelIndex(),0,0);
		LanguageModel *langModel=new LanguageModel();
		langModel->language = "en_EN";
		langModel->languageName = "English";
		m_languages.append(langModel);
		endInsertRows();
	}

	beginInsertRows(QModelIndex(),0,langs.count());
	foreach (const QString &lang, langs) {

		QString languageName;
		QString languageCountry;
		QDir dir = ThemeManager::path("languages", lang);
		if (dir.exists("core.qm")) {
			QTranslator translator;
			translator.load(QLatin1String("core.qm"), dir.absolutePath());
			languageName = translator.translate("Localization", "<Language>", "Localized language name");
			if (!languageName.isEmpty())
			{
				languageCountry = translator.translate("Localization", "<Country>","Localized country name, empty if localization is country-independent");
			}
		}

		if (languageName.isEmpty())
		{
			QLocale locale(lang);
			languageName = QLocale::languageToString(locale.language());
			if (languageName.isEmpty())
				languageName = lang;
			if (locale.country() != QLocale::AnyCountry) {
				languageCountry = locale.country();
			}
		}

		LanguageModel *langModel = new LanguageModel();
		langModel->language = lang;
		langModel->languageCountry = languageCountry;
		langModel->languageName = languageName;
		m_languages.append(langModel);
	}
	endInsertRows();
}

int LanguagesModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_languages.count();
}

QVariant LanguagesModel::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() > m_languages.count())
		return QVariant();
	LanguageModel *langModel=m_languages[index.row()];
	switch (role) {
	case Qt::DisplayRole:
		if (!langModel->languageCountry.isEmpty())
			return langModel->languageName+"("+langModel->languageCountry+")";
		else
			return langModel->languageName;
	case LanguageRole:
		return langModel->language;
	case LanguageNameRole:
		return langModel->languageName;
	case LanguageCountryRole:
		return langModel->languageCountry;
	default:
		return QVariant();
	}
}

}

