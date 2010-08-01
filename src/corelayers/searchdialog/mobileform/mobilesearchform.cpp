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
		AbstractSearchForm(factories, title, icon, parent)
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
		connect(ui.searchButton, SIGNAL(clicked()), SLOT(startSearch()));
		connect(ui.cancelButton, SIGNAL(clicked()), SLOT(cancelSearch()));
		connect(ui.requestBox, SIGNAL(currentIndexChanged(int)), SLOT(updateRequest(int)));
		connect(ui.updateServiceButton, SIGNAL(clicked()), SLOT(updateService()));
		if (requestsModel()->rowCount() > 0)
			updateRequest(0);
	}

	void MobileSearchForm::startSearch()
	{
		if (AbstractSearchForm::startSearch()) {
			setState(true);
			ui.stackedWidget->setCurrentIndex(1);
		}
	}

	void MobileSearchForm::cancelSearch()
	{
		if (AbstractSearchForm::cancelSearch()) {
			setState(false);
			ui.stackedWidget->setCurrentIndex(0);
		}
	}

	void MobileSearchForm::updateRequest(int row)
	{
		setCurrentRequest(requestsModel()->request(row));
		if (currentRequest()) {
			ui.searchButton->setEnabled(true);
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
			ui.searchButton->setEnabled(false);
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
		setState(false);
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
			QPushButton *button = getActionButton(i);
			ui.actionLayout->addWidget(button);
			connect(button, SIGNAL(clicked()), SLOT(actionButtonClicked()));
		}
	}

	void MobileSearchForm::actionButtonClicked()
	{
		Q_ASSERT(qobject_cast<QPushButton*>(sender()));
		AbstractSearchForm::actionButtonClicked(reinterpret_cast<QPushButton*>(sender()),
										ui.resultView->selectionModel()->selectedRows());
	}

	void MobileSearchForm::setState(bool search)
	{
		ui.searchButton->setEnabled(!search);
		ui.requestBox->setEnabled(!search);
		ui.progressBar->setVisible(search);
		if (searchFieldsWidget())
			searchFieldsWidget()->setEnabled(!search);
	}

	AbstractSearchForm *MobileSearchFormFactory::createForm(const QList<AbstractSearchFactory*> &factories,
															const QString &title,
															const QIcon &icon,
															QWidget *parent)
	{
		return new MobileSearchForm(factories, title, icon, parent);
	}

}
