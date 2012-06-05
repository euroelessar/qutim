#include "quickregexpservice.h"
#include <QDebug>

QuickRegExpService::QuickRegExpService(QObject *parent) :
    QObject(parent)
{
}

QVariant QuickRegExpService::fromRegExp(const QVariant &variant)
{
    QRegExp regexp = variant.toRegExp();
    QVariantMap data;
    data.insert(QLatin1String("pattern"), regexp.pattern());
    data.insert(QLatin1String("patternSyntax"), regexp.patternSyntax());
    return data;
}

QVariant QuickRegExpService::toRegExp(const QVariant &variant)
{
    QVariantMap data = variant.toMap();
    QRegExp regexp;
    regexp.setPattern(data.value(QLatin1String("pattern")).toString());
    int syntax = data.value(QLatin1String("patternSyntax"), QRegExp::RegExp).toInt();
    regexp.setPatternSyntax(static_cast<QRegExp::PatternSyntax>(syntax));
    return QVariant(regexp);
}
