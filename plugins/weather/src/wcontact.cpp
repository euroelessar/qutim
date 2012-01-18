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
#include "wcontact.h"
#include <qutim/servicemanager.h>

WContact::WContact(const QString &code, const QString &name, Account *account)
    : Contact (account)
{
	m_city = code;
	m_name = name;
	m_tags << QLatin1String("Weather");
	setMenuFlags(ShowSelfActions);

	m_status.setType(Status::Online);
	m_status.setIcon(QIcon(":/icons/weather.png"));
	emit statusChanged(m_status, Status(Status::Offline));

	addToList();
	QMetaObject::invokeMethod(ServiceManager::getByName("ContactList"),
	                          "addContact",
	                          Q_ARG(qutim_sdk_0_3::Contact*, this));

	m_wmanager = new WManager(m_city);
	connect(m_wmanager, SIGNAL(finished()), this, SLOT(finished()));
	m_forecast = false;
	m_forStatus = false;
}

WContact::~WContact()
{
	m_wmanager->deleteLater();
}


bool WContact::event(QEvent *ev)
{
	if (ev->type() == ToolTipEvent::eventType()) {
		ToolTipEvent *event = static_cast< ToolTipEvent * >(ev);
		QString ut = m_wmanager->getUnit(QLatin1String("ut"));
		if (!ut.isEmpty()) {
			event->addField(QT_TRANSLATE_NOOP("Weather", "Weather"), QString::fromLatin1("%1 °%2")
			                .arg(m_wmanager->getCC(QLatin1String("tmp")), ut));
		}

		return true;
	}

	return Contact::event(ev);
}

void WContact::update()
{
	m_forStatus = true;

	m_wmanager->update();
}

void WContact::updateStatus()
{
	if ((static_cast<WAccount *>(account()))->getShowStatusRow()) {
		update();
	} else {
		Status current = m_status;
		m_status.setText(QString());
		emit statusChanged(m_status, current);
	}
}

void WContact::finished()
{
//	m_menu->setDisabled(false);

	if (m_name.isEmpty())
		setNamev2(m_wmanager->getLoc(QLatin1String("dnam")));

	QString msgWeatherT = getFileData("weatherT.html");
	QString msgWeatherHtmlT = getFileData("weatherHtmlT.html");
	QString msgForecastT, msgForecastHtmlT;

	QHashIterator< QString, QString > it(*m_wmanager->getCC());
	while (it.hasNext())
	{
		it.next();

		msgWeatherT.replace("%" + it.key() + "%", it.value());
		msgWeatherHtmlT.replace("%" + it.key() + "%", it.value());
	}

	if (m_forecast)
	{
		QString tMsgForecastT, tMsgForecastHtmlT;

		msgForecastT = getFileData("forecastTitle.html");
		msgForecastHtmlT = getFileData("forecastHtmlTitle.html").arg(m_wmanager->getDayF(-1, "lsup"));

		for (int i = 0; i <= 4; i++)
		{
			QHash<QString, QString> h = *m_wmanager->getDayF(i);
			it = h;

			tMsgForecastT = getFileData("forecastT.html");
			tMsgForecastHtmlT = getFileData("forecastHtmlT.html");

			while (it.hasNext())
			{
				it.next();

				tMsgForecastT.replace("%" + it.key() + "%", it.value());
				tMsgForecastHtmlT.replace("%" + it.key() + "%", it.value());
			}

			msgForecastT += tMsgForecastT;
			msgForecastHtmlT += tMsgForecastHtmlT;
		}
	}
	else
	{
		Status previous = m_status;
		m_status.setIcon(QIcon(QString(":/icons/%1.png").arg(m_wmanager->getCC("icon"))));
		if (((WAccount *)account())->getShowStatusRow())
			m_status.setText(QString::fromUtf8("Weather: %1 °%2").arg(m_wmanager->getCC("tmp")).arg(m_wmanager->getUnit("ut")));
		emit statusChanged(m_status, previous);
	}

	it = *m_wmanager->getLoc();
	while (it.hasNext())
	{
		it.next();

		msgWeatherT.replace("%loc_" + it.key() + "%", it.value());
		msgWeatherHtmlT.replace("%loc_" + it.key() + "%", it.value());

		msgForecastT.replace("%loc_" + it.key() + "%", it.value());
		msgForecastHtmlT.replace("%loc_" + it.key() + "%", it.value());
	}

	it = *m_wmanager->getUnit();
	while (it.hasNext())
	{
		it.next();

		msgWeatherT.replace("%unit_" + it.key() + "%", it.value());
		msgWeatherHtmlT.replace("%unit_" + it.key() + "%", it.value());

		msgForecastT.replace("%unit_" + it.key() + "%", it.value());
		msgForecastHtmlT.replace("%unit_" + it.key() + "%", it.value());
	}

	Message message(m_forecast ? msgForecastT : msgWeatherT);
	message.setProperty("html", m_forecast ? msgForecastHtmlT : msgWeatherHtmlT); 
	message.setIncoming(1);
	message.setTime(QDateTime::currentDateTime());
	message.setChatUnit(this);

	if (!m_forStatus && false)
		ChatLayer::get(this, true)->appendMessage(message);

	m_forecast = false;
	m_forStatus = false;
}

void WContact::getWeather()
{
//	m_menu->setDisabled(true);

	m_wmanager->update();
}

void WContact::getForecast()
{
	m_forecast = true;
//	m_menu->setDisabled(true);

	m_wmanager->update(5);
}

bool WContact::sendMessage(const Message &)
{
	return false;
}

void WContact::setName(const QString &)
{
	emit nameChanged(m_name, m_name);
}

void WContact::setNamev2(const QString &name)
{
	QString previous = m_name;
	m_name = name;
	emit nameChanged(m_name, previous);
}

void WContact::setTags(const QStringList &)
{
	emit tagsChanged(m_tags, m_tags);
}

QString WContact::id() const
{
	return m_city;
}

QString WContact::name() const
{
	return m_name;
}

QString WContact::title() const
{
	return m_name;
}

QStringList WContact::tags() const
{
	return m_tags;
}

Status WContact::status() const
{
	return m_status;
}

bool WContact::isInList() const
{
	return m_inList;
}

void WContact::setInList(bool inList)
{
	m_inList = inList;

	emit inListChanged(m_inList);
}

QString WContact::avatar() const
{
	return ":/icons/weather_big.png";
}

QString WContact::getFileData(const QString &path)
{
	QFile file(((WAccount *)account())->getThemePath() + path);
	file.open(QFile::ReadOnly);
	return QString::fromUtf8(file.readAll()).remove("\n");
}

