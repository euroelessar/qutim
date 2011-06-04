#ifndef NO_RTF_SUPPORT

#include <QTextCodec>
#include "rtftextreader.h"

RtfTextReader::RtfTextReader(const char *defaultEncoding)
    : RtfReader(std::string()), m_cursor(&m_document)
{
    m_defaultEnc = defaultEncoding;
    m_codec = QTextCodec::codecForName(defaultEncoding);
}

RtfTextReader::~RtfTextReader() {
    m_codec = 0;
}

void RtfTextReader::addCharData(const char *data, size_t len, bool convert)
{
	Q_UNUSED(convert);
	m_cursor.insertText(m_codec->toUnicode(data, len));
}

void RtfTextReader::insertImage(const std::string &mimeType, const std::string &fileName, size_t startOffset, size_t size)
{
	Q_UNUSED(mimeType);
	Q_UNUSED(fileName);
	Q_UNUSED(startOffset);
	Q_UNUSED(size);
}

void RtfTextReader::setEncoding(int code)
{
    m_codec = QTextCodec::codecForName(QString("cp%1").arg(code).toAscii().constData());

    if (!m_codec)
        m_codec = QTextCodec::codecForName(m_defaultEnc.constData());
}

void RtfTextReader::switchDestination(DestinationType destination, bool on)
{
	Q_UNUSED(destination);
	Q_UNUSED(on);
}

void RtfTextReader::setAlignment()
{
}

void RtfTextReader::setFontProperty(FontProperty property)
{
	QTextCharFormat format = m_cursor.charFormat();
	switch (property) {
	case RtfReader::FONT_BOLD:
		format.setFontWeight(format.fontWeight() == QFont::Bold ? QFont::Normal : QFont::Bold);
		break;
	case RtfReader::FONT_ITALIC:
		format.setFontItalic(!format.fontItalic());
		break;
	case RtfReader::FONT_UNDERLINED:
		format.setFontUnderline(!format.fontUnderline());
		break;
	}
	m_cursor.setCharFormat(format);
}

void RtfTextReader::newParagraph()
{
	m_cursor.insertBlock();
}

QString RtfTextReader::getText() const
{
    return m_document.toPlainText();
}

QString RtfTextReader::getHtml() const
{
	return m_document.toHtml();
}

void RtfTextReader::flushBuffers()
{
	m_cursor = QTextCursor(&m_document);
	m_document.clear();
}

#endif //NO_RTF_SUPPORT
