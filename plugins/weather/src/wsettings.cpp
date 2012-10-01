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

#include "wsettings.h"
#include "wmanager.h"

WSettings::WSettings()
{
	ui.setupUi(this);

	m_networkManager = new QNetworkAccessManager();
	connect(m_networkManager, SIGNAL(finished(QNetworkReply*)),
	        SLOT(searchFinished(QNetworkReply*)));

	QFocusEvent focusEvent(QEvent::FocusOut);
	eventFilter(ui.searchEdit, &focusEvent);
	ui.searchEdit->installEventFilter(this);

	lookForWidgetState(ui.intervalBox);
	lookForWidgetState(ui.showStatusBox);
	lookForWidgetState(ui.themeNameBox);
}

WSettings::~WSettings()
{
}

void WSettings::loadImpl()
{
	Config config(QLatin1String("weather"));
	config.beginGroup(QLatin1String("main"));
	ui.intervalBox->setValue(config.value(QLatin1String("interval"), 25));
	ui.showStatusBox->setChecked(config.value(QLatin1String("showStatus"), true));
	int index = ui.themeNameBox->findText(config.value(QLatin1String("themeName"), QString()));
	ui.themeNameBox->setCurrentIndex(qMax(0, index));
	int count = config.beginArray(QLatin1String("contacts"));
	for (int i = 0; i < count; i++) {
		config.setArrayIndex(i);
		QString cityCode = config.value(QLatin1String("code"), QString());
		QString cityName = config.value(QLatin1String("name"), QString());
		QString stateName = config.value(QLatin1String("state"), QString());
		WListItem *item = new WListItem(cityName, stateName, cityCode, ui.citiesList);
		connect(item, SIGNAL(buttonClicked()), this, SLOT(onRemoveButtonClicked()));
		m_items << item;
	}
}

void WSettings::saveImpl()
{
	Config config(QLatin1String("weather"));
	config.beginGroup(QLatin1String("main"));
	config.setValue("interval", ui.intervalBox->value());
	config.setValue("showStatus", ui.showStatusBox->isChecked());
	int count = config.beginArray(QLatin1String("contacts"));
	for (int i = 0; i < m_items.size(); i++) {
		config.setArrayIndex(i);
		WListItem *item = m_items.at(i);
		config.setValue(QLatin1String("code"), item->id());
		config.setValue(QLatin1String("name"), item->name());
		config.setValue(QLatin1String("state"), item->state());
	}
	for (int i = count - 1; i >= m_items.size(); --i)
		config.remove(i);
}

void WSettings::cancelImpl()
{
}

bool WSettings::eventFilter(QObject *object, QEvent *event)
{
	if (object == ui.searchEdit) {
		if (event->type() == QEvent::FocusIn) {
			if (!ui.searchEdit->styleSheet().isEmpty()) {
				ui.searchEdit->setStyleSheet(QString());
				ui.searchEdit->clearEditText();
			}
		} else if (event->type() == QEvent::FocusOut) {
			if (ui.searchEdit->currentText() == QString()) {
				ui.searchEdit->setStyleSheet(QLatin1String("color: rgb(130, 130, 130); font-style: italic;"));
				ui.searchEdit->setEditText(tr("Enter here city's name"));
			}
		}
	}

	return QObject::eventFilter(object, event);
}

void WSettings::onRemoveButtonClicked()
{
	WListItem *item = qobject_cast<WListItem*>(sender());
	Q_ASSERT(item);
	m_items.removeOne(item);
	delete item->item();
}

void WSettings::on_addButton_clicked()
{
	int index = ui.searchEdit->currentIndex();
	QString cityCode = ui.searchEdit->itemData(index, CodeRole).toString();
	if (cityCode.isEmpty())
		return;
	QString cityName = ui.searchEdit->itemData(index, CityRole).toString();
	QString stateName = ui.searchEdit->itemData(index, StateRole).toString();
	
	WListItem *item = new WListItem(cityName, stateName, cityCode, ui.citiesList);
	connect(item, SIGNAL(buttonClicked()), this, SLOT(onRemoveButtonClicked()));
	m_items << item;

	setModified(true);
}

void WSettings::on_searchButton_clicked()
{
	QUrl url(QLatin1String("http://forecastfox3.accuweather.com/adcbin/forecastfox3/city-find.asp"));
	QString langId = WManager::currentLangId();
	if (!langId.isEmpty())
		url.addQueryItem(QLatin1String("langid"), langId);
	url.addQueryItem(QLatin1String("location"), ui.searchEdit->currentText());
	m_networkManager->get(QNetworkRequest(url));
	ui.addButton->setEnabled(false);
}

void WSettings::on_chooseButton_clicked()
{
//	ui.themePath->setText(QFileDialog::getExistingDirectory(this, tr("Select theme's directory"), QDesktopServices::storageLocation(QDesktopServices::HomeLocation), QFileDialog::ShowDirsOnly));
}

void WSettings::searchFinished(QNetworkReply *reply)
{
	reply->deleteLater();
	ui.addButton->setEnabled(true);
	ui.searchEdit->clear();
	
	QDomDocument doc;
	if (!doc.setContent(reply->readAll()))
		return;
	QDomElement rootElement = doc.documentElement();
	
	QDomNodeList locations = rootElement.elementsByTagName(QLatin1String("location"));
	if (locations.isEmpty())
		ui.searchEdit->addItem(tr("Not found"));
	for (int i = 0; i < locations.count(); i++) {
		QDomNamedNodeMap attributes = locations.at(i).attributes();
		QString cityId = attributes.namedItem(QLatin1String("location")).nodeValue();
		QString cityName = attributes.namedItem(QLatin1String("city")).nodeValue();
		QString stateName = attributes.namedItem(QLatin1String("state")).nodeValue();
		QString cityFullName = cityName + ", " + stateName;
		int index = ui.searchEdit->count();
		ui.searchEdit->addItem(cityFullName);
		ui.searchEdit->setItemData(index, cityId, CodeRole);
		ui.searchEdit->setItemData(index, cityName, CityRole);
		ui.searchEdit->setItemData(index, stateName, StateRole);
	}
}


void WSettings::on_searchEdit_activated(int index)
{
	QVariant currentData = ui.searchEdit->itemData(index);
	if (!currentData.isValid())
		ui.searchButton->animateClick();
}
