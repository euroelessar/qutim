#if !defined RTFTEXTREADER_H && !defined NO_RTF_SUPPORT
#define RTFTEXTREADER_H

#include <rtf/rtfreader.h>

class RtfTextReader : public RtfReader
{
public:
    RtfTextReader(const char *defaultEncoding = "utf-8");
    virtual ~RtfTextReader();
    //new
    QString getText() const;

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
    QString m_result;
    QTextCodec *m_codec;
    QByteArray m_defaultEnc;
};


#endif // RTFTEXTREADER_H, NO_RTF_SUPPORT
