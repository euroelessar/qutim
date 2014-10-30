#ifndef CORE_QUICKMACTOOLBAR_H
#define CORE_QUICKMACTOOLBAR_H

#include <qutim/quickaction.h>
#include <QMacToolBar>
#include <QMacToolBarItem>

namespace Core {

class QuickMacToolBar : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QWindow *window READ window WRITE setWindow NOTIFY windowChanged)
public:
    explicit QuickMacToolBar(QObject *parent = 0);

    static void registerTypes();

    QWindow *window() const;
    void setWindow(QWindow *window);

public slots:
    void insert(int index, qutim_sdk_0_3::QuickAction *action);
    void replace(int index, qutim_sdk_0_3::QuickAction *action);
    void remove(int index);

signals:
    void windowChanged(QWindow *window);

private:
    void reloadItems();

    QMacToolBar m_toolBar;
    QList<QMacToolBarItem *> m_items;
    QWindow * m_window;
};

} // namespace Core

#endif // CORE_QUICKMACTOOLBAR_H
