#ifndef NO_RTF_SUPPORT

#include <QTextCodec>
#include "rtftextreader.h"

RtfTextReader::RtfTextReader(const char *defaultEncoding)
    : RtfReader(QString().toStdString())
{
    m_defaultEnc = defaultEncoding;
    m_codec = QTextCodec::codecForName(defaultEncoding);
}

RtfTextReader::~RtfTextReader() {
    m_codec = 0;
}

void RtfTextReader::addCharData(const char *data, size_t len, bool convert) {
    m_result.append(m_codec->toUnicode(data,len));
}

void RtfTextReader::insertImage(const std::string &mimeType, const std::string &fileName, size_t startOffset, size_t size) {
}

void RtfTextReader::setEncoding(int code) {
    m_codec = QTextCodec::codecForName(QString("cp%1").arg(code).toAscii().constData());

    if (!m_codec) {
        QTextCodec::codecForName(m_defaultEnc.constData());
    }
}

void RtfTextReader::switchDestination(DestinationType destination, bool on) {
}

void RtfTextReader::setAlignment() {
}

void RtfTextReader::setFontProperty(FontProperty property) {
}

void RtfTextReader::newParagraph() {
    if (!m_result.isEmpty())
        m_result.append('\n');
}

QString RtfTextReader::getText() const {
    return m_result;
}

void RtfTextReader::flushBuffers() {
    m_result.clear();
}

#endif //NO_RTF_SUPPORT
