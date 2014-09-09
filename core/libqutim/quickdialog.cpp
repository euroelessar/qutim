#include "quickdialog.h"
#include "config.h"
#include "thememanager.h"
#include "declarativeview.h"
#include "debug.h"

#include <QQmlComponent>
#include <QPointer>

namespace qutim_sdk_0_3 {

class QuickDialogPrivate
{
public:
	QuickDialogPrivate(QuickDialog *q) : q_ptr(q), visible(false)
	{
	}

	QuickDialog *q_ptr;
	bool visible;
	QString dialogName;
	QScopedPointer<QQmlComponent> component;
	QPointer<QObject> dialog;
	QMetaObject::Connection deathConnection;
	QMetaProperty visibleProperty;
	QMetaMethod onVisibleChanged;

	void updateVisible()
	{
		bool newVisible = false;
		if (visibleProperty.isValid()) {
			Q_ASSERT(dialog.data());
			newVisible = visibleProperty.read(dialog.data()).toBool();
		}

		if (newVisible != visible) {
			visible = newVisible;
			emit q_ptr->visibleChanged(visible);
		}
	}

	void ensureInitialized()
	{
		if (!onVisibleChanged.isValid()) {
			int index = q_ptr->metaObject()->indexOfMethod("implementationVisibileChanged()");
			Q_ASSERT(index >= 0);
			onVisibleChanged = q_ptr->metaObject()->method(index);
		}

		if (component.isNull()) {
			Config config;
			config.beginGroup(QStringLiteral("qml/themes"));
			QString themeName = config.value(dialogName, QStringLiteral("default"));
			QString themePath = ThemeManager::path(QStringLiteral("qml/") + dialogName, themeName);

			if (themePath.isEmpty()) {
				qWarning() << "Failed to find theme:" << themeName << "for dialog:" << dialogName;
				themePath = ThemeManager::path(QStringLiteral("qml/") + dialogName, QStringLiteral("default"));
				if (themePath.isEmpty()) {
					qCritical() << "Failed to find default theme for dialog:" << dialogName;
					return;
				}
			}

			QString fileName = themePath + QStringLiteral("/main.qml");

			component.reset(new QQmlComponent(DeclarativeView::globalEngine(), fileName));
		}

		if (dialog.isNull()) {
			dialog = component->create();
			visibleProperty = QMetaProperty();

			if (!dialog) {
				qCritical() << "Failed to create object for component:" << component->url() << "errors:";
				for (QQmlError error : component->errors()) {
					qCritical() << error.toString();
				}
				return;
			}

			deathConnection = QObject::connect(dialog.data(), &QObject::destroyed, q_ptr, [this] () {
				dialog.clear();
				visibleProperty = QMetaProperty();
				updateVisible();
			});

			int visibleIndex = dialog->metaObject()->indexOfProperty("visible");
			if (visibleIndex < 0) {
				qCritical() << "Failed to find \"visible\" property for component:" << component->url();
				return;
			}

			visibleProperty = dialog->metaObject()->property(visibleIndex);

			if (!visibleProperty.hasNotifySignal()) {
				qCritical() << "Property \"visible\" has no notify signal for component:" << component->url();
			} else {
				QObject::connect(dialog.data(), visibleProperty.notifySignal(), q_ptr, onVisibleChanged);
			}

			updateVisible();
		}
	}
};

QuickDialog::QuickDialog(const QString &name) : d_ptr(new QuickDialogPrivate(this))
{
	Q_D(QuickDialog);

	d->dialogName = name;
}

QuickDialog::~QuickDialog()
{
	Q_D(QuickDialog);

	if (d->deathConnection)
		QObject::disconnect(d->deathConnection);

	delete d->dialog.data();
}

void QuickDialog::setVisible(bool visible)
{
	Q_D(QuickDialog);

	d->ensureInitialized();
	if (d->dialog && d->visibleProperty.isValid()) {
		d->visibleProperty.write(d->dialog.data(), visible);
	}
}

bool QuickDialog::isVisible()
{
	return d_func()->visible;
}

void QuickDialog::show()
{
	setVisible(true);
}

void QuickDialog::hide()
{
	setVisible(false);
}

void QuickDialog::implementationVisibileChanged()
{
	d_func()->updateVisible();
}

} // namespace qutim_sdk_0_3
