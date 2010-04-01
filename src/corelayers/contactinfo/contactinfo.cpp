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

#include "libqutim/debug.h"

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
#if 0
	if (type == QVariant::Date) {
		addDataWidget(new QDateEdit(item.data().toDate(), this));
	} else if (type == QVariant::DateTime) {
		addDataWidget(new QDateTimeEdit(item.data().toDateTime(), this));
	} else if (type == QVariant::Int || type == QVariant::LongLong || type == QVariant::UInt) {
		QSpinBox *d = new QSpinBox(this);
		d->setValue(item.data().toInt());
		addDataWidget(d);
	} else if (type == QVariant::Double) {
		QDoubleSpinBox *d = new QDoubleSpinBox(this);
		d->setValue(item.data().toDouble());
		addDataWidget(d);
	} else
#endif
	if (type == QVariant::Image) {
		QLabel *d = new QLabel(this);
		d->setPixmap(QPixmap::fromImage(item.data().value<QImage>()));
		addDataWidget(d);
	} else if (type == QVariant::Pixmap) {
		QLabel *d = new QLabel(this);
		d->setPixmap(item.data().value<QPixmap>());
		addDataWidget(d);
	} else if (item.data().canConvert(QVariant::StringList)) {
		foreach (const QString &i, item.data().toStringList())
			addLabel(i);
	} else {
		QString str = item.data().toString();
		bool enabled = !str.isEmpty();
		if (!enabled)
			str = QT_TRANSLATE_NOOP("ContactInfo", "the field is not set");
		addLabel(str)->setEnabled(enabled);
	}

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
	QWidget *widget = new QWidget(this);
	setWidget(widget);
	m_layout = new QVBoxLayout(widget);
}

void MainWindow::setBuddy(Buddy *buddy)
{
	if (!m_groups.isEmpty()) {
		qDeleteAll(m_groups);
		m_groups.clear();
		QLayoutItem *spacer = m_layout->takeAt(0);
		if (spacer)
			delete spacer;
	}
	if (request)
		request->deleteLater();
	request = buddy->infoRequest();
	setWindowTitle(QT_TRANSLATE_NOOP("ContactInfo", "About contact %1 <%2>")
					.toString()
					.arg(buddy->name())
					.arg(buddy->id()));
	if (request->state() == InfoRequest::Done) {
		addItems(request->items());
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
		addItems(request->items());
	if (state == InfoRequest::Done || state == InfoRequest::Cancel)
		request->deleteLater(); request = 0;
}

void MainWindow::addItems(const QList<InfoItem> &items)
{
	foreach (const InfoItem &item, items) {
		QList<LocalizedString> groups = item.group();
		LocalizedString name = groups.count() >= 1 ? groups.at(0) : QT_TRANSLATE_NOOP("ContactInfo", "General");
		group(name)->addItem(item);
	}
	QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	m_layout->addItem(spacer);
}

InfoGroup *MainWindow::group(const LocalizedString &localozedName)
{
	QString name = localozedName.original();
	if (!m_groups.contains(name)) {
		InfoGroup *group = new InfoGroup(this);
		group->setTitle(localozedName);
		m_layout->addWidget(group);
		m_groups.insert(name, group);
	}
	return m_groups.value(name);
}

ContactInfo::ContactInfo()
{
	MenuController::addAction<Contact>(new ActionGenerator(Icon("dialog-information"),
										QT_TRANSLATE_NOOP("ContactInfo", "Show information"),
										this, SLOT(onShow())));
}

void ContactInfo::show(Buddy *buddy)
{
	if (!info) {
		info = new MainWindow();
		info->show();
		info->setAttribute(Qt::WA_DeleteOnClose, true);
	} else {
		info->raise();
	}
	info->setBuddy(buddy);

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
