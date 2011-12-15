/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Rusanov Peter <peter.rusanov@gmail.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include <QtEndian>
#include <QBuffer>

#ifndef NO_RTF_SUPPORT
#include "../3rdparty/rtf/rtftextreader.h"

#include "protoutils.h"
#include "rtfutils.h"

class RtfPrivate
{
public:
    RtfTextReader *reader;
};

Rtf::Rtf(const char *defaultEncoding) :
    p(new RtfPrivate)
{
    p->reader = new RtfTextReader(defaultEncoding);
}

Rtf::~Rtf() {
    delete p->reader;
}

void Rtf::parse(RtfTextReader *reader, const QString& rtfMsg, QString *plainText, QString *html)
{
	QByteArray unbased = QByteArray::fromBase64(rtfMsg.toLatin1());
    QByteArray arr;
    quint32 beLen = qToBigEndian(unbased.length()*10);
    arr.append(ByteUtils::toByteArray(beLen));
    arr.append(unbased);
    QByteArray uncompressed = qUncompress(arr);

    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    buf.write(uncompressed);
    buf.seek(0);
    quint32 numLps = ByteUtils::readUint32(buf);

    if (numLps > 1) {
        QByteArray rtfMsg = ByteUtils::readArray(buf);
        QString color = ByteUtils::readString(buf);//not used now
        Q_UNUSED(color);
        reader->readDocument(rtfMsg);
		if (plainText)
			*plainText = reader->getText();
		if (html) {
			QString plainHtml = reader->getHtml();
			int index = plainHtml.indexOf(QLatin1String("<body"));
			int lastIndex = plainHtml.lastIndexOf(QLatin1String("</body"));
			*html = QLatin1String("<span");
			// length of "<body" is equal to 5
			html->append(html->midRef(index + 5, lastIndex - index - 5));
			html->append(QLatin1String("</span>"));
		}
    } else {
		if (plainText)
			plainText->clear();
		if (html)
			html->clear();
	}
}

void Rtf::parse(const QString& rtfMsg, QString *plainText, QString *html)
{
    parse(p->reader, rtfMsg, plainText, html);
}

#endif //NO_RTF_SUPPORT

