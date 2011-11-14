/****************************************************************************
**
** qutIM - instant messenger
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
#if !defined RTFTEXTREADER_H && !defined NO_RTF_SUPPORT
#define RTFTEXTREADER_H

#include "rtfreader.h"
#include <QTextDocument>
#include <QTextCursor>

class RtfTextReader : public RtfReader
{
public:
    RtfTextReader(const char *defaultEncoding = "utf-8");
    virtual ~RtfTextReader();
    //new
    QString getText() const;
	QString getHtml() const;

protected:
    //inherited
    virtual void addCharData(const char *data, size_t len, bool convert);
    virtual void insertImage(const std::string &mimeType, const std::string &fileName, size_t startOffset, size_t size);
    virtual void setEncoding(int code);
    virtual void switchDestination(DestinationType destination, bool on);
    virtual void setAlignment();
    virtual void setFontProperty(FontProperty property);
    virtual void newParagraph();
    virtual void flushBuffers();

private:
	QTextDocument m_document;
	QTextCursor m_cursor;
//    QString m_result;
    QTextCodec *m_codec;
    QByteArray m_defaultEnc;
};


#endif // RTFTEXTREADER_H, NO_RTF_SUPPORT
