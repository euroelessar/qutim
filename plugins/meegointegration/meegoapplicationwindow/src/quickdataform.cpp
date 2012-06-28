#include "quickdataform.h"
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <qutim/localizedstring.h>
#include <qutim/thememanager.h>
#include <qutim/metaobjectbuilder.h>

namespace MeegoIntegration
{

using namespace qutim_sdk_0_3;

QuickDataForm::QuickDataForm(QDeclarativeItem *parent) :
    QDeclarativeItem(parent), m_hasSubitems(false), m_column(0)
{
    //    setFlag(ItemHasNoContents, false);
    qRegisterMetaType<DataItem>();
    m_basePath = ThemeManager::path(QLatin1String("declarative"), QLatin1String("meego"));
    m_basePath += QLatin1Char('/');
}

QVariant QuickDataForm::item()
{
    updateItem(m_item);
    return qVariantFromValue(m_item);
}

void QuickDataForm::setItem(const QVariant &item)
{
    m_item = item.value<DataItem>();
    emit itemChanged(item);
    buildContent();
}

QDeclarativeComponent *QuickDataForm::loadComponent(const QString &name)
{
    QDeclarativeComponent * &component = m_componentsCache[name];
    if (!component) {
        QUrl url = QUrl::fromLocalFile(m_basePath + name);
        component = new QDeclarativeComponent(m_app->engine(), url, this);
        qDebug() << url << component->isLoading() << component->isError() << component->errorString();
    }
    return component;
}

void QuickDataForm::onHeightChanged()
{
    setImplicitHeight(m_column->implicitHeight());
}

void QuickDataForm::buildContent()
{
    if (m_column)
        delete m_column;
    m_items.clear();
    m_hasSubitems = m_item.hasSubitems() || m_item.isAllowedModifySubitems();
    QDeclarativeComponent *component = loadComponent("dataform/DataColumn.qml");
    m_context = new QDeclarativeContext(QDeclarativeEngine::contextForObject(this));
    m_column = static_cast<QDeclarativeItem*>(component->beginCreate(m_context));
    connect(m_column, SIGNAL(implicitHeightChanged()), this, SLOT(onHeightChanged()));
    m_column->setParentItem(this);
    m_context->setParent(m_column);
    component->completeCreate();
    onHeightChanged();
    if (m_item.hasSubitems()) {
        foreach (const DataItem &item, m_item.subitems())
            addItem(item);
    } else {
        addItem(m_item);
    }
}

void QuickDataForm::updateItem(DataItem &item)
{
    if (item.isNull())
        return;
    QVariant::Type type = item.data().type();
    QObject *object = m_items.value(item.name());
    if (item.hasSubitems()) {
        QList<DataItem> items = item.subitems();
        for (int i = 0; i < items.size(); ++i)
            updateItem(items[i]);
        item.setSubitems(items);
    } else if (!object || item.isReadOnly()) {
        return;
    } else if (type == QVariant::StringList || item.data().canConvert<LocalizedStringList>()) {
//		return new StringListGroup(dataForm, item, parent);
    } else if (item.isAllowedModifySubitems()) {
//		return new ModifiableGroup(dataForm, item, parent);
    } else if (type == QVariant::Bool) {
        item.setData(object->property("checked").toBool());
    } else if (type == QVariant::Date) {
//		return new DateEdit(dataForm, item, parent);
    } else if (type == QVariant::DateTime) {
//		return new DateTimeEdit(dataForm, item, parent);
    } else if (type == QVariant::Int || type == QVariant::LongLong || type == QVariant::UInt) {
        QVariant result = object->property("value");
        result.convert(type);
        item.setData(result);
    } else if (type == QVariant::Double) {
        item.setData(object->property("value").toDouble());
    } else if (type == QVariant::Icon || type == QVariant::Pixmap || type == QVariant::Image) {
//		if (item.property("alternatives").isNull())
//			return new IconWidget(dataForm, item, parent);
//		else
//			return new IconListWidget(dataForm, item, parent);
    } else {
        item.setData(object->property("text").toString());
    }
}

static QPixmap variantToPixmap(const QVariant &data, const QSize &size)
{
	int type = data.type();
	QPixmap pixmap;
	if (type == QVariant::Icon)
		return data.value<QIcon>().pixmap(size); // The pixmap can be returned as its size is correct.
	else if (type == QVariant::Pixmap)
		pixmap = data.value<QPixmap>();
	else if (type == QVariant::Image)
		pixmap = QPixmap::fromImage(data.value<QImage>());
	if (!pixmap.isNull())
		return pixmap.scaled(size, Qt::KeepAspectRatio);
	else
		return pixmap;
}

static QStringList variantToStringList(const QVariant &data)
{
	QStringList list = qVariantValue<QStringList>(data);
	if (list.isEmpty()) {
		foreach (const LocalizedString &str, qVariantValue<LocalizedStringList>(data))
			list << str;
	}
	return list;
}

class ComponentCreator
{
public:
    ComponentCreator(QuickDataForm *form, const QString &id, const QString &name) : m_form(form)
    {
        m_component = form->loadComponent(name);
        QDeclarativeContext *context = new QDeclarativeContext(form->m_context, form->m_column);
        QObject *object = m_component->beginCreate(context);
        m_object = qobject_cast<QDeclarativeItem*>(object);
//        const char *property = MetaObjectBuilder::info(m_form->m_column->metaObject(), "DefaultProperty");
//        qDebug("%s: %s \"%s\"", Q_FUNC_INFO, m_form->m_column->metaObject()->className(), property);
//        QDeclarativeListReference list(m_form->m_column, property);
//        qDebug("%s", list.listElementType()->className());
//        list.append(m_object);
        m_object->setParentItem(form->m_column);
        if (!id.isEmpty())
            form->m_items.insert(id, m_object);
    }
    
