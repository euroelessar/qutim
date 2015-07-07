#include "quickmacwindowbackgroundblur.h"

#include <Carbon/Carbon.h>
#include <QDebug>
#import <Cocoa/Cocoa.h>

#ifndef CG_EXTERN_C_BEGIN
#define CG_EXTERN_C_BEGIN
#define CG_EXTERN_C_END
#endif
#ifndef CG_EXTERN
#define CG_EXTERN
#endif

typedef int CGSConnectionID;
CG_EXTERN_C_BEGIN
CG_EXTERN CGSConnectionID CGSMainConnectionID(void);
typedef void * CGSConnection;
CG_EXTERN_C_END

namespace Core {

QuickMacWindowBackgroundBlur::QuickMacWindowBackgroundBlur(QObject *parent) :
    QObject(parent), m_window(nullptr), m_radius(0)
{
}

QWindow *QuickMacWindowBackgroundBlur::window() const
{
    return m_window;
}

void QuickMacWindowBackgroundBlur::setWindow(QWindow *window)
{
    if (m_window == window)
        return;

    if (m_window)
        forceSetRadius(0);

    m_window = window;
    emit windowChanged(window);

    if (window && m_backgroundColor.isValid())
        forceSetBackgroundColor();
    if (window && m_radius > 0)
        forceSetRadius(m_radius);
}

int QuickMacWindowBackgroundBlur::radius() const
{
    return m_radius;
}

void QuickMacWindowBackgroundBlur::setRadius(int radius)
{
    if (m_radius == radius)
        return;

    m_radius = radius;
    emit radiusChanged(radius);

    if (m_window)
        forceSetRadius(m_radius);
}

QColor QuickMacWindowBackgroundBlur::backgroundColor() const
{
    return m_backgroundColor;
}

void QuickMacWindowBackgroundBlur::setBackgroundColor(const QColor &backgroundColor)
{
    if (m_backgroundColor == backgroundColor)
        return;

    m_backgroundColor = backgroundColor;
    emit backgroundColorChanged(backgroundColor);

    if (m_window)
        forceSetBackgroundColor();
}

void QuickMacWindowBackgroundBlur::forceSetBackgroundColor()
{
    NSView *view = reinterpret_cast<NSView *>(m_window->winId());
    NSWindow *window = view.window;
    window.backgroundColor = [NSColor colorWithSRGBRed:(CGFloat)m_backgroundColor.redF()
            green:(CGFloat)m_backgroundColor.greenF()
             blue:(CGFloat)m_backgroundColor.blueF()
            alpha:(CGFloat)m_backgroundColor.alphaF()];
}

static void *GetFunctionByName(NSString *library, char *func) {
    CFBundleRef bundle;
    CFURLRef bundleURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef) library, kCFURLPOSIXPathStyle, true);
    CFStringRef functionName = CFStringCreateWithCString(kCFAllocatorDefault, func, kCFStringEncodingASCII);
    bundle = CFBundleCreate(kCFAllocatorDefault, bundleURL);
    void *f = NULL;
    if (bundle) {
        f = CFBundleGetFunctionPointerForName(bundle, functionName);
        CFRelease(bundle);
    }
    CFRelease(functionName);
    CFRelease(bundleURL);
    return f;
}

typedef int CGSWindowID;
typedef CGError CGSSetWindowBackgroundBlurRadiusFunction(CGSConnectionID cid, CGSWindowID wid, NSUInteger blur);

static CGSSetWindowBackgroundBlurRadiusFunction *GetCGSSetWindowBackgroundBlurRadiusFunction()
{
    static BOOL tried = NO;
    static CGSSetWindowBackgroundBlurRadiusFunction *function = NULL;
    if (!tried) {
        QByteArray functionName = "CGSSetWindowBackgroundBlurRadius";
        function = reinterpret_cast<CGSSetWindowBackgroundBlurRadiusFunction *>(
                    GetFunctionByName(@"/System/Library/Frameworks/ApplicationServices.framework",
                                      functionName.data())
                    );
        tried = YES;
    }
    return function;
}

void QuickMacWindowBackgroundBlur::forceSetRadius(int radius)
{
    CGSConnectionID con = CGSMainConnectionID();
    if (!con) {
        return;
    }
    NSView *view = reinterpret_cast<NSView *>(m_window->winId());
    NSWindow *window = view.window;

    [window setOpaque:NO];
    if (auto function = GetCGSSetWindowBackgroundBlurRadiusFunction())
        function(con, [window windowNumber], (int)radius);
}

} // namespace Core
