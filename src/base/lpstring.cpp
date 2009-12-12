#include <QIODevice>
#include <QTextCodec>

#include "protoutils.h"
#include "lpstring.h"

quint32 LPString::read(QIODevice& device, bool unicode)
{
    m_str.clear();
    m_unicode = unicode;
    m_arr = device.read(ByteUtils::readUint32(device));
    return m_arr.length() + sizeof(quint32);
}

quint32 LPString::read(const QByteArray& arr, quint32 pos, bool unicode)
{
    quint32 len = ByteUtils::readUint32(arr,pos);
    m_str.clear();
    m_unicode = unicode;
    m_arr = arr.mid(pos+sizeof(len),len); 
    return m_arr.length() + sizeof(len);
}

LPString* LPString::readFrom(QIODevice& device, bool unicode)
{
    LPString *str = new LPString;
    str->read(device,unicode);
    return str;
}

LPString* LPString::readFrom(const QByteArray& arr, quint32 pos, bool unicode)
{
    LPString *str = new LPString;
    str->read(arr,pos,unicode);
    return str;
}

QByteArray LPString::toByteArray(const QString& str, bool unicode)
{
    QByteArray arr;
    
    QString codepage = (unicode) ? "UTF-16LE" : "CP1251";
    QTextCodec* codec = QTextCodec::codecForName(codepage.toLocal8Bit());
    
    if (codec != 0)
    {
        QTextCodec::ConverterState convState(QTextCodec::IgnoreHeader);

        if (!str.isEmpty())
        {
            arr.append(codec->fromUnicode(str.data(),str.size(),&convState));
        }
        arr.prepend(ByteUtils::toByteArray(arr.length()));
    }
    return arr;    
}

QByteArray LPString::toByteArray()
{
    if (m_arr.isNull())
    {
        m_arr = toByteArray(m_str,m_unicode);
    }
    return m_arr;
}

QByteArray LPString::toByteArray(bool unicode)
{
    m_unicode = unicode;
    return toByteArray();
}

QString LPString::toString()
{
    if (m_str.isNull())
    {
        QString codepage = (m_unicode) ? "UTF-16LE" : "CP1251";
        QTextCodec* codec = QTextCodec::codecForName(codepage.toLocal8Bit());
    
        if (codec != NULL)
        {
            QTextCodec::ConverterState convState(QTextCodec::IgnoreHeader);
            m_str = codec->toUnicode(m_arr.constData(),m_arr.length(),&convState);
        }
    }
    return m_str;
}

QString LPString::toString(bool unicode)
{
    m_unicode = unicode;
    return toString();
}

LPString::~LPString()
{
}
