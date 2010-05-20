#include "contactinfo.h"
#include "src/modulemanagerimpl.h"
#include "libqutim/icon.h"
#include "libqutim/extensioninfo.h"
#include "libqutim/contact.h"
#include "libqutim/account.h"
#include "libqutim/dataforms.h"
#include <QDate>
#include <QDateEdit>
#include <QFileDialog>

namespace Core
{
static Core::CoreModuleHelper<ContactInfo> contact_info_static(
	QT_TRANSLATE_NOOP("Plugin", "Simple information window about a contact"),
	QT_TRANSLATE_NOOP("Plugin", "Default qutIM implementation of the information window")
);

MainWindow::MainWindow() :
	request(0)
{
	ui.setupUi(this);
	connect(ui.requestButton, SIGNAL(clicked()), SLOT(onRequestButton()));
	connect(ui.saveButton, SIGNAL(clicked()), SLOT(onSaveButton()));
	connect(ui.addButton, SIGNAL(clicked()), SLOT(onAddAvatar()));
	connect(ui.removeButton, SIGNAL(clicked()), SLOT(onRemoveAvatar()));
	ui.addButton->setIcon(Icon("list-add-avatar"));
	ui.removeButton->setIcon(Icon("list-remove-avatar"));
}

void MainWindow::setObject(QObject *obj, RequestType type)
{
	object = obj;
	readWrite = type == InfoRequestCheckSupportEvent::ReadWrite;
	InfoRequestEvent event;
	qApp->sendEvent(object, &event);
	if (event.request())
		setRequest(event.request());
}

void MainWindow::setRequest(InfoRequest *req)
{
	if (request && req != request)
		request->deleteLater();
	int curPage = ui.detailsStackedWidget->currentIndex();
	if (request != req) {
		request = req;
		connect(request, SIGNAL(stateChanged(InfoRequest::State)),
				SLOT(onRequestStateChanged(InfoRequest::State)));
	}
	ui.infoListWidget->clear();
	QWidget *w;
	while ((w = ui.detailsStackedWidget->widget(0)) != 0)
		delete w;
	Buddy *buddy = qobject_cast<Buddy*>(object);
	if (buddy) {
		setWindowTitle(QT_TRANSLATE_NOOP("ContactInfo", "About contact %1 <%2>")
						.toString()
						.arg(buddy->name())
						.arg(buddy->id()));
		m_avatar = buddy->avatar();
	} else {
		Account *account = qobject_cast<Account*>(object);
		if (account) {
			setWindowTitle(QT_TRANSLATE_NOOP("ContactInfo", "About account %1")
							.toString()
							.arg(account->name()));
		} else {
			setWindowTitle(QT_TRANSLATE_NOOP("ContactInfo", "About %1 <%2>")
							.toString()
							.arg(object->property("name").toString())
							.arg(object->property("id").toString()));
		}
		m_avatar = account->property("avatar").toString();
	}
	ui.saveButton->setVisible(readWrite);
	ui.addButton->setVisible(readWrite);
	ui.removeButton->setVisible(readWrite);
	if (m_avatar.isEmpty())
		onRemoveAvatar();
	else
		updateAvatar();
	addItems(request->item());
	if (curPage >= 0)
		ui.infoListWidget->setCurrentRow(curPage);
	InfoRequest::State state = request->state();
	if (state == InfoRequest::Done || state == InfoRequest::Cancel) {
		request->deleteLater(); request = 0;
	} else if (state == InfoRequest::Cache) {
		request->resend();
	}
}

void MainWindow::onRequestStateChanged(InfoRequest::State state)
{
	if (request != sender())
		return;
	Q_UNUSED(state);
	setRequest(request);
}

void MainWindow::onRequestButton()
{
	InfoRequestEvent event;
	qApp->sendEvent(object, &event);
	if (event.request())
		setRequest(event.request());
}

void MainWindow::onSaveButton()
{
	DataItem items;
	for (int i = 0; i < ui.detailsStackedWidget->count(); ++i) {
		AbstractDataForm *dataFrom = qobject_cast<AbstractDataForm*>(ui.detailsStackedWidget->widget(i));
		if (!dataFrom)
			continue;
		if (dataFrom->objectName() == "General") {
			foreach (const DataItem &item, dataFrom->item().subitems())
				items.addSubitem(item);
		} else {
			items.addSubitem(dataFrom->item());
		}
	}
	InfoItemUpdatedEvent event(items);
	qApp->sendEvent(object, &event);
	object->setProperty("avatar", m_avatar);
}

void MainWindow::onAddAvatar()
{
	QString avatar = QFileDialog::getOpenFileName(
			this,
			QT_TRANSLATE_NOOP("ContactInfo", "Open avatar"),
			QDir::homePath(),
			QT_TRANSLATE_NOOP("ContactInfo",
							  "Images (*.gif *.bmp *.jpg *.jpeg *.png);;"\
							  "All files (*.*)"));
	if (!avatar.isEmpty()) {
		m_avatar = avatar;
		updateAvatar();
	}
}

void MainWindow::onRemoveAvatar()
{
	m_avatar = ":/icons/qutim_64.png";
	updateAvatar();
}

void MainWindow::addItems(const DataItem &items)
{
	if (items.isNull() || !items.hasSubitems())
		return;
	// Summary
	QLabel *w = new QLabel(summary(items), ui.detailsStackedWidget);
	w->setWordWrap(true);
	w->setAlignment(Qt::AlignTop);
	w->setFrameShape(QFrame::Panel);
	w->setFrameShadow(QFrame::Sunken);
	w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	w->setTextInteractionFlags(Qt::LinksAccessibleByMouse |
							Qt::LinksAccessibleByKeyboard |
							Qt::TextSelectableByMouse |
							Qt::TextSelectableByKeyboard);
	ui.infoListWidget->addItem(QT_TRANSLATE_NOOP("ContactInfo", "Summary"));
	ui.detailsStackedWidget->addWidget(w);
	// Pages
	DataItem general;
	foreach (const DataItem &item, items.subitems()) {
		if (item.hasSubitems()) {
			QWidget *page = getPage(item);
			ui.infoListWidget->addItem(item.title());
			ui.detailsStackedWidget->addWidget(page);
		} else {
			general.addSubitem(item);
		}
	}
	if (!general.isNull()) {
		QWidget *page = getPage(general);
		ui.infoListWidget->insertItem(0, QT_TRANSLATE_NOOP("ContactInfo", "General"));
		ui.detailsStackedWidget->insertWidget(0, page);
	}
}

QWidget *MainWindow::getPage(DataItem item)
{
	if (!readWrite)
		item.setReadOnly(true);
	QWidget *data = AbstractDataForm::get(item);
	if (data) {
		data->setParent(ui.detailsStackedWidget);
		QFrame *frame = qobject_cast<QFrame*>(data);
		if (frame) {
			frame->setFrameShape(QFrame::Panel);
			frame->setFrameShadow(QFrame::Sunken);
		}
	}
	return data;
}

QString MainWindow::summary(const DataItem &items)
{
	QString text;
	bool first = true;
	foreach (const DataItem &item, items.subitems()) {
		if (item.property("additional", false) || item.property("notSet", false))
			continue;
		if (item.hasSubitems()) {
			text += summary(item);
		} else if (item.data().canConvert(QVariant::String)) {
			QString str = item.data().toString();
			if (str.isEmpty())
				continue;
			if (first) {
				text += QString("<b>[%1]:</b><br>").arg(items.title());
				first = false;
			}
			text += QString("<b>%1:</b>  ").arg(item.title());
			QVariant::Type type = item.data().type();
			if (type == QVariant::Date)
				text += item.data().toDate().toString(Qt::SystemLocaleLongDate);
			else if (type == QVariant::DateTime)
				text += item.data().toDateTime().toString(Qt::SystemLocaleLongDate);
			else if (type == QVariant::Bool)
				text += item.data().toBool() ?
						 QT_TRANSLATE_NOOP("ContactInfo", "yes") :
						 QT_TRANSLATE_NOOP("ContactInfo", "no");
			else
				text += str.replace(QRegExp("(\r\n|\n|\r)"), "<br>");
			text += "<br>";
		}
	}
	return text;
}

void MainWindow::updateAvatar()
{
	ui.pictureLabel->setPixmap(QPixmap(m_avatar).scaled(QSize(64, 64), Qt::KeepAspectRatio));
}

class InfoActionGenerator : public ActionGenerator
{
public:
	InfoActionGenerator(QObject *receiver) :
		ActionGenerator(Icon("dialog-information"), 0, receiver, SLOT(onShow()))
	{
		setPriority(90);
		setType(90);
	}
protected:
	virtual QObject *generateHelper() const
	{
		InfoRequestCheckSupportEvent event;
		qApp->sendEvent(controller(), &event);
		if (event.supportType() != InfoRequestCheckSupportEvent::NoSupport) {
			QAction *action = prepareAction(new QAction(0));
			if (event.supportType() == InfoRequestCheckSupportEvent::Read)
				action->setText(QT_TRANSLATE_NOOP("ContactInfo", "Show information"));
			else if (event.supportType() == InfoRequestCheckSupportEvent::ReadWrite)
				action->setText(QT_TRANSLATE_NOOP("ContactInfo", "Edit information"));
			return action;
		} else {
			return 0;
		}
	}
};

ContactInfo::ContactInfo()
{
	MenuController::addAction<Buddy>(new InfoActionGenerator(this));
	MenuController::addAction<Account>(new InfoActionGenerator(this));
}

void ContactInfo::show(QObject *object)
{
	InfoRequestCheckSupportEvent event;
	qApp->sendEvent(object, &event);
	RequestType type = event.supportType();
	if (type == InfoRequestCheckSupportEvent::NoSupport)
		return;
	if (!info) {
		info = new MainWindow();
		centerizeWidget(info);
		info->show();
		info->setAttribute(Qt::WA_DeleteOnClose, true);
	} else {
		info->raise();
	}
	info->setObject(object, type);
}

void ContactInfo::onShow()
{
	QAction *action = qobject_cast<QAction *>(sender());
	Q_ASSERT(action);
	show(action->data().value<MenuController*>());
}

}
