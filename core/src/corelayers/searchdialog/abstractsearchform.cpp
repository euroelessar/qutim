/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "abstractsearchform.h"
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <QKeyEvent>
#include <qutim/itemdelegate.h>
#include <QComboBox>
#include <QAction>
#include <QPushButton>

namespace Core {

AbstractSearchForm::AbstractSearchForm(const QList<AbstractSearchFactory*> &factories,
									   const QString &title,
									   const QIcon &icon,
									   QWidget *parent) :
	QWidget(parent),
	m_resultModel(new ResultModel(this)),
	m_requestsModel(new RequestsListModel(factories, this))
{
	setWindowIcon(icon);
	setWindowTitle(title);
}

AbstractSearchForm::~AbstractSearchForm()
{
}

void AbstractSearchForm::setTitle(const QString &title, const QIcon &icon)
{
	if (!icon.isNull())
		setWindowIcon(icon);
	if (!title.isEmpty())
		setWindowTitle(title);
}

bool AbstractSearchForm::startSearch()
{
	if (!m_searchFieldsWidget)
		return false;
	Q_ASSERT(m_currentRequest);
	if (m_searchFieldsWidget) {
		m_resultModel->beginResetModel();
		m_currentRequest->start(m_searchFieldsWidget.data()->item());
		m_resultModel->endResetModel();
		m_done = false;
		return true;
	}
	return false;
}

bool AbstractSearchForm::cancelSearch()
{
	if (m_currentRequest && !m_done) {
		m_currentRequest->cancel();
		return true;
	}
	return false;
}

void AbstractSearchForm::setCurrentRequest(RequestPtr request)
{
	if (m_currentRequest)
		m_currentRequest->disconnect(this);
	m_currentRequest = request;
	if (!m_currentRequest) {
		if (m_searchFieldsWidget)
			m_searchFieldsWidget.data()->deleteLater();
		qDeleteAll(m_actions);
	} else {
		m_resultModel->setRequest(m_currentRequest);
		connect(m_currentRequest.data(), SIGNAL(done(bool)), SLOT(done(bool)));
	}
}

void AbstractSearchForm::setService(const QString &service)
{
	Q_ASSERT(m_currentRequest);
	m_currentRequest->setService(service);
}

void AbstractSearchForm::updateServiceBox(QComboBox *serviceBox, QPushButton *updateServiceButton)
{
	Q_ASSERT(m_currentRequest);
	QSet<QString> services = m_currentRequest->services();
	bool visible = !services.isEmpty();
	serviceBox->setVisible(visible);
	updateServiceButton->setVisible(visible);
	if (visible) {
		QString currentService = serviceBox->currentText();
		int currentIndex = -1;
		serviceBox->clear();
		int i = 0;
		foreach (const QString &service, services.toList()) {
			if (!service.isNull())
				serviceBox->addItem(service);
			if (currentIndex == -1 && service == currentService)
				currentIndex = i;
			++i;
		}
		serviceBox->setCurrentIndex(currentIndex);
		if (currentIndex == -1)
			m_currentRequest->setService(QString());
	} else {
		serviceBox->clear();
	}
}

void AbstractSearchForm::updateSearchFieldsWidget()
{
	if (m_searchFieldsWidget)
		m_searchFieldsWidget.data()->deleteLater();
	m_searchFieldsWidget = AbstractDataForm::get(currentRequest()->fields());
	if (m_searchFieldsWidget)
		searchFieldsWidget()->setParent(this);
	searchFieldsWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

QAction *AbstractSearchForm::actionAt(int i)
{
	Q_ASSERT(m_currentRequest);
	QIcon icon = m_currentRequest->actionData(i, Qt::DecorationRole).value<QIcon>();
	QVariant textVar = m_currentRequest->actionData(i, Qt::DisplayRole);
	QString text = textVar.canConvert<LocalizedString>() ? textVar.value<LocalizedString>().toString() : textVar.toString();
	QAction *button = new QAction(icon, text,this);
	m_actions << button;
	button->setProperty("actionIndex", i);
	return button;
}

void AbstractSearchForm::clearActionButtons()
{
	qDeleteAll(m_actions);
	m_actions.clear();
}

void AbstractSearchForm::actionButtonClicked(QAction *button, const QList<QModelIndex> &selected)
{
	if (!m_resultModel->request())
		return;
	int actionIndex = button->property("actionIndex").toInt();
	if (m_resultModel->request()->rowCount() == 1) {
		m_resultModel->request()->actionActivated(actionIndex, 0);
	} else {
		foreach (const QModelIndex &index, selected)
			m_resultModel->request()->actionActivated(actionIndex, index.row());
	}
}

void AbstractSearchForm::done(bool ok)
{
	Q_UNUSED(ok);
	m_done = true;
}

bool AbstractSearchForm::event(QEvent *e)
{
	/*if (e->type() == QEvent::KeyPress) {
  QKeyEvent *event = static_cast<QKeyEvent*>(e);
  if (event->key() == Qt::Key_Return) {
   if (ui.searchButton->isEnabled())
	startSearch();
   return true;
  }
 }*/
	return QWidget::event(e);
}

AbstractSearchFormFactory *AbstractSearchFormFactory::instance()
{
	return ServiceManager::getByName<AbstractSearchFormFactory*>("SearchForm");
}

} // namespace Core

