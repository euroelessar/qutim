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

#include "waccount.h"
#include "wprotocol.h"
#include <qutim/thememanager.h>
#include <QTextDocument>
#include <QStringBuilder>
#include <QDebug>

WAccount::WAccount(WProtocol *protocol) : Account(QLatin1String("Weather"), protocol)
{
	m_settings = new GeneralSettingsItem<WSettings>(Settings::Plugin, QIcon(":/icons/weather.png"),
	                                                QT_TRANSLATE_NOOP("Weather", "Weather"));
	m_settings->connect(SIGNAL(saved()), this, SLOT(loadSettings()));
	Settings::registerItem(m_settings);
	connect(&m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(onNetworkReply(QNetworkReply*)));
	loadSettings();
}

WAccount::~WAccount()
{
	Settings::removeItem(m_settings);
	delete m_settings;
	foreach (WContact *v, m_contacts)
		v->deleteLater();
}

ChatUnit *WAccount::getUnitForSession(ChatUnit *unit)
{
	return unit;
}

ChatUnit *WAccount::getUnit(const QString &unitId, bool create)
{
	Q_UNUSED(create);
	return m_contacts.value(unitId);
}

QString WAccount::name() const
{
	return QLatin1String("You");
}

QString WAccount::getThemePath()
{
	return m_themePath;
}

bool WAccount::getShowStatusRow()
{
	return m_showStatusRow;
}

void WAccount::doConnectToServer()
{
}

void WAccount::doDisconnectFromServer()
{
}

void WAccount::doStatusChange(const Status &status)
{
	Q_UNUSED(status);
}

void WAccount::update(WContact *contact, bool needMessage)
{
	QUrl url(QLatin1String("http://forecastfox3.accuweather.com/adcbin/forecastfox3/current-conditions.asp"));
	QUrlQuery q;
	q.addQueryItem(QLatin1String("location"), contact->id());
	q.addQueryItem(QLatin1String("metric"), QString::number(1));
	QString langId = WManager::currentLangId();
	if (!langId.isEmpty())
		q.addQueryItem(QLatin1String("langid"), langId);
	url.setQuery(q);
	QNetworkRequest request(url);
	request.setOriginatingObject(contact);
	QNetworkReply *reply = m_manager.get(request);
	reply->setProperty("needMessage", needMessage);
}

void WAccount::getForecast(WContact *contact)
{
	QUrlQuery url(QLatin1String("http://forecastfox3.accuweather.com/adcbin/forecastfox3/forecast-data.asp"));
	url.addQueryItem(QLatin1String("location"), contact->id());
	url.addQueryItem(QLatin1String("metric"), QString::number(1));
	QString langId = WManager::currentLangId();
	if (!langId.isEmpty())
		url.addQueryItem(QLatin1String("langid"), langId);
	QNetworkRequest request(QUrl::fromEncoded(url.query(QUrl::FullyEncoded).toUtf8()));
	request.setOriginatingObject(contact);
	QNetworkReply *reply = m_manager.get(request);
	reply->setProperty("needMessage", true);
}

void WAccount::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_timer.timerId()) {
		foreach (WContact *contact, m_contacts)
			update(contact, false);
		return;
	}
	return Account::timerEvent(event);
}

void WAccount::loadSettings()
{
	Config config = Config(QLatin1String("weather"));
	config.beginGroup(QLatin1String("main"));

	int minutesInterval = config.value(QLatin1String("interval"), 25);
	m_timer.start(minutesInterval * 60 * 1000, this);

	m_showStatusRow = config.value(QLatin1String("showStatus"), true);
	QString themeName = config.value(QLatin1String("themeName"), QLatin1String("default"));
	m_themePath = ThemeManager::path(QLatin1String("weatherthemes"), themeName);
	if (m_themePath.isEmpty() && themeName != QLatin1String("default"))
		m_themePath = ThemeManager::path(QLatin1String("weatherthemes"), QLatin1String("default"));
	m_themePath += QLatin1Char('/');

	loadContacts();
}

