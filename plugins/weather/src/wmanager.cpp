/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nikita Belov <null@deltaz.org>
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

#include "wmanager.h"

WManager::WManager()
{
	init();
}

WManager::WManager(QString cityid, QString unit)
	: m_cityid (cityid), m_unit(unit)
{
	init();
}

WManager::~WManager()
{
	delete m_netman;
}

void WManager::init()
{
	m_netman = new QNetworkAccessManager();
	connect(m_netman, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
}

void WManager::update(int dayf)
{
	QString address = "http://xoap.weather.com/weather/local/%1?cc=*&prod=xoap&par=1085658115&key=b3fcda23f931ed87&unit=%2&dayf=%3";
	m_netman->get(QNetworkRequest(QUrl(address.arg(m_cityid).arg(m_unit).arg(dayf))));
}

void WManager::fillData(QDomNodeList elements, QHash< QString, QString > &hash, QString prefix)
{
	if (!prefix.isEmpty())
		prefix.append("_");

	for (int i = 0, size = elements.size(); i < size ; i++)
	{
		QDomElement element(elements.at(i).toElement());
		QDomNodeList celements = element.childNodes();

		if (celements.size() > 1)
			for (int j = 0, csize = celements.size(); j < csize ; j++)
			{
				QDomElement celement(celements.at(j).toElement());
				hash.insert(prefix + element.tagName() + "_" + celement.tagName(), celement.text());
			}
		else
			hash.insert(prefix + element.tagName(), element.text());
	}
}

void WManager::finished(QNetworkReply *reply)
{
	QDomDocument data;
	reply->deleteLater();
	if (!data.setContent(reply->readAll()))
		return;

	QDomElement content(data.documentElement());
	QDomElement head(content.firstChildElement("head"));
	QDomElement loc(content.firstChildElement("loc"));
	QDomElement cc(content.firstChildElement("cc"));
	QDomElement dayf(content.firstChildElement("dayf"));

	if (!head.isNull())
	{
		m_units.clear();

		QDomNodeList elements = head.childNodes();
		for (int i = 0, size = elements.size(); i < size ; i++)
		{
			QDomElement element(elements.at(i).toElement());
			m_units.insert(element.tagName(), element.text());
		}
	}

	if (!loc.isNull())
	{
		m_loc.clear();

		QDomNodeList elements = loc.childNodes();
		for (int i = 0, size = elements.size(); i < size ; i++)
		{
			QDomElement element(elements.at(i).toElement());
			m_loc.insert(element.tagName(), element.text());
		}
	}

	if (!cc.isNull())
	{
		m_cc.clear();

		fillData(cc.childNodes(), m_cc);
	}

	if (!dayf.isNull())
	{
		m_dayf.clear();

		QDomNodeList elements = dayf.childNodes();

		for (int i = 0, size = elements.size(); i < size ; i++)
		{
			QDomElement element(elements.at(i).toElement());
			QHash < QString, QString > *hash = new QHash < QString, QString >;

			int d = (element.hasAttribute("d") ? element.attribute("d").toInt() : -1);

			if (element.hasAttribute("t"))
				hash->insert("t", element.attribute("t"));
			if (element.hasAttribute("dt"))
				hash->insert("dt", element.attribute("dt"));

			QDomNodeList celements = element.childNodes();

			if (celements.size() > 1)
				for (int j = 0, size = celements.size(); j < size ; j++)
				{
					QDomElement celement(celements.at(j).toElement());
					QDomNodeList ccelements = celement.childNodes();

					if (ccelements.size() > 1)
						fillData(ccelements, *hash, celement.attribute("p"));
					else
						hash->insert(celement.tagName(), celement.text());
				}
			else
				hash->insert(element.tagName(), element.text());

			if (m_dayf.contains(d))
				delete m_dayf.value(d);
			m_dayf.insert(d, hash);
		}
	}

	emit finished();
}

const QHash< QString, QString > *WManager::getUnit()
{
	return &m_units;
}

const QHash< QString, QString > *WManager::getLoc()
{
	return &m_loc;
}

const QHash< QString, QString > *WManager::getCC()
{
	return &m_cc;
}

const QHash< QString, QString > *WManager::getDayF(int day)
{
	return m_dayf.value(day);
}

QString WManager::getUnit(QString key)
{
	return m_units.value(key);
}

QString WManager::getLoc(QString key)
{
	return m_loc.value(key);
}

QString WManager::getCC(QString key)
{
	return m_cc.value(key);
}

QString WManager::getDayF(int day, QString key)
{
	return m_dayf.value(day)->value(key);
}

struct WLang {
	const char *locale;
	int id;
};

static bool isCStrLessThen(const WLang &a, const WLang &b)
{
	return qstricmp(a.locale, b.locale) < 0;
}

QString WManager::currentLangId()
{
	static WLang langs[] = {
		{ "ar", 26 },
		{ "cs", 19 },
		{ "da", 4 },
		{ "de", 9 },
		{ "el", 27 },
		{ "en", 1 },
		{ "es", 2 },
		{ "fi", 11 },
		{ "fr", 3 },
		{ "hu", 20 },
		{ "it", 8 },
		{ "ja", 29 },
		{ "ko", 30 },
		{ "nl", 6 },
		{ "no", 7 },
		{ "pl", 21 },
		{ "pt_BR", 5 },
		{ "pt_PT", 5 },
		{ "ro", 18 },
		{ "ru", 25 },
		{ "sk", 17 },
		{ "sv", 10 },
		{ "tr", 31 },
		{ "zh_CN", 13 },
		{ "zh_TW", 12 }
	};
	int count = sizeof(langs) / sizeof(langs[0]);
	QByteArray name = QLocale().name().toLatin1();
	WLang noop = { name.constData(), 0 };
	WLang *lang = qBinaryFind(langs, langs + count, noop, isCStrLessThen);
	if (lang == langs + count && name.contains('_')) {
		name.truncate(name.indexOf('_'));
		noop.locale = name.constData();
		lang = qBinaryFind(langs, langs + count, noop, isCStrLessThen);
	}
	if (lang == langs + count)
		return QString();
	return QString::number(lang->id);
}

