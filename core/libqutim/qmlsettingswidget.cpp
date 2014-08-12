#include "qmlsettingswidget_p.h"
#include <QFileSelector>
#include <QQmlEngine>
#include <QQmlProperty>
#include <QQmlComponent>
#include <QQuickItem>
#include "systeminfo.h"
#include "declarativeview.h"
#include <QVBoxLayout>
#include <QPalette>

namespace qutim_sdk_0_3 {

#define ITEM_WRAPPER \
    "import QtQuick 2.1\n" \
    "import QtQuick.Controls 1.1\n" \
    "import QtQuick.Window 2.1\n" \
    "\n" \
    "Window {\n" \
    "    id: root\n" \
    "}\n"

QmlSettingsWidget::QmlSettingsWidget(const QString &name, QWidget *parent) :
    SettingsWidget(parent), m_object(nullptr)
{
    setObjectName(QStringLiteral("QmlSettingsWidget(%1)").arg(name));

//    QQmlComponent windowComponent(DeclarativeView::globalEngine());
//    windowComponent.setData(ITEM_WRAPPER, QUrl());
//    QObject *windowObject = windowComponent.create();
//    if (!windowObject) {
//        qDebug() << windowComponent.errors();
//    }

//    QQuickWindow *window = qobject_cast<QQuickWindow *>(windowObject);
//    Q_ASSERT(windowObject);
//    Q_ASSERT(window);

    QString path = SystemInfo::getPath(SystemInfo::SystemShareDir);
    path += QStringLiteral("/qml/settings/");
    path += name;
    path += QStringLiteral("/main.qml");

    QFileSelector selector;
    QString selectedPath = selector.select(path);

    QQuickView *window = new DeclarativeView;
    window->setResizeMode(QQuickView::SizeRootObjectToView);
    window->setSource(QUrl::fromLocalFile(selectedPath));
    window->setColor(palette().window().color());

    m_wrapper.reset(window);
    m_object = window->rootObject();

    if (!m_object) {
        qDebug() << window->errors();
        return;
    }

    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    QWidget *contentWidget = QWidget::createWindowContainer(window, this);

    layout->addWidget(contentWidget);

    QQmlProperty modified(m_object, QStringLiteral("__modified"));
    modified.connectNotifySignal(this, SLOT(onModifiedChanged()));
}

void QmlSettingsWidget::setController(QObject *controller)
{
    QQmlProperty property(m_object, QStringLiteral("controller"));
    if (property.isValid() && property.isWritable())
        property.write(QVariant::fromValue(controller));
}

void QmlSettingsWidget::onModifiedChanged()
{
    QQmlProperty modified(m_object, QStringLiteral("__modified"));
    setModified(modified.read().toBool());
}

void QmlSettingsWidget::loadImpl()
{
    QMetaObject::invokeMethod(m_object, "__load");
}

void QmlSettingsWidget::saveImpl()
{
    QMetaObject::invokeMethod(m_object, "__save");
	for (QObject *config : m_object->findChildren<QObject*>(QStringLiteral("QuickConfig"))) {
		QMetaObject::invokeMethod(config, "forceSync");
	}
}

void QmlSettingsWidget::cancelImpl()
{
    QMetaObject::invokeMethod(m_object, "__cancel");
}

} // namespace qutim_sdk_0_3
