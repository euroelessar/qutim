#include "datasettingsobject_p.h"

namespace qutim_sdk_0_3 {

DataSettingsObject::DataSettingsObject(QObject *parent) :
    QObject(parent), d_ptr(new DataSettingsObjectPrivate)
{
}

DataSettingsObject::~DataSettingsObject()
{
}

DataItem DataSettingsObject::item() const
{
    return d_func()->item;
}

void DataSettingsObject::setController(QObject *controller)
{
    Q_D(DataSettingsObject);
    if (d->controller != controller) {
        d->controller = controller;
        emit controllerChanged(controller);
        setControllerImpl(controller);
    }
}

QObject *DataSettingsObject::controller() const
{
    return d_func()->controller;
}

void DataSettingsObject::load()
{
    loadImpl();
}

void DataSettingsObject::save(const DataItem &item)
{
    saveImpl(item);
}

void DataSettingsObject::cancel()
{
    cancelImpl();
}

void DataSettingsObject::cancelImpl()
{
    loadImpl();
}

void DataSettingsObject::setControllerImpl(QObject *controller)
{
    Q_UNUSED(controller);
}

void DataSettingsObject::setItem(const DataItem &item)
{
    d_func()->item = item;
    emit itemChanged(item);
}

class DataSettingsWidgetPrivate
{
public:
};

DataSettingsWidget::DataSettingsWidget(DataSettingsObject *object) : m_object(object)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(0);
    connect(object, SIGNAL(itemChanged(qutim_sdk_0_3::DataItem)),
            this, SLOT(onItemChanged(qutim_sdk_0_3::DataItem)));
}

DataSettingsWidget::~DataSettingsWidget()
{
}

void DataSettingsWidget::setController(QObject *controller)
{
    m_object->setController(controller);
}

void DataSettingsWidget::onItemChanged(const DataItem &item)
{
    delete m_form;
    m_form = AbstractDataForm::get(item);
    if (m_form) {
        connect(m_form, SIGNAL(changed()), SLOT(onChanged()));
        m_layout->addWidget(m_form);
    }
}

void DataSettingsWidget::onChanged()
{
    emit modifiedChanged(true);
}

void DataSettingsWidget::loadImpl()
{
    m_object->load();
    if (m_form)
        m_form->clearState();
}

void DataSettingsWidget::saveImpl()
{
    if (m_form)
        m_object->save(m_form->item());
}

void DataSettingsWidget::cancelImpl()
{
	loadImpl();
}

DataSettingsItem::DataSettingsItem(Settings::Type type, const QIcon &icon, const LocalizedString &text) :
    SettingsItem(*new DataSettingsItemPrivate)
{
	Q_D(DataSettingsItem);
	d->icon = icon;
	d->text = text;
	d->type = type;
}

DataSettingsItem::DataSettingsItem(Settings::Type type, const LocalizedString &text) :
    SettingsItem(*new DataSettingsItemPrivate)
{
	Q_D(DataSettingsItem);
	d->text = text;
	d->type = type;
}

DataSettingsItem::~DataSettingsItem()
{
}

const ObjectGenerator *DataSettingsItem::generator() const
{
    return new DataSettingsGenerator(const_cast<DataSettingsItem*>(this));
}

} // namespace qutim_sdk_0_3
