#ifndef QUTIM_SDK_0_3_QUICKACTION_H
#define QUTIM_SDK_0_3_QUICKACTION_H

#include <QObject>
#include <QKeySequence>
#include <QVariant>
#include <QUrl>

namespace qutim_sdk_0_3 {

class QuickAction : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QUrl iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(QString iconName READ iconName WRITE setIconName NOTIFY iconNameChanged)
    Q_PROPERTY(QString tooltip READ tooltip WRITE setTooltip NOTIFY tooltipChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable NOTIFY checkableChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked DESIGNABLE isCheckable NOTIFY toggled)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QVariant shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)
public:
    explicit QuickAction(QObject *parent = 0);
    ~QuickAction();

    QString text() const;
    QUrl iconSource() const;
    QString iconName() const;
    QString tooltip() const;
    bool isEnabled() const;
    bool isCheckable() const;
    bool isChecked() const;
    QVariant shortcut() const;
    bool isVisible() const;

    void setText(const QString &arg);
    void setIcon(const QIcon &icon);
    void setIconSource(const QUrl &arg);
    void setIconName(const QString &arg);
    void setTooltip(const QString &arg);
    void setEnabled(bool arg);
    void setCheckable(bool arg);
    void setChecked(bool arg);
    void setShortcut(const QVariant &arg);
    void setVisible(bool arg);

public slots:
    void trigger();
    void toggle();

signals:
    void textChanged(QString arg);
    void iconSourceChanged(QUrl arg);
    void iconNameChanged(QString arg);
    void tooltipChanged(QString arg);
    void enabledChanged(bool arg);
    void checkableChanged(bool arg);
    void toggled(bool arg);
    void shortcutChanged(QVariant arg);
    void visibleChanged(bool arg);

    void triggered(bool checked);

private:
    QString m_text;
    QUrl m_iconSource;
    QString m_iconName;
    QString m_tooltip;
    bool m_enabled;
    bool m_checkable;
    bool m_checked;
    QKeySequence m_shortcut;
    bool m_visible;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKACTION_H
