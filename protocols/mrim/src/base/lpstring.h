/****************************************************************************
 *  lpstring.h
 *
 *  Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef LPSTRING_H
#define LPSTRING_H

#include <QMetaType>
#include <QString>
#include <QByteArray>

class LPString
{
public:
    inline LPString();
    inline LPString(const QString& str, bool unicode = false);
    inline LPString(const QByteArray& arr, bool unicode = false);
    inline LPString(const char* str, bool unicode = false);
    virtual ~LPString();

    static LPString* readFrom(class QIODevice& device, bool unicode = false);
    static LPString* readFrom(const QByteArray& arr, quint32 pos = 0, bool unicode = false);
    static QByteArray toByteArray(const QString& str, bool unicode = false);
    static QString toString(const QByteArray& arr, bool unicode = false);

    quint32 read(class QIODevice& device, bool unicode = false);
    quint32 read(const QByteArray& arr, quint32 pos = 0, bool unicode = false);
    
    inline QByteArray array() const;

    QByteArray toByteArray();
    QByteArray toByteArray(bool unicode);
    QString toString();
    QString toString(bool unicode);

private:
    QByteArray m_arr;
    QString    m_str;    
    bool       m_unicode;
};

inline LPString::LPString() : m_unicode(false)
{ }

inline LPString::LPString(const QString& str, bool unicode)
    : m_str(str), m_unicode(unicode) 
{ }

inline LPString::LPString(const QByteArray& arr, bool unicode)
    : m_arr(arr), m_unicode(unicode)
{ }

inline LPString::LPString(const char* str, bool unicode)
    : m_arr(str), m_unicode(unicode)
{ }

inline QByteArray LPString::array() const
{ return m_arr; }

Q_DECLARE_METATYPE(LPString);

#endif // LPSTRING_H
