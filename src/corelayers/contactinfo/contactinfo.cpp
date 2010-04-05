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

InfoGroup::InfoGroup(QWidget *parent) :
	QGroupBox(parent), m_layout(new QGridLayout(this)), m_row(0)
{
}

void InfoGroup::addItems(const QList<InfoItem> &items)
{
	foreach (const InfoItem &item, items) {
		if (item.hasSubitems()) {
			InfoGroup *group = new InfoGroup(this);
			group->setTitle(item.title());
			group->addItems(item.subitems());
			m_layout->addWidget(group, m_row++, 0, 1, 2);
		} else {
			addItem(item);
		}
	}
}

void InfoGroup::addItem(const InfoItem &item)
{
	QLabel *title = new QLabel(item.title().toString() + ":", this);
	title->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	QFont font;
	font.setBold(true);
	title->setFont(font);
	title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	m_layout->addWidget(title, m_row, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);

	QVariant::Type type = item.data().type();
	if (type == QVariant::Date) {
		addLabel(item.data().toDate().toString(Qt::SystemLocaleLongDate));
		return;
	} else if (type == QVariant::DateTime) {
		addLabel(item.data().toDateTime().toString(Qt::SystemLocaleLongDate));
		return;
	} else if (type == QVariant::Image) {
		QLabel *d = new QLabel(this);
		d->setPixmap(QPixmap::fromImage(item.data().value<QImage>()));
		addDataWidget(d);
		return;
	} else if (type == QVariant::Pixmap) {
		QLabel *d = new QLabel(this);
		d->setPixmap(item.data().value<QPixmap>());
		addDataWidget(d);
		return;
	} else if (type == QVariant::Bool) {
		addLabel(item.data().toBool() ?
				 QT_TRANSLATE_NOOP("ContactInfo", "yes") :
				 QT_TRANSLATE_NOOP("ContactInfo", "no"));
		return;
	} else if (item.data().canConvert<QHostAddress>()) {
		QHostAddress address = item.data().value<QHostAddress>();
		if (!address.isNull()) {
			addLabel(address.toString());
			return;
		}
	} else if (item.data().canConvert(QVariant::StringList)) {
		QStringList list = item.data().toStringList();
		if (!list.isEmpty()) {
			foreach (const QString &i, list)
				addLabel(i);
			return;
		}
	}
	QString str = item.data().toString();
	bool enabled = !str.isEmpty();
	if (!enabled)
		str = QT_TRANSLATE_NOOP("ContactInfo", "the field is not set");
	addLabel(str)->setEnabled(enabled);
}

void InfoGroup::addDataWidget(QWidget *widget)
{
	widget->setProperty("readOnly", true);
	widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	m_layout->addWidget(widget, m_row++, 1, 1, 1, Qt::AlignLeft);
}

QLabel *InfoGroup::addLabel(const QString &data)
{
	QLabel *d = new QLabel(data, this);
	d->setTextInteractionFlags(Qt::LinksAccessibleByMouse |
							Qt::LinksAccessibleByKeyboard |
							Qt::TextSelectableByMouse |
							Qt::TextSelectableByKeyboard);
	addDataWidget(d);
	return d;
}

MainWindow::MainWindow() :
	request(0)
{
	setMinimumSize(350, 250);
	setFrameShape(QScrollArea::NoFrame);
	setWidgetResizable(true);
}

void MainWindow::setBuddy(Buddy *buddy, InfoRequest *req)
{
	if (request)
		request->deleteLater();
	request = req;
	if (widget())
		delete widget();
	QWidget *w = new QWidget(this);
	setWidget(w);
	m_layout = new QVBoxLayout(w);
	setWindowTitle(QT_TRANSLATE_NOOP("ContactInfo", "About contact %1 <%2>")
					.toString()
					.arg(buddy->name())
					.arg(buddy->id()));
	if (request->state() == InfoRequest::Done) {
		addItems(request->item());
		request->deleteLater(); request = 0;
	} else {
		connect(request, SIGNAL(stateChanged(InfoRequest::State)), SLOT(onRequestStateChanged(InfoRequest::State)));
	}
}

void MainWindow::onRequestStateChanged(InfoRequest::State state)
{
	if (request != sender())
		return;
	if (state == InfoRequest::Done)
		addItems(request->item());
	if (state == InfoRequest::Done || state == InfoRequest::Cancel)
		request->deleteLater(); request = 0;
}

void MainWindow::addItems(const InfoItem &items)
{
	InfoGroup *general = 0;
	foreach (const InfoItem &item, items.subitems()) {
		if (item.hasSubitems()) {
			InfoGroup *group = new InfoGroup(widget());
			group->setTitle(item.name());
			m_layout->addWidget(group);
			group->addItems(item.subitems());
		} else {
			if (!general) {
				general = new InfoGroup(widget());
				general->setTitle(QT_TRANSLATE_NOOP("ContactInfo", "General"));
				m_layout->addWidget(general);
			}
			general->addItem(item);
		}
	}
	QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	m_layout->addItem(spacer);
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
