#include "quickaction.h"
#include <QIcon>

namespace qutim_sdk_0_3 {

QuickAction::QuickAction(QObject *parent) :
    QObject(parent), m_enabled(true), m_checkable(false), m_checked(false)
{
}

QuickAction::~QuickAction()
{

}

QString QuickAction::text() const
{
    return m_text;
}

QUrl QuickAction::iconSource() const
{
    return m_iconSource;
}

QString QuickAction::iconName() const
{
    return m_iconName;
}

QString QuickAction::tooltip() const
{
    return m_tooltip;
}

bool QuickAction::isEnabled() const
{
    return m_enabled;
}

bool QuickAction::isCheckable() const
{
    return m_checkable;
}

bool QuickAction::isChecked() const
{
    return m_checked;
}

QVariant QuickAction::shortcut() const
{
    return m_shortcut.toString(QKeySequence::NativeText);
}

bool QuickAction::isVisible() const
{
    return m_visible;
}

void QuickAction::setText(const QString &arg)
{
    if (m_text != arg) {
        m_text = arg;
        emit textChanged(arg);
    }
}

void QuickAction::setIcon(const QIcon &icon)
{
    setIconName(icon.name());
}

void QuickAction::setIconSource(const QUrl &arg)
{
    if (m_iconSource != arg) {
        m_iconSource = arg;
        emit iconSourceChanged(arg);
    }
}

void QuickAction::setIconName(const QString &arg)
{
    if (m_iconName != arg) {
        m_iconName = arg;
        emit iconNameChanged(arg);
    }
}

void QuickAction::setTooltip(const QString &arg)
{
    if (m_tooltip != arg) {
        m_tooltip = arg;
        emit tooltipChanged(arg);
    }
}

void QuickAction::setEnabled(bool arg)
{
    if (m_enabled != arg) {
        m_enabled = arg;
        emit enabledChanged(arg);
    }
}

void QuickAction::setCheckable(bool arg)
{
    if (m_checkable != arg) {
        m_checkable = arg;
        emit checkableChanged(arg);
    }
}

void QuickAction::setChecked(bool arg)
{
    if (m_checked != arg) {
        m_checked = arg;
        emit toggled(arg);
    }
}

void QuickAction::setShortcut(const QVariant &arg)
{
    QKeySequence sequence;
    if (arg.type() == QVariant::Int)
        sequence = QKeySequence(static_cast<QKeySequence::StandardKey>(arg.toInt()));
    else
        sequence = QKeySequence::fromString(arg.toString());

    if (sequence == m_shortcut)
        return;

//    if (!m_shortcut.isEmpty())
//        QGuiApplicationPrivate::instance()->shortcutMap.removeShortcut(0, this, m_shortcut);

    m_shortcut = sequence;

//    if (!m_shortcut.isEmpty()) {
//        Qt::ShortcutContext context = Qt::WindowShortcut;
//        QGuiApplicationPrivate::instance()->shortcutMap.addShortcut(this, m_shortcut, context, qShortcutContextMatcher);
//    }
    emit shortcutChanged(shortcut());
}

void QuickAction::setVisible(bool arg)
{
    if (m_visible != arg) {
        m_visible = arg;
        emit visibleChanged(arg);
    }
}

void QuickAction::trigger()
{
    emit triggered(isChecked());
}

void QuickAction::toggle()
{
    setChecked(!isChecked());
}

} // namespace qutim_sdk_0_3
