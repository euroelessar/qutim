#ifndef CORE_QUICKMACWINDOWBACKGROUNDBLUR_H
#define CORE_QUICKMACWINDOWBACKGROUNDBLUR_H

#include <QWindow>

namespace Core {

class QuickMacWindowBackgroundBlur : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QWindow* window READ window WRITE setWindow NOTIFY windowChanged)
    Q_PROPERTY(int radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
public:
    explicit QuickMacWindowBackgroundBlur(QObject *parent = 0);

    QWindow *window() const;
    void setWindow(QWindow *window);

    int radius() const;
    void setRadius(int radius);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &backgroundColor);

signals:
    void windowChanged(QWindow *window);
    void radiusChanged(int radius);
    void backgroundColorChanged(const QColor &backgroundColor);

private:
    void forceSetBackgroundColor();
    void forceSetRadius(int radius);

    QWindow *m_window;
    int m_radius;
    QColor m_backgroundColor;
};

} // namespace Core

#endif // CORE_QUICKMACWINDOWBACKGROUNDBLUR_H
