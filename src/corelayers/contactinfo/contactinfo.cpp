#include "contactinfo.h"
#include "src/modulemanagerimpl.h"
#include "libqutim/icon.h"
#include "libqutim/extensioninfo.h"
#include "libqutim/contact.h"
#include "libqutim/account.h"
#include "readonlyinfolayout.h"
#include "editableinfolayout.h"
#include <QDate>
#include <QDateEdit>

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
	QString avatar;
	Buddy *buddy = qobject_cast<Buddy*>(object);
	if (buddy) {
		setWindowTitle(QT_TRANSLATE_NOOP("ContactInfo", "About contact %1 <%2>")
						.toString()
						.arg(buddy->name())
						.arg(buddy->id()));
		avatar = buddy->avatar();
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
		avatar = account->property("avatar").toString();
	}
	ui.saveButton->setVisible(readWrite);
	ui.addButton->setVisible(readWrite);
	ui.removeButton->setVisible(readWrite);
	if (avatar.isEmpty())
		avatar = ":/icons/qutim_64.png";
	ui.pictureLabel->setPixmap(QPixmap(avatar).scaled(QSize(64, 64), Qt::KeepAspectRatio));
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
	InfoItem items;
	for (int i = 0; i < ui.detailsStackedWidget->count(); ++i) {
		EditableInfoLayout *layout = qobject_cast<EditableInfoLayout*>(
				ui.detailsStackedWidget->widget(i)->layout());
		if (!layout)
			continue;
		if (layout->objectName() == "General") {
			foreach (const InfoItem &item, layout->item().subitems())
				items.addSubitem(item);
		} else {
			items.addSubitem(layout->item());
		}
	}
	InfoItemUpdatedEvent event(items);
	qApp->sendEvent(object, &event);
}

// TODO: maybe move the function to InfoItem class?
void MainWindow::dump(const InfoItem &items, int ident)
{
	if (!items.hasSubitems())
		return;
	QString space;
	for (int i = 0; i < ident; ++i)
		space += "    ";
	foreach (const InfoItem &item, items.subitems()) {
		qDebug() << space.toLatin1().data() << item.name()
				<< item.title() << item.data();
		if (item.hasSubitems())
			dump(item, ident+1);
	}
}

void MainWindow::addItems(const InfoItem &items)
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
	AbstractInfoLayout *general = 0;
	foreach (const InfoItem &item, items.subitems()) {
		if (item.hasSubitems()) {
			QFrame *w = new QFrame(ui.detailsStackedWidget);

			w->setFrameShape(QFrame::Panel);
			w->setFrameShadow(QFrame::Sunken);
			AbstractInfoLayout *group;
			if (!readWrite)
				group = new ReadOnlyInfoLayout(w);
			else
				group = new EditableInfoLayout(w);
			group->setObjectName(item.name());
			group->addItems(item.subitems());
			group->addSpacer();
			ui.infoListWidget->addItem(item.name());
			ui.detailsStackedWidget->addWidget(w);
		} else {
			if (!general) {
				QFrame *w = new QFrame(ui.detailsStackedWidget);
				w->setFrameShape(QFrame::Panel);
				w->setFrameShadow(QFrame::Sunken);
				if (!readWrite)
					general = new ReadOnlyInfoLayout(w);
				else
					general = new EditableInfoLayout(w);
				general->setObjectName("General");
				ui.infoListWidget->addItem(QT_TRANSLATE_NOOP("ContactInfo", "General"));
				ui.detailsStackedWidget->addWidget(w);
			}
			general->addItem(item);
		}
	}
	if (general)
		general->addSpacer();
}

QString MainWindow::summary(const InfoItem &items)
{
	QString text;
	bool first = true;
	foreach (const InfoItem &item, items.subitems()) {
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