void WAccount::onNetworkReply(QNetworkReply *reply)
{
	reply->deleteLater();
	WContact *contact = qobject_cast<WContact*>(reply->request().originatingObject());
	if (!contact)
		return;
	
	QDomDocument doc;
	if (!doc.setContent(reply->readAll()))
		return;
	bool needMessage = reply->property("needMessage").toBool();
	QDomElement rootElement = doc.documentElement();
	
	QDomElement units = rootElement.namedItem(QLatin1String("units")).toElement();
	QString tempUnit = units.namedItem(QLatin1String("temp")).toElement().text();
	
	QDomElement local = rootElement.namedItem(QLatin1String("local")).toElement();
	QString cityName = local.namedItem(QLatin1String("city")).toElement().text();
	contact->setNameInternal(cityName);
	
	QDomElement current = rootElement.namedItem(QLatin1String("current")).toElement();
	if (!current.isNull()) {
		QString temperature = current.namedItem(QLatin1String("temperature")).toElement().text();
		QString iconId = current.namedItem(QLatin1String("weathericon")).toElement().text();
		QString text = tr("Weather: %1°%2").arg(temperature, tempUnit);
		contact->setStatusInternal(iconId, text);
		if (needMessage) {
			QString text = loadResourceFile(QLatin1String("Current.txt"));
			QString html = loadResourceFile(QLatin1String("Current.html"));
			
			fillStrings(text, html, units, QLatin1String("%units"));
			fillStrings(text, html, current, QLatin1String("%"));
			
			Message message(text);
			message.setHtml(html);
			message.setChatUnit(contact);
			message.setIncoming(true);
			message.setTime(QDateTime::currentDateTime());
			ChatLayer::get(contact, true)->appendMessage(message);
		}
	}
	
	QDomElement forecast = rootElement.namedItem(QLatin1String("forecast")).toElement();
	if (needMessage && !forecast.isNull()) {
		QString textResult = loadResourceFile(QLatin1String("ForecastTitle.txt"));
		QString htmlResult = loadResourceFile(QLatin1String("ForecastTitle.html"));
		QString textBody = loadResourceFile(QLatin1String("ForecastDay.txt"));
		QString htmlBody = loadResourceFile(QLatin1String("ForecastDay.html"));
		
		fillStrings(textBody, htmlBody, units, QLatin1String("%units"));
		
		QDomNodeList days = forecast.elementsByTagName(QLatin1String("day"));
		for (int i = 0; i < days.count(); ++i) {
			if (!days.at(i).isElement())
				continue;
			QDomElement day = days.at(i).toElement();
			QString text = textBody;
			QString html = htmlBody;
			fillStrings(text, html, day, QLatin1String("%"));
			textResult += text;
			htmlResult += html;
		}
		
		Message message(textResult);
		message.setHtml(htmlResult);
		message.setChatUnit(contact);
		message.setIncoming(true);
		message.setTime(QDateTime::currentDateTime());
		ChatLayer::get(contact, true)->appendMessage(message);
	}
}

void WAccount::fillStrings(QString &text, QString &html, const QDomElement &element, const QString &prefix)
{
	QString key = prefix + QLatin1Char('%');
	if (key != QLatin1String("%%")) {
		text.replace(key, element.text());
		html.replace(key, element.text().toHtmlEscaped());
		qDebug() << key;
	}
	key.chop(1);
	
	QDomNamedNodeMap attributes = element.attributes();
	for (int i = 0; i < attributes.count(); ++i) {
		QDomNode attribute = attributes.item(i);
		QString attributeKey = key
		        % QLatin1Char('/')
		        % attribute.nodeName()
		        % QLatin1Char('%');
		qDebug() << attributeKey;
		text.replace(attributeKey, attribute.nodeValue());
		html.replace(attributeKey, attribute.nodeValue().toHtmlEscaped());
	}
	
	if (!key.endsWith(QLatin1Char('%')))
		key += QLatin1Char('/');
	QDomNodeList elementChildren = element.childNodes();
	for (int i = 0; i < elementChildren.count(); ++i) {
		QDomNode node = elementChildren.at(i);
		if (node.isElement())
			fillStrings(text, html, node.toElement(), key + node.nodeName());
	}
}

QString WAccount::loadResourceFile(const QString &fileName)
{
	static QRegExp regexp(QLatin1String("%localized\\{(.*)\\}%"));
	if (!regexp.isMinimal())
		regexp.setMinimal(true);
	QFile file(m_themePath + fileName);
	file.open(QFile::ReadOnly);
	QString text = QString::fromUtf8(file.readAll());
	int pos = 0;
    while ((pos = regexp.indexIn(text, pos)) != -1) {
		QString translation = QCoreApplication::translate("Weather", regexp.cap(1).toUtf8());
		text.replace(pos, regexp.matchedLength(), translation);
        pos += translation.length();
    }
	return text;
}

void WAccount::loadContacts()
{
	Config config(QLatin1String("weather"));
	config.beginGroup(QLatin1String("main"));
	QHash<QString, WContact *> contacts;
	int count = config.beginArray(QLatin1String("contacts"));
	for (int i = 0; i < count; i++) {
		config.setArrayIndex(i);
		QString cityCode = config.value(QLatin1String("code"), QString());
		QString cityName = config.value(QLatin1String("name"), QString());
		WContact *cityContact = m_contacts.take(cityCode);
		if (!cityContact) {
			cityContact = new WContact(cityCode, cityName, this);
			update(cityContact, false);
		}
		contacts.insert(cityCode, cityContact);
	}

	qDeleteAll(m_contacts);
	m_contacts = contacts;
}