    ~ComponentCreator()
    {
        m_component->completeCreate();
    }
    
    QDeclarativeItem *object() const
    {
        return m_object;
    }

private:
    QuickDataForm *m_form;
    QDeclarativeItem *m_object;
    QDeclarativeComponent *m_component;
};

void QuickDataForm::addLabel(const DataItem &item)
{
    if (item.property("hidden", false))
        return;
    ComponentCreator creator(this, QString(), "dataform/DataLabel.qml");
    creator.object()->setProperty("text", item.title().toString());
}

void QuickDataForm::addItem(const DataItem &item)
{
    if (item.isNull())
        return;
    QVariant::Type type = item.data().type();
    if (item.isReadOnly()) {
//		if (item.hasSubitems()) {
//			if (twoColumn)
//				*twoColumn = true;
//			return new DataGroup(dataForm, item, parent);
//		} else {
//			return new Label(dataForm, item, parent);
//		}
	} else if (type == QVariant::StringList || item.data().canConvert<LocalizedStringList>()) {
//		return new StringListGroup(dataForm, item, parent);
	} else if (item.isAllowedModifySubitems()) {
//		return new ModifiableGroup(dataForm, item, parent);
	} else if (item.hasSubitems()) {
//		return new DataGroup(dataForm, item, parent);
	} else if (type == QVariant::Bool) {
//		return new CheckBox(dataForm, item, parent);
	} else if (type == QVariant::Date) {
//		return new DateEdit(dataForm, item, parent);
	} else if (type == QVariant::DateTime) {
//		return new DateTimeEdit(dataForm, item, parent);
	} else if (type == QVariant::Int || type == QVariant::LongLong || type == QVariant::UInt) {
        addLabel(item);
        ComponentCreator creator(this, item.name(), "dataform/DataIntegerField.qml");
        bool ok;
        int value = item.property("maxValue").toInt(&ok);
        if (ok)
            creator.object()->setProperty("maximumValue", value);
        value = item.property("minValue").toInt(&ok);
        if (ok)
            creator.object()->setProperty("minimumValue", value);
        creator.object()->setProperty("value", item.data());
	} else if (type == QVariant::Double) {
//		return new DoubleSpinBox(dataForm, item, parent);
	} else if (type == QVariant::Icon || type == QVariant::Pixmap || type == QVariant::Image) {
//		if (item.property("alternatives").isNull())
//			return new IconWidget(dataForm, item, parent);
//		else
//			return new IconListWidget(dataForm, item, parent);
	} else {
        QString str;
        if (item.data().canConvert<LocalizedString>())
            str = item.data().value<LocalizedString>();
        else
            str = item.data().toString();
        QStringList alt = variantToStringList(item.property("alternatives"));
        if (!alt.isEmpty()) {
//          return new ComboBox(dataForm, str, alt, false, item, parent);
        } else {
            addLabel(item);
            ComponentCreator creator(this, item.name(), item.property("multiline", false)
                                     ? "dataform/DataAreaField.qml"
                                     : "dataform/DataTextField.qml");
            creator.object()->setProperty("text", item.data());
        }
    }
}

}
