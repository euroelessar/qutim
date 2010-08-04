/****************************************************************************
 *  mobilesearchform.cpp
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

#include "mobilesearchform.h"
#include "itemdelegate.h"
#include "libqutim/icon.h"
#include "modulemanagerimpl.h"

namespace Core {

	static CoreModuleHelper<MobileSearchFormFactory> mobile_searchform_static(
			QT_TRANSLATE_NOOP("Plugin", "Mobile search form"),
			QT_TRANSLATE_NOOP("Plugin", "Search form for mobile phones")
			);

	MobileSearchForm::MobileSearchForm(const QList<AbstractSearchFactory*> &factories,
									   const QString &title,
									   const QIcon &icon,
									   QWidget *parent) :
	AbstractSearchForm(factories, title, icon, parent),
	m_negative_action(new QAction(this)),
	m_search_state(ReadyState)
	{
		ui.setupUi(this);
		setTitle(title, icon);		
		ui.updateServiceButton->setIcon(Icon("view-refresh"));
		ui.serviceBox->setVisible(false);
		ui.updateServiceButton->setVisible(false);
		ui.progressBar->setVisible(false);
		ui.resultView->setModel(resultModel());
		ui.resultView->setItemDelegate(new ItemDelegate(this));
		ui.resultView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
		ui.requestBox->setModel(requestsModel());

		m_negative_action->setSoftKeyRole(QAction::NegativeSoftKey);
		m_negative_action->setText(QT_TRANSLATE_NOOP("SearchForm","Search"));
		ui.actionBox->addAction(m_negative_action);

		connect(ui.requestBox, SIGNAL(currentIndexChanged(int)), SLOT(updateRequest(int)));
		connect(ui.updateServiceButton, SIGNAL(clicked()), SLOT(updateService()));
		connect(m_negative_action,SIGNAL(triggered()),SLOT(onNegativeActionTriggered()));
		if (requestsModel()->rowCount() > 0)
			updateRequest(0);
	}

	void MobileSearchForm::startSearch()
	{
		if (AbstractSearchForm::startSearch()) {
			setState(SearchingState);
		}
	}

	void MobileSearchForm::cancelSearch()
	{
		if (AbstractSearchForm::cancelSearch()) {
			setState(ReadyState);
		}
	}

	void MobileSearchForm::updateRequest(int row)
	{
		setCurrentRequest(requestsModel()->request(row));
		if (currentRequest()) {
			connect(currentRequest().data(), SIGNAL(done(bool)), SLOT(done(bool)));
			connect(currentRequest().data(), SIGNAL(fieldsUpdated()), SLOT(updateFields()));
			connect(currentRequest().data(), SIGNAL(servicesUpdated()), SLOT(updateServiceBox()));
			connect(currentRequest().data(), SIGNAL(actionsUpdated()), SLOT(updateActionButtons()));
			connect(resultModel(), SIGNAL(rowAdded(int)), ui.resultView, SLOT(resizeRowToContents(int)));
			for (int i = 1, c = currentRequest()->columnCount(); i < c; ++i)
				ui.resultView->hideColumn(i);
			updateFields();
			updateServiceBox();
			updateActionButtons();
		} else {
			if (searchFieldsWidget())
				searchFieldsWidget()->deleteLater();
			clearActionButtons();
		}
	}

	void MobileSearchForm::updateService()
	{
		setService(ui.serviceBox->currentText());
	}

	void MobileSearchForm::done(bool ok)
	{
		Q_UNUSED(ok);
		setState(DoneState);
	}

	void MobileSearchForm::updateFields()
	{
		updateSearchFieldsWidget();
		if (searchFieldsWidget())
			ui.generalLayout->insertWidget(2, searchFieldsWidget());
	}

	void MobileSearchForm::updateServiceBox()
	{
		AbstractSearchForm::updateServiceBox(ui.serviceBox, ui.updateServiceButton);
	}

	void MobileSearchForm::updateActionButtons()
	{
		Q_ASSERT(currentRequest());
		clearActionButtons();
		for (int i = 0, c = currentRequest()->actionCount(); i < c; ++i) {
			QAction *button = actionAt(i);
			m_actions.append(button);
			connect(button, SIGNAL(triggered()), SLOT(actionButtonClicked()));
		}
	}

	void MobileSearchForm::actionButtonClicked()
	{
		Q_ASSERT(qobject_cast<QAction*>(sender()));
		AbstractSearchForm::actionButtonClicked(reinterpret_cast<QAction*>(sender()),
												ui.resultView->selectionModel()->selectedRows());
	}

	void MobileSearchForm::setState(SearchState search)
	{
		m_search_state = search;
		if (search == ReadyState) {
			m_negative_action->setText(QT_TRANSLATE_NOOP("SearchForm","Search"));
			ui.actionBox->removeActions(m_actions);
			ui.stackedWidget->setCurrentIndex(0);
		}
		else {
			m_negative_action->setText(QT_TRANSLATE_NOOP("SearchForm","Back"));
			ui.actionBox->addActions(m_actions);
			ui.stackedWidget->setCurrentIndex(1);
		}

		ui.requestBox->setEnabled(search == ReadyState);
		ui.progressBar->setVisible(search == SearchingState);
		if (searchFieldsWidget())
			searchFieldsWidget()->setEnabled(search != SearchingState);

	}

	void MobileSearchForm::onNegativeActionTriggered()
	{
		if (m_search_state != ReadyState)
			cancelSearch();
		else
			startSearch();
	}

	AbstractSearchForm *MobileSearchFormFactory::createForm(const QList<AbstractSearchFactory*> &factories,
															const QString &title,
															const QIcon &icon,
															QWidget *parent)
	{
		return new MobileSearchForm(factories, title, icon, parent);
	}

}
