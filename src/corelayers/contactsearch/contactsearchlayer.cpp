#include "contactsearchlayer.h"
#include "libqutim/objectgenerator.h"
#include "libqutim/account.h"
#include "libqutim/protocol.h"
#include "libqutim/contact.h"
#include "libqutim/icon.h"
#include <QKeyEvent>
#include "src/modulemanagerimpl.h"

namespace Core
{

static Core::CoreModuleHelper<ContactSearch> contact_search_static(
		QT_TRANSLATE_NOOP("Plugin", "Contact search"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM implementation of contact search window")
);

void ContactModel::setRequest(const RequestPtr &request)
{
	beginResetModel();
	if (m_request)
		m_request->disconnect(this);
	m_request = request;
	if (m_request) {
		connect(m_request.data(), SIGNAL(contactAboutToBeAdded(int)), SLOT(contactAboutToBeAdded(int)));
		connect(m_request.data(), SIGNAL(contactAdded(int)), SLOT(contactAdded(int)));
	}
	endResetModel();
}

int ContactModel::rowCount(const QModelIndex &parent) const
{
	if (m_request)
		return m_request->contactCount();
	else
		return 0;
}

int ContactModel::columnCount(const QModelIndex &parent) const
{
	if (m_request)
		return m_request->columnCount();
	else
		return 0;
}

QVariant ContactModel::data(const QModelIndex &index, int role) const
{
	if (m_request)
		return m_request->data(index.row(), index.column(), role);
	else
		return QVariant();
}

QVariant ContactModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && m_request)
		return m_request->headerData(section, role);
	else
		return QVariant();
}

void ContactModel::contactAboutToBeAdded(int row)
{
	beginInsertRows(QModelIndex(), row, row);
}

void ContactModel::contactAdded(int row)
{
	endInsertRows();
}

ContactSearchForm::ContactSearchForm(QWidget *parent) :
	QWidget(parent), requestListUpdating(false)
{
	ui.setupUi(this);
	ui.splitter->setStretchFactor(1, 2);
	ui.updateServiceButton->setIcon(Icon("view-refresh"));
	ui.serviceBox->setVisible(false);
	ui.updateServiceButton->setVisible(false);
	//ui.contactsView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	ui.progressBar->setVisible(false);
	ui.contactsView->setModel(&m_contactModel);
	foreach(const ObjectGenerator *gen, moduleGenerators<ContactSearchFactory>()) {
		FactoryPtr factory(gen->generate<ContactSearchFactory>());
		m_factories << factory;
		connect(factory.data(), SIGNAL(requestListUpdated()), SLOT(updateRequestBox()));
	}
	connect(ui.searchButton, SIGNAL(clicked()), SLOT(startSearch()));
	connect(ui.cancelButton, SIGNAL(clicked()), SLOT(cancelSearch()));
	connect(ui.requestBox, SIGNAL(currentIndexChanged(int)), SLOT(updateRequest(int)));
	connect(ui.addContactButton, SIGNAL(clicked()), SLOT(addContact()));
	connect(ui.updateServiceButton, SIGNAL(clicked()), SLOT(updateService()));
	updateRequestBox();
}

void ContactSearchForm::updateRequestBox()
{
	requestListUpdating = true;
	RequestBoxItem currentItem = m_requestItems.value(ui.requestBox->currentIndex());
	m_requestItems.clear();
	ui.requestBox->clear();
	int i = 0;
	int newIndex = -1;
	foreach (const FactoryPtr &factory, m_factories) {
		foreach (const LocalizedString &request, factory->requestList()) {
			ui.requestBox->addItem(request);
			m_requestItems << RequestBoxItem(factory, request.original());
			if (newIndex == -1 && factory == currentItem.factory && request.original() == currentItem.name)
				newIndex = i;
			++i;
		}
	}
	if (newIndex == -1) {
		m_currentRequest = RequestPtr();
		clearFields();
	}
	ui.requestBox->setCurrentIndex(newIndex);
	requestListUpdating = false;
}

