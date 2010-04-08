#include "contactinfo.h"
#include "src/modulemanagerimpl.h"
#include "libqutim/icon.h"
#include "libqutim/extensioninfo.h"
#include "libqutim/contact.h"
#include <QDate>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QTextEdit>
#include "QGridLayout"
#include "QVBoxLayout"
#include <QDesktopWidget>
#include <QApplication>
#include <QHostAddress>
#include "libqutim/debug.h"

Q_DECLARE_METATYPE(QHostAddress);

namespace Core
{
static Core::CoreModuleHelper<ContactInfo> contact_info_static(
	QT_TRANSLATE_NOOP("Plugin", "Simple information window about a contact"),
	QT_TRANSLATE_NOOP("Plugin", "Default qutIM implementation of the information window")
);

InfoLayout::InfoLayout(QWidget *parent) :
	QGridLayout(parent), m_row(0)
{
}

InfoLayout::~InfoLayout()
{
}

void InfoLayout::addItems(const QList<InfoItem> &items)
{
	foreach (const InfoItem &item, items) {
		if (item.hasSubitems()) {
			QGroupBox *box = new QGroupBox(parentWidget());
			box->setTitle(item.title());
			box->setFlat(true);
			InfoLayout *group = new InfoLayout(box);
			group->addItems(item.subitems());
			addWidget(box, m_row++, 0, 1, 2);
		} else {
			addItem(item);
		}
	}
}

void InfoLayout::addItem(const InfoItem &item)
{
	if (!item.property("hideTitle", false)) {
		QLabel *title = new QLabel(item.title().toString() + ":", parentWidget());
		title->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		QFont font;
		font.setBold(true);
		title->setFont(font);
		title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		addWidget(title, m_row, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	}

	QVariant::Type type = item.data().type();
	if (type == QVariant::Date) {
		addLabel(item.data().toDate().toString(Qt::SystemLocaleLongDate), item.name());
		return;
	} else if (type == QVariant::DateTime) {
		addLabel(item.data().toDateTime().toString(Qt::SystemLocaleLongDate), item.name());
		return;
	} else if (type == QVariant::Image) {
		QLabel *d = new QLabel();
		d->setPixmap(QPixmap::fromImage(item.data().value<QImage>()));
		addDataWidget(d, item.name());
		return;
	} else if (type == QVariant::Pixmap) {
		QLabel *d = new QLabel();
		d->setPixmap(item.data().value<QPixmap>());
		addDataWidget(d, item.name());
		return;
	} else if (type == QVariant::Bool) {
		addLabel(item.data().toBool() ?
				 QT_TRANSLATE_NOOP("ContactInfo", "yes") :
				 QT_TRANSLATE_NOOP("ContactInfo", "no"),
				 item.name());
		return;
	} else if (item.data().canConvert<QHostAddress>()) {
		QHostAddress address = item.data().value<QHostAddress>();
		if (!address.isNull()) {
			addLabel(address.toString(), item.name());
			return;
		}
	} else if (item.data().canConvert(QVariant::StringList)) {
		QStringList list = item.data().toStringList();
		if (!list.isEmpty()) {
			foreach (const QString &i, list)
				addLabel(i, item.name());
			return;
		}
	}
	QString str = item.data().toString();
	bool enabled = !str.isEmpty();
	if (!enabled)
		str = QT_TRANSLATE_NOOP("ContactInfo", "the field is not set");
	addLabel(str, item.name())->setEnabled(enabled);
}

void InfoLayout::addSpacer()
{
	QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	QGridLayout::addItem(spacer, m_row++, 0);
}

void InfoLayout::addDataWidget(QWidget *widget, const QString &name)
{
	widget->setParent(parentWidget());
	widget->setObjectName(name);
	widget->setProperty("readOnly", true);
	widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	addWidget(widget, m_row++, 1, 1, 1, Qt::AlignLeft);
}

QLabel *InfoLayout::addLabel(const QString &data, const QString &name)
{
	QLabel *d = new QLabel(data, parentWidget());
	d->setTextInteractionFlags(Qt::LinksAccessibleByMouse |
							Qt::LinksAccessibleByKeyboard |
							Qt::TextSelectableByMouse |
							Qt::TextSelectableByKeyboard);
	addDataWidget(d, name);
	return d;
}

MainWindow::MainWindow() :
	request(0)
{
	ui.setupUi(this);
	connect(ui.requestButton, SIGNAL(clicked()), SLOT(onRequestButton()));
}

void MainWindow::setBuddy(Buddy *buddy, InfoRequest *req)
{
	m_buddy = buddy;
	if (request && req != request)
		request->deleteLater();
	int curPage = ui.detailsStackedWidget->currentIndex();
	request = req;
	ui.infoListWidget->clear();
	QWidget *w;
	while ((w = ui.detailsStackedWidget->widget(0)) != 0)
		delete w;
	setWindowTitle(QT_TRANSLATE_NOOP("ContactInfo", "About contact %1 <%2>")
					.toString()
					.arg(buddy->name())
					.arg(buddy->id()));
	ui.saveButton->setVisible(false);
	ui.addButton->setVisible(false);
	ui.removeButton->setVisible(false);
	QString avatar = buddy->avatar();
	if (avatar.isEmpty())
		avatar = ":/icons/qutim_64.png";
	ui.pictureLabel->setPixmap(QPixmap(avatar).scaled(QSize(64, 64), Qt::KeepAspectRatio));
	addItems(request->item());
	if (curPage >= 0)
		ui.infoListWidget->setCurrentRow(curPage);
	if (request->state() == InfoRequest::Done || request->state() == InfoRequest::Cancel) {
		request->deleteLater(); request = 0;
	} else {
		connect(request, SIGNAL(stateChanged(InfoRequest::State)), SLOT(onRequestStateChanged(InfoRequest::State)));
	}
}

void MainWindow::onRequestStateChanged(InfoRequest::State state)
{
	if (request != sender())
		return;
	Q_UNUSED(state);
	setBuddy(m_buddy, request);
}

void MainWindow::onRequestButton()
{
	InfoRequest *request = m_buddy->infoRequest();
	if (!request)
		return;
	setBuddy(m_buddy, request);
}

void MainWindow::addItems(const InfoItem &items)
{
	if (!items.hasSubitems())
		return;
	// Summary
	QLabel *w = new QLabel(summary(items), ui.detailsStackedWidget);
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
	InfoLayout *general = 0;
	foreach (const InfoItem &item, items.subitems()) {
		if (item.hasSubitems()) {
			QFrame *w = new QFrame(ui.detailsStackedWidget);
			w->setFrameShape(QFrame::Panel);
			w->setFrameShadow(QFrame::Sunken);
			InfoLayout *group = new InfoLayout(w);
			group->addItems(item.subitems());
			group->addSpacer();
			ui.infoListWidget->addItem(item.name());
			ui.detailsStackedWidget->addWidget(w);
		} else {
			if (!general) {
				QFrame *w = new QFrame(ui.detailsStackedWidget);
				w->setFrameShape(QFrame::Panel);
				w->setFrameShadow(QFrame::Sunken);
				general = new InfoLayout(w);
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
		if (item.property("additional", false))
			continue;
		if (item.hasSubitems()) {
			text += summary(item);
		} else if (item.data().canConvert(QVariant::String)) {
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
				text += item.data().toString();
			text += "<br>";
		}
	}
	return text;
}

ContactInfo::ContactInfo()
{
	MenuController::addAction<Contact>(new ActionGenerator(Icon("dialog-information"),
										QT_TRANSLATE_NOOP("ContactInfo", "Show information"),
										this, SLOT(onShow())));
}

void ContactInfo::show(Buddy *buddy)
{
	InfoRequest *request = buddy->infoRequest();
	if (!request)
		return;
	if (!info) {
		info = new MainWindow();
		centerizeWidget(info);
		info->show();
		info->setAttribute(Qt::WA_DeleteOnClose, true);
	} else {
		info->raise();
	}
	info->setBuddy(buddy, request);
}

void ContactInfo::onShow()
{
	QAction *action = qobject_cast<QAction *>(sender());
	Q_ASSERT(action);
	Buddy *buddy = qobject_cast<Buddy*>(action->data().value<MenuController*>());
	Q_ASSERT(buddy);
	show(buddy);
}

}
