/****************************************************************************
 * searchform.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "searchform.h"
#include "libqutim/icon.h"
#include <QKeyEvent>
#include "3rdparty/itemdelegate/itemdelegate.h"

namespace Core {

SearchForm::SearchForm(QMetaObject *factory, const QString &title, const QIcon &icon, QWidget *parent) :
	QWidget(parent)
{
	ui.setupUi(this);
	if (!icon.isNull())
		setWindowIcon(icon);
	if (!title.isEmpty())
		setWindowTitle(title);
	ui.updateServiceButton->setIcon(Icon("view-refresh"));
	ui.serviceBox->setVisible(false);
	ui.updateServiceButton->setVisible(false);
	ui.progressBar->setVisible(false);
	ui.resultView->setModel(&m_resultModel);
	ui.resultView->setItemDelegate(new ItemDelegate(this));
	ui.resultView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	m_requestsModel = new RequestsListModel(factory);
	ui.requestBox->setModel(m_requestsModel);
	connect(ui.searchButton, SIGNAL(clicked()), SLOT(startSearch()));
	connect(ui.cancelButton, SIGNAL(clicked()), SLOT(cancelSearch()));
	connect(ui.requestBox, SIGNAL(currentIndexChanged(int)),
			SLOT(updateRequest(int)));
	connect(ui.updateServiceButton, SIGNAL(clicked()), SLOT(updateService()));
}

SearchForm::~SearchForm()
{
	delete m_requestsModel;
}

void SearchForm::startSearch()
{
	if (!m_searchWidget)
		return;
	Q_ASSERT(m_currentRequest);
	AbstractDataWidget *dataWidget = qobject_cast<AbstractDataWidget*>(m_searchWidget.data());
	if (dataWidget) {
		setState(true);
		m_resultModel.beginResetModel();
		m_currentRequest->start(dataWidget->item());
		m_resultModel.endResetModel();
		ui.stackedWidget->setCurrentIndex(1);
		m_done = false;
	}
}

void SearchForm::updateRequest(int row)
{
	if (m_currentRequest)
		m_currentRequest->disconnect(this);
	m_currentRequest = m_requestsModel->request(row);
	if (!m_currentRequest) {
		ui.searchButton->setEnabled(false);
		if (m_searchWidget)
			m_searchWidget->deleteLater();
		qDeleteAllLater(m_actionButtons);
		return;
	}
	ui.searchButton->setEnabled(true);
	connect(m_currentRequest.data(), SIGNAL(done(bool)), SLOT(done(bool)));
	connect(m_currentRequest.data(), SIGNAL(fieldsUpdated()), SLOT(updateFields()));
	connect(m_currentRequest.data(), SIGNAL(servicesUpdated()), SLOT(updateServiceBox()));
	connect(m_currentRequest.data(), SIGNAL(actionsUpdated()), SLOT(updateActionButtons()));
	connect(&m_resultModel, SIGNAL(rowAdded(int)),
			ui.resultView, SLOT(resizeRowToContents(int)));
	m_resultModel.setRequest(m_currentRequest);
	updateFields();
	updateServiceBox();
	updateActionButtons();
}

void SearchForm::updateFields()
{
	clearFields();
	m_searchWidget = AbstractDataForm::get(m_currentRequest->fields());
	if (m_searchWidget) {
		m_searchWidget->setParent(this);
		m_searchWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		ui.generalLayout->insertWidget(2, m_searchWidget.data());
	}
}

void SearchForm::clearFields()
{
	if (m_searchWidget)
		m_searchWidget->deleteLater();
}

void SearchForm::cancelSearch()
{
	if (m_currentRequest && !m_done)
		m_currentRequest->cancel();
	setState(false);
	ui.stackedWidget->setCurrentIndex(0);
}

void SearchForm::done(bool ok)
{
	Q_UNUSED(ok);
	setState(false);
	m_done = true;
}

void SearchForm::actionButtonClicked()
{
	Q_ASSERT(qobject_cast<QPushButton*>(sender()));
	if (!m_resultModel.request())
		return;
	int actionIndex = sender()->property("actionIndex").toInt();
	if (m_resultModel.request()->rowCount() == 1) {
		actionButtonClicked(actionIndex, 0);
	} else {
		foreach (const QModelIndex &index, ui.resultView->selectionModel()->selectedRows())
			actionButtonClicked(actionIndex, index.row());
	}
}

void SearchForm::actionButtonClicked(int actionIndex, int row)
{
	m_resultModel.request()->actionActivated(actionIndex, row);
}

void SearchForm::updateService()
{
	Q_ASSERT(m_currentRequest);
	m_currentRequest->setService(ui.serviceBox->currentText());
}

void SearchForm::updateServiceBox()
{
	Q_ASSERT(m_currentRequest);
	QSet<QString> services = m_currentRequest->services();
	bool visible = !services.isEmpty();
	ui.serviceBox->setVisible(visible);
	ui.updateServiceButton->setVisible(visible);
	if (visible) {
		QString currentService = ui.serviceBox->currentText();
		int currentIndex = -1;
		ui.serviceBox->clear();
		int i = 0;
		foreach (const QString &service, m_currentRequest->services().toList()) {
			if (!service.isNull())
				ui.requestBox->addItem(service);
			if (currentIndex == -1 && service == currentService)
				currentIndex = i;
			++i;
		}
		ui.requestBox->setCurrentIndex(currentIndex);
		if (currentIndex == -1)
			m_currentRequest->setService(QString());
	} else {
		ui.serviceBox->clear();
	}
}

void SearchForm::updateActionButtons()
{
	Q_ASSERT(m_currentRequest);
	qDeleteAllLater(m_actionButtons);
	for (int i = 0, c = m_currentRequest->actionCount(); i < c; ++i) {
		QIcon icon = m_currentRequest->actionData(i, Qt::DecorationRole).value<QIcon>();
		QVariant textVar = m_currentRequest->actionData(i, Qt::DisplayRole);
		QString text = textVar.canConvert<LocalizedString>() ? textVar.value<LocalizedString>().toString() : textVar.toString();
		QPushButton *button = new QPushButton(icon, text);
		m_actionButtons << button;
		button->setProperty("actionIndex", i);
		connect(button, SIGNAL(clicked()), SLOT(actionButtonClicked()));
		ui.actionLayout->addWidget(button);
	}
}

void SearchForm::setState(bool search)
{
	ui.searchButton->setEnabled(!search);
	ui.requestBox->setEnabled(!search);
	ui.progressBar->setVisible(search);
	if (m_searchWidget)
		m_searchWidget->setEnabled(!search);
}

bool SearchForm::event(QEvent *e)
{
	if (e->type() == QEvent::KeyPress) {
		QKeyEvent *event = static_cast<QKeyEvent*>(e);
		if (event->key() == Qt::Key_Return) {
			if (ui.searchButton->isEnabled())
				startSearch();
			return true;
		}
	}
	return QWidget::event(e);
}

} // namespace Core