void ContactSearchForm::startSearch()
{
	if (!m_searchWidget)
		return;
	Q_ASSERT(m_currentRequest);
	AbstractDataWidget *dataWidget = qobject_cast<AbstractDataWidget*>(m_searchWidget.data());
	if (dataWidget) {
		setState(true);
		m_contactModel.beginResetModel();
		m_currentRequest->start(dataWidget->item());
		m_contactModel.endResetModel();
	}
}

void ContactSearchForm::updateRequest(int index)
{
	if (requestListUpdating)
		return;
	RequestBoxItem currentItem = m_requestItems.value(index);
	if (m_currentRequest)
		m_currentRequest->disconnect(this);
	m_currentRequest = RequestPtr();
	if (currentItem.factory)
		m_currentRequest = RequestPtr(currentItem.factory->request(currentItem.name));
	if (m_currentRequest.isNull()) {
		ui.searchButton->setEnabled(false);
		return;
	}
	ui.searchButton->setEnabled(true);
	connect(m_currentRequest.data(), SIGNAL(done(bool)), SLOT(done(bool)));
	connect(m_currentRequest.data(), SIGNAL(fieldsUpdated()), SLOT(updateFields()));
	connect(m_currentRequest.data(), SIGNAL(servicesUpdated()), SLOT(updateServiceBox()));
	m_contactModel.setRequest(m_currentRequest);
	updateFields();
	updateServiceBox();
}

void ContactSearchForm::updateFields()
{
	clearFields();
	m_searchWidget = AbstractDataForm::get(m_currentRequest->fields());
	if (m_searchWidget) {
		m_searchWidget->setParent(this);
		m_searchWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		ui.generalLayout->insertWidget(2, m_searchWidget.data());
	}
}

void ContactSearchForm::clearFields()
{
	if (m_searchWidget)
		m_searchWidget->deleteLater();
}

void ContactSearchForm::cancelSearch()
{
	Q_ASSERT(m_currentRequest);
	m_currentRequest->cancel();
	setState(false);
}

void ContactSearchForm::done(bool ok)
{
	Q_UNUSED(ok);
	setState(false);
}

void ContactSearchForm::addContact()
{
	if (m_contactModel.request()->contactCount() == 1) {
		addContact(0);
	} else {
		foreach (const QModelIndex &index, ui.contactsView->selectionModel()->selectedRows())
			addContact(index.row());
	}
}

void ContactSearchForm::addContact(int row)
{
	Contact *contact = m_contactModel.request()->contact(row);
	if (contact)
		contact->addToList();
}

void ContactSearchForm::updateService()
{
	Q_ASSERT(m_currentRequest);
	m_currentRequest->setService(ui.serviceBox->currentText());
}

void ContactSearchForm::updateServiceBox()
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

void ContactSearchForm::setState(bool search)
{
	ui.searchButton->setEnabled(!search);
	ui.cancelButton->setEnabled(search);
	ui.requestBox->setEnabled(!search);
	ui.progressBar->setVisible(search);
	if (m_searchWidget)
		m_searchWidget->setEnabled(!search);
}

bool ContactSearchForm::event(QEvent *e)
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

ContactSearch::ContactSearch()
{
	QObject *contactList = getService("ContactList");
	if (contactList) {
		static QScopedPointer<ActionGenerator> button(new ActionGenerator(Icon("edit-find-contact"),
										QT_TRANSLATE_NOOP("ContactSearch", "Search contact"),
										this, SLOT(show())));
		//QMetaObject::invokeMethod(contactList, "addButton", Q_ARG(ActionGenerator*, button.data()));
		MenuController *controller = qobject_cast<MenuController*>(contactList);
		if (controller)
			controller->addAction(button.data());
	}
}

void ContactSearch::show()
{
	if (searchContactForm) {
		searchContactForm->raise();
	} else {
		searchContactForm = new ContactSearchForm();
		centerizeWidget(searchContactForm);
		searchContactForm->show();
		searchContactForm->setAttribute(Qt::WA_DeleteOnClose, true);
	}
}

}
