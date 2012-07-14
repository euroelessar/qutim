#ifndef QUICKMASKEFFECT_H
#define QUICKMASKEFFECT_H

#include <QGraphicsEffect>
#include <QDeclarativeItem>

namespace MeegoIntegration
{

class QuickMaskEffect : public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeItem *mask READ mask WRITE setMask NOTIFY maskChanged)
    
public:
    explicit QuickMaskEffect(QObject *parent = 0);

    QDeclarativeItem *mask() const;
    void setMask(QDeclarativeItem *mask);

signals:
    void maskChanged(QDeclarativeItem *mask);
    
protected:
    virtual void draw(QPainter *painter);
    
private:
    QPixmap m_buffer;
    QDeclarativeItem *m_mask;
};

}

#endif // QUICKMASKEFFECT_H
