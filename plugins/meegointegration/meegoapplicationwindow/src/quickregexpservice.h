#ifndef QUICKREGEXPSERVICE_H
#define QUICKREGEXPSERVICE_H

#include <QObject>
#include <QRegExp>
#include <QVariant>

class QuickRegExpService : public QObject
{
    Q_OBJECT
    Q_ENUMS(PatternSyntax)
public:
    enum PatternSyntax {
        RegExp,
        Wildcard,
        FixedString,
        RegExp2,
        WildcardUnix,
        W3CXmlSchema11
    };
    explicit QuickRegExpService(QObject *parent = 0);
    
    Q_INVOKABLE QVariant fromRegExp(const QVariant &variant);
    Q_INVOKABLE QVariant toRegExp(const QVariant &variant);
    
signals:
    
public slots:
    
};

#endif // QUICKREGEXPSERVICE_H
