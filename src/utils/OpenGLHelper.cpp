/******************************************************************************
    QtAV:  Media play library based on Qt and FFmpeg
    Copyright (C) 2012-2015 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

#include "OpenGLHelper.h"
#include <string.h> //strstr
#include <QtCore/QCoreApplication>
#include <QtGui/QMatrix4x4>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
#include <QtOpenGL/QGLFunctions>
#endif
#else
#include <QtGui/QGuiApplication>
#endif
#ifdef QT_OPENGL_DYNAMIC
#include <QtGui/QOpenGLFunctions_1_0>
#endif
#if QTAV_HAVE(EGL_CAPI)  && QTAV_HAVE(QT_EGL) //make sure no crash if no egl library
#define EGL_CAPI_NS
#include "capi/egl_api.h"
#endif //QTAV_HAVE(EGL_CAPI)
#include "utils/Logger.h"

namespace QtAV {
namespace OpenGLHelper {

// glGetTexParameteriv is supported by es2 does not support GL_TEXTURE_INTERNAL_FORMAT.
// glGetTexLevelParameteriv is supported by es3.1
static void glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params)
{
#ifdef QT_OPENGL_DYNAMIC
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx)
        return;
    if (isOpenGLES()) {
        typedef void (QOPENGLF_APIENTRYP glGetTexLevelParameteriv_t)(GLenum, GLint, GLenum, GLint*);
        static glGetTexLevelParameteriv_t GetTexLevelParameteriv = 0;
        if (!GetTexLevelParameteriv) {
            GetTexLevelParameteriv = (glGetTexLevelParameteriv_t)ctx->getProcAddress("glGetTexLevelParameteriv");
#ifdef Q_OS_WIN
            if (!GetTexLevelParameteriv) {
                qDebug("resolve glGetTexLevelParameteriv from dso");
                GetTexLevelParameteriv = (glGetTexLevelParameteriv_t)GetProcAddress((HMODULE)QOpenGLContext::openGLModuleHandle(), "glGetTexLevelParameteriv");
            }
#endif
            if (!GetTexLevelParameteriv) {
                qWarning("can not resolve glGetTexLevelParameteriv");
                return;
            }
        }
        GetTexLevelParameteriv(target, level, pname, params);
        return;
    }
    QOpenGLFunctions_1_0* f = 0;
    f = ctx->versionFunctions<QOpenGLFunctions_1_0>();
    if (!f)
        return;
    f->glGetTexLevelParameteriv(target, level, pname, params);
#else
#if defined(GL_ES_VERSION_3_1)
    ::glGetTexLevelParameteriv(target, level, pname, params);
#elif defined(GL_ES_VERSION_2_0) //also defined in es3.0
    Q_UNUSED(target);
    Q_UNUSED(level);
    Q_UNUSED(pname);
    Q_UNUSED(params);
    qDebug("OpenGL ES2 and 3.0 does not support glGetTexLevelParameteriv");
#elif !defined(QT_OPENGL_ES)
    ::glGetTexLevelParameteriv(target, level, pname, params);
#endif //GL_ES_VERSION_3_1
#endif
}

int depth16BitTexture()
{
    static int depth = qgetenv("QTAV_TEXTURE16_DEPTH").toInt() == 16 ? 16 : 8;//8 ? 8 : 16;
    return depth;
}

bool useDeprecatedFormats()
{
    static int v = qgetenv("QTAV_GL_DEPRECATED").toInt() == 1;
    return v;
}

int GLSLVersion()
{
    static int v = -1;
    if (v >= 0)
        return v;
    if (!QOpenGLContext::currentContext()) {
        qWarning("%s: current context is null", __FUNCTION__);
        return 0;
    }
    const char* vs = (const char*)DYGL(glGetString(GL_SHADING_LANGUAGE_VERSION));
    int major = 0, minor = 0;
    if (sscanf(vs, "%d.%d", &major, &minor) == 2)
        v = major * 100 + minor;
    else
        v = 0;
    qDebug("GLSL version: %s/%d", vs, v);
    return v;
}

bool isEGL()
{
    static int is_egl = -1;
    if (is_egl >= 0)
        return !!is_egl;
    if (isOpenGLES()) { //TODO: ios has no egl
        is_egl = 1;
        return true;
    }
#if QTAV_HAVE(EGL_CAPI) && QTAV_HAVE(QT_EGL) //make sure no crash if no egl library
    if (eglGetCurrentDisplay() != EGL_NO_DISPLAY) {
        is_egl = 1;
        return true;
    }
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (QGuiApplication::platformName().contains(QLatin1String("egl"))) {
        is_egl = 1;
        return true;
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    if (QGuiApplication::platformName().contains(QLatin1String("xcb"))) {
        is_egl = qgetenv("QT_XCB_GL_INTEGRATION") == "xcb_egl";
        qDebug("xcb_egl=%d", is_egl);
        return !!is_egl;
    }
#endif //5.5.0
#endif
    // we can use QOpenGLContext::currentContext()->nativeHandle().value<QEGLNativeContext>(). but gl context is required
    if (QOpenGLContext::currentContext())
        is_egl = 0;
    return false;
}

bool isOpenGLES()
{
#ifdef QT_OPENGL_DYNAMIC
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    // desktop can create es compatible context
    return qApp->testAttribute(Qt::AA_UseOpenGLES) || (ctx ? ctx->isOpenGLES() : QOpenGLContext::openGLModuleType() != QOpenGLContext::LibGL); //
#endif //QT_OPENGL_DYNAMIC
#ifdef QT_OPENGL_ES_2
    return true;
#endif //QT_OPENGL_ES_2
#if defined(QT_OPENGL_ES_2_ANGLE_STATIC) || defined(QT_OPENGL_ES_2_ANGLE)
    return true;
#endif //QT_OPENGL_ES_2_ANGLE_STATIC
    return false;
}

bool hasExtensionEGL(const char *exts[])
{
#if QTAV_HAVE(EGL_CAPI) && QTAV_HAVE(QT_EGL) //make sure no crash if no egl library
    static QList<QByteArray> supported;
    if (supported.isEmpty()) {
        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglInitialize(display, NULL, NULL);
        supported = QByteArray(eglQueryString(display, EGL_EXTENSIONS)).split(' ');
    }
    static bool print_exts = true;
    if (print_exts) {
        print_exts = false;
        qDebug() << "EGL extensions: " << supported;
    }
    for (int i = 0; exts[i]; ++i) {
        if (supported.contains(QByteArray(exts[i])))
            return true;
    }
#endif
    return false;
}

bool hasExtension(const char *exts[])
{
    const QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        qWarning("no gl context for hasExtension");
        return false;
    }
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    const char *ext = (const char*)glGetString(GL_EXTENSIONS);
    if (!ext)
        return false;
#endif
    for (int i = 0; exts[i]; ++i) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        if (ctx->hasExtension(exts[i]))
#else
        if (strstr(ext, exts[i]))
#endif
            return true;
    }
    return false;
}

bool isPBOSupported() {
    // check pbo support
    static bool support = false;
    static bool pbo_checked = false;
    if (pbo_checked)
        return support;
    const QOpenGLContext *ctx = QOpenGLContext::currentContext();
    Q_ASSERT(ctx);
    if (!ctx)
        return false;
    const char* exts[] = {
        "GL_ARB_pixel_buffer_object",
        "GL_EXT_pixel_buffer_object",
        "GL_NV_pixel_buffer_object", //OpenGL ES
        NULL
    };
    support = hasExtension(exts);
    qDebug() << "PBO: " << support;
    pbo_checked = true;
    return support;
}

// glActiveTexture in Qt4 on windows release mode crash for me
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#ifndef QT_OPENGL_ES
// APIENTRY may be not defined(why? linux es2). or use QOPENGLF_APIENTRY
// use QGLF_APIENTRY for Qt4 crash, why? APIENTRY is defined in windows header
#ifndef APIENTRY
// QGLF_APIENTRY is in Qt4,8+
#if defined(QGLF_APIENTRY)
#define APIENTRY QGLF_APIENTRY
#elif defined(GL_APIENTRY)
#define APIENTRY GL_APIENTRY
#endif //QGLF_APIENTRY
#endif //APIENTRY
typedef void (APIENTRY *type_glActiveTexture) (GLenum);
static type_glActiveTexture qtav_glActiveTexture = 0;

static void qtavResolveActiveTexture()
{
    const QGLContext *context = QGLContext::currentContext();
    qtav_glActiveTexture = (type_glActiveTexture)context->getProcAddress(QLatin1String("glActiveTexture"));
    if (!qtav_glActiveTexture) {
        qDebug("resolve glActiveTextureARB");
        qtav_glActiveTexture = (type_glActiveTexture)context->getProcAddress(QLatin1String("glActiveTextureARB"));
    }
    //Q_ASSERT(qtav_glActiveTexture);
}
#endif //QT_OPENGL_ES
#endif //QT_VERSION

void glActiveTexture(GLenum texture)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#ifndef QT_OPENGL_ES
    if (!qtav_glActiveTexture)
        qtavResolveActiveTexture();
    if (!qtav_glActiveTexture)
        return;
    qtav_glActiveTexture(texture);
#else
    ::glActiveTexture(texture);
#endif //QT_OPENGL_ES
#else
    QOpenGLContext::currentContext()->functions()->glActiveTexture(texture);
#endif
}

typedef struct {
    GLint internal_format;
    GLenum format;
    GLenum type;
} gl_param_t;

// es formats:  ALPHA, RGB, RGBA, LUMINANCE, LUMINANCE_ALPHA
// es types:  UNSIGNED_BYTE, UNSIGNED_SHORT_5_6_5, UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_5_5_5_1 (NO UNSIGNED_SHORT)
/*!
  c: number of channels(components) in the plane
  b: componet size
    result is gl_param_compat[(c-1)+4*(b-1)]
*/
static const gl_param_t gl_param_compat[] = { // it's legacy
    { GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE},
    { GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE},
    { GL_RGB, GL_RGB, GL_UNSIGNED_BYTE},
    { GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE},
    { GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE}, //2 x 8 fallback to ra
    {0,0,0},
};
static const gl_param_t gl_param_desktop[] = {
    {GL_R8,     GL_RED,     GL_UNSIGNED_BYTE},      // 1 x 8
    {GL_RG,      GL_RG,      GL_UNSIGNED_BYTE},      // 2 x 8
    {GL_RGB,     GL_RGB,     GL_UNSIGNED_BYTE},      // 3 x 8
    {GL_RGBA,    GL_RGBA,    GL_UNSIGNED_BYTE},      // 4 x 8
    {GL_R16,     GL_RED,     GL_UNSIGNED_SHORT},     // 1 x 16
    {GL_RG16,    GL_RG,      GL_UNSIGNED_SHORT},     // 2 x 16
    {GL_RGB16,   GL_RGB,     GL_UNSIGNED_SHORT},     // 3 x 16
    {GL_RGBA16,  GL_RGBA,    GL_UNSIGNED_SHORT},     // 4 x 16
    {0,0,0},
    {GL_RGB10_A2, GL_RGBA,   GL_UNSIGNED_INT_2_10_10_10_REV},         // 4 x 8
};
static const gl_param_t gl_param_desktop_fallback[] = {
    {GL_RED,     GL_RED,     GL_UNSIGNED_BYTE},      // 1 x 8
    {GL_RG,      GL_RG,      GL_UNSIGNED_BYTE},      // 2 x 8
    {GL_RGB,     GL_RGB,     GL_UNSIGNED_BYTE},      // 3 x 8
    {GL_RGBA,    GL_RGBA,    GL_UNSIGNED_BYTE},      // 4 x 8
    {GL_RG,      GL_RG,      GL_UNSIGNED_BYTE},     // 2 x 8
    {0,0,0},
};
static const gl_param_t gl_param_es3[] = {
    {GL_R8,       GL_RED,    GL_UNSIGNED_BYTE},      // 1 x 8
    {GL_RG8,      GL_RG,     GL_UNSIGNED_BYTE},      // 2 x 8
    {GL_RGB8,     GL_RGB,    GL_UNSIGNED_BYTE},      // 3 x 8
    {GL_RGBA8,    GL_RGBA,   GL_UNSIGNED_BYTE},      // 4 x 8
    {GL_RG8,      GL_RG,     GL_UNSIGNED_BYTE},      // 2 x 8 fallback to rg
    {0,0,0},
};
//https://www.khronos.org/registry/gles/extensions/EXT/EXT_texture_rg.txt
// supported by ANGLE+D3D11
static const gl_param_t gl_param_es2rg[] = {
    {GL_RED,     GL_RED,    GL_UNSIGNED_BYTE},      // 1 x 8 //es2: GL_EXT_texture_rg. R8, RG8 are for render buffer
    {GL_RG,      GL_RG,     GL_UNSIGNED_BYTE},      // 2 x 8
    {GL_RGB,     GL_RGB,    GL_UNSIGNED_BYTE},      // 3 x 8
    {GL_RGBA,    GL_RGBA,   GL_UNSIGNED_BYTE},      // 4 x 8
    {GL_RG,      GL_RG,     GL_UNSIGNED_BYTE},      // 2 x 8 fallback to rg
    {0,0,0},
};

bool test_gl_param(const gl_param_t& gp, bool* has_16 = 0)
{
    if (!QOpenGLContext::currentContext()) {
        qWarning("%s: current context is null", __FUNCTION__);
        return false;
    }
    GLuint tex;
    DYGL(glGenTextures(1, &tex));
    DYGL(glBindTexture(GL_TEXTURE_2D, tex));
    DYGL(glTexImage2D(GL_TEXTURE_2D, 0, gp.internal_format, 64, 64, 0, gp.format, gp.type, NULL));
    GLint param = 0;
    //GL_PROXY_TEXTURE_2D and no glGenTextures?
#ifndef GL_TEXTURE_INTERNAL_FORMAT //only in desktop
#define GL_TEXTURE_INTERNAL_FORMAT 0x1003
#endif
    OpenGLHelper::glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &param);
    // TODO: check glGetError()?
    if (param != gp.internal_format) {
        qDebug("Do not support texture internal format: %#x", gp.internal_format);
        DYGL(glDeleteTextures(1, &tex));
        return false;
    }
    if (!has_16) {
        DYGL(glDeleteTextures(1, &tex));
        return true;
    }
    *has_16 = false;
    GLenum pname = 0;
#ifndef GL_TEXTURE_RED_SIZE
#define GL_TEXTURE_RED_SIZE 0x805C
#endif
#ifndef GL_TEXTURE_LUMINANCE_SIZE
#define GL_TEXTURE_LUMINANCE_SIZE 0x8060
#endif
    switch (gp.format) {
    case GL_RED:        pname = GL_TEXTURE_RED_SIZE; break;
    case GL_LUMINANCE:  pname = GL_TEXTURE_LUMINANCE_SIZE; break;
    }
    param = 0;
    if (pname)
        OpenGLHelper::glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, pname, &param);
    if (param) {
        qDebug("16 bit texture depth: %d.\n", (int)param);
        *has_16 = (int)param == 16;
    }
    DYGL(glDeleteTextures(1, &tex));
    return true;
}

bool hasRG()
{
    static int has_rg = -1;
    if (has_rg >= 0)
        return !!has_rg;
    if (test_gl_param(gl_param_desktop[1])) {
        has_rg = 1;
        return true;
    }
    if (test_gl_param(gl_param_es3[1])) {
        has_rg = 1;
        return true;
    }
    static const char* ext[] = { "GL_EXT_texture_rg", 0}; //RED, RG, R8, RG8
    if (hasExtension(ext)) {
        qDebug("has extension GL_EXT_texture_rg");
        has_rg = 1;
        return true;
    }
    if (QOpenGLContext::currentContext())
        has_rg = isOpenGLES() && QOpenGLContext::currentContext()->format().majorVersion() > 2;
    return has_rg;
}

static int has_16_tex = -1;
static const gl_param_t* get_gl_param()
{
    if (!QOpenGLContext::currentContext()) {
        qWarning("%s: current context is null", __FUNCTION__);
        return gl_param_compat;
    }
    static gl_param_t* gp = 0;
    if (gp)
        return gp;

    //XUNO
    bool has_10 = false;
    // [9] RGB10_A2
    if (test_gl_param(gl_param_desktop[9], &has_10)) {
        qDebug()<<"OpenGLHelper.cpp : test RGB10_A2"<<has_10;
    }
    //XUNO

    bool has_16 = false;
    // [4] is always available
    if (test_gl_param(gl_param_desktop[4], &has_16)) {
        if (has_16 && depth16BitTexture() == 16)
            gp = (gl_param_t*)gl_param_desktop;
        else
            gp = (gl_param_t*)gl_param_desktop_fallback;
        has_16_tex = has_16;
        if (!useDeprecatedFormats()) {
            qDebug("using gl_param_desktop%s", gp == gl_param_desktop? "" : "_fallback");
            return gp;
        }
    } else if (test_gl_param(gl_param_es3[4], &has_16)) { //3.0 will fail because no glGetTexLevelParameteriv
        gp = (gl_param_t*)gl_param_es3;
        has_16_tex = has_16;
        if (!useDeprecatedFormats()) {
            qDebug("using gl_param_es3");
            return gp;
        }
    } else if (isOpenGLES()) {
        if (QOpenGLContext::currentContext()->format().majorVersion() > 2)
            gp = (gl_param_t*)gl_param_es3; //for 3.0
        else if (hasRG())
            gp = (gl_param_t*)gl_param_es2rg;
        has_16_tex = has_16;
        if (gp && !useDeprecatedFormats()) {
            qDebug("using gl_param_%s", gp == gl_param_es3 ? "es3" : "es2rg");
            return gp;
        }
    }
    qDebug("fallback to gl_param_compat");
    gp = (gl_param_t*)gl_param_compat;
    has_16_tex = false;
    return gp;
}

bool has16BitTexture()
{
    if (has_16_tex >= 0)
        return !!has_16_tex;
    if (!QOpenGLContext::currentContext()) {
        qWarning("%s: current context is null", __FUNCTION__);
        return false;
    }
    get_gl_param();
    return !!has_16_tex;
}

typedef struct {
    VideoFormat::PixelFormat pixfmt;
    quint8 channels[4];
} reorder_t;
// use with gl_param_compat
static const reorder_t gl_channel_maps[] = {
    { VideoFormat::Format_ARGB32, {1, 2, 3, 0}},
    { VideoFormat::Format_ABGR32, {3, 2, 1, 0}}, // R->gl.?(a)->R
    { VideoFormat::Format_BGR24,  {2, 1, 0, 3}},
    { VideoFormat::Format_BGR565, {2, 1, 0, 3}},
    { VideoFormat::Format_BGRA32, {2, 1, 0, 3}},
    { VideoFormat::Format_BGR32,  {2, 1, 0, 3}},
    { VideoFormat::Format_BGR48LE,{2, 1, 0, 3}},
    { VideoFormat::Format_BGR48BE,{2, 1, 0, 3}},
    { VideoFormat::Format_BGR48,  {2, 1, 0, 3}},
    { VideoFormat::Format_BGR555, {2, 1, 0, 3}},
    { VideoFormat::Format_Invalid,{1, 2, 3}}
};

static QMatrix4x4 channelMap(const VideoFormat& fmt)
{
    if (fmt.isPlanar()) //currently only for planar
        return QMatrix4x4();
    switch (fmt.pixelFormat()) {
    case VideoFormat::Format_UYVY:
        return QMatrix4x4(0.0f, 0.5f, 0.0f, 0.5f,
                                 1.0f, 0.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f);
    case VideoFormat::Format_YUYV:
        return QMatrix4x4(0.5f, 0.0f, 0.5f, 0.0f,
                                 0.0f, 1.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f);
    case VideoFormat::Format_VYUY:
        return QMatrix4x4(0.0f, 0.5f, 0.0f, 0.5f,
                                 0.0f, 0.0f, 1.0f, 0.0f,
                                 1.0f, 0.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f);
    case VideoFormat::Format_YVYU:
        return QMatrix4x4(0.5f, 0.0f, 0.5f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f,
                                 0.0f, 1.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f);
    default:
        break;
    }

    const quint8 *channels = NULL;//{ 0, 1, 2, 3};
    for (int i = 0; gl_channel_maps[i].pixfmt != VideoFormat::Format_Invalid; ++i) {
        if (gl_channel_maps[i].pixfmt == fmt.pixelFormat()) {
            channels = gl_channel_maps[i].channels;
            break;
        }
    }
    QMatrix4x4 m;
    if (!channels)
        return m;
    m.fill(0);
    for (int i = 0; i < 4; ++i) {
        m(i, channels[i]) = 1;
    }
    qDebug() << m;
    return m;
}

//template<typename T, size_t N> Q_CONSTEXPR size_t array_size(const T (&)[N]) { return N;} //does not support local type if no c++11
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
bool videoFormatToGL(const VideoFormat& fmt, GLint* internal_format, GLenum* data_format, GLenum* data_type, QMatrix4x4* mat)
{
    typedef struct fmt_entry {
        VideoFormat::PixelFormat pixfmt;
        GLint internal_format;
        GLenum format;
        GLenum type;
    } fmt_entry;
    static const fmt_entry pixfmt_to_gles[] = {
        {VideoFormat::Format_BGRA32, GL_BGRA, GL_BGRA, GL_UNSIGNED_BYTE }, //tested for angle
        {VideoFormat::Format_RGB32,  GL_BGRA, GL_BGRA, GL_UNSIGNED_BYTE },
        {VideoFormat::Format_Invalid, 0, 0, 0}
    };
    Q_UNUSED(pixfmt_to_gles);
    static const fmt_entry pixfmt_to_desktop[] = {
        {VideoFormat::Format_BGRA32, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE }, //bgra bgra works on win but not osx
        {VideoFormat::Format_RGB32,  GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE }, //FIXMEL endian check
        //{VideoFormat::Format_BGRA32,  GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE }, //{2,1,0,3}
        //{VideoFormat::Format_BGR24,   GL_RGB,  GL_BGR,  GL_UNSIGNED_BYTE }, //{0,1,2,3}
    #ifdef GL_UNSIGNED_SHORT_5_6_5_REV
        {VideoFormat::Format_BGR565, GL_RGB,  GL_RGB,  GL_UNSIGNED_SHORT_5_6_5_REV}, // es error, use channel map
    #endif
    #ifdef GL_UNSIGNED_SHORT_1_5_5_5_REV
        {VideoFormat::Format_RGB555, GL_RGBA, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV},
    #endif
    #ifdef GL_UNSIGNED_SHORT_1_5_5_5_REV
        {VideoFormat::Format_BGR555, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV},
    #endif
        // TODO: BE formats not implemeted
        {VideoFormat::Format_RGB48, GL_RGB, GL_RGB, GL_UNSIGNED_SHORT }, //TODO: rgb16?
        {VideoFormat::Format_RGB48LE, GL_RGB, GL_RGB, GL_UNSIGNED_SHORT },
        {VideoFormat::Format_RGB48BE, GL_RGB, GL_RGB, GL_UNSIGNED_SHORT },
        {VideoFormat::Format_BGR48, GL_RGB, GL_BGR, GL_UNSIGNED_SHORT }, //RGB16?
        {VideoFormat::Format_BGR48LE, GL_RGB, GL_BGR, GL_UNSIGNED_SHORT },
        {VideoFormat::Format_BGR48BE, GL_RGB, GL_BGR, GL_UNSIGNED_SHORT },
        {VideoFormat::Format_RGBA64LE, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT },
        {VideoFormat::Format_RGBA64BE, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT },
        {VideoFormat::Format_BGRA64LE, GL_RGBA, GL_BGRA, GL_UNSIGNED_SHORT },
        {VideoFormat::Format_BGRA64BE, GL_RGBA, GL_BGRA, GL_UNSIGNED_SHORT },
        {VideoFormat::Format_Invalid, 0, 0, 0}
    };
    Q_UNUSED(pixfmt_to_desktop);
    const fmt_entry *pixfmt_gl_entry = pixfmt_to_desktop;
    if (OpenGLHelper::isOpenGLES())
        pixfmt_gl_entry = pixfmt_to_gles;
    // Very special formats, for which OpenGL happens to have direct support
    static const fmt_entry pixfmt_gl_base[] = {
        {VideoFormat::Format_RGBA32, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE }, // only tested for osx, win, angle
        {VideoFormat::Format_RGB24,  GL_RGB,  GL_RGB,  GL_UNSIGNED_BYTE },
        {VideoFormat::Format_RGB565, GL_RGB,  GL_RGB,  GL_UNSIGNED_SHORT_5_6_5},
        {VideoFormat::Format_BGR32,  GL_BGRA, GL_BGRA, GL_UNSIGNED_BYTE }, //rgba(tested) or abgr, depending on endian
    };
    const VideoFormat::PixelFormat pixfmt = fmt.pixelFormat();
    // can not use array size because pixfmt_gl_entry is set on runtime
    for (const fmt_entry* e = pixfmt_gl_entry; e->pixfmt != VideoFormat::Format_Invalid; ++e) {
        if (e->pixfmt == pixfmt) {
            *internal_format = e->internal_format;
            *data_format = e->format;
            *data_type = e->type;
            if (mat)
                *mat = QMatrix4x4();
            return true;
        }
    }
    for (size_t i = 0; i < ARRAY_SIZE(pixfmt_gl_base); ++i) {
        const fmt_entry& e = pixfmt_gl_base[i];
        if (e.pixfmt == pixfmt) {
            *internal_format = e.internal_format;
            *data_format = e.format;
            *data_type = e.type;
            if (mat)
                *mat = QMatrix4x4();
            return true;
        }
    }
    static const fmt_entry pixfmt_to_gl_swizzele[] = {
        {VideoFormat::Format_UYVY, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },
        {VideoFormat::Format_YUYV, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },
        {VideoFormat::Format_VYUY, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },
        {VideoFormat::Format_YVYU, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },
        {VideoFormat::Format_BGR565, GL_RGB,  GL_RGB,  GL_UNSIGNED_SHORT_5_6_5}, //swizzle
        {VideoFormat::Format_RGB555, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1}, //not working
        {VideoFormat::Format_BGR555, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1}, //not working
    };
    for (size_t i = 0; i < ARRAY_SIZE(pixfmt_to_gl_swizzele); ++i) {
        const fmt_entry& e = pixfmt_to_gl_swizzele[i];
        if (e.pixfmt == pixfmt) {
            *internal_format = e.internal_format;
            *data_format = e.format;
            *data_type = e.type;
            if (mat)
                *mat = channelMap(fmt);
            return true;
        }
    }
    GLint *i_f = internal_format;
    GLenum *d_f = data_format;
    GLenum *d_t = data_type;
    gl_param_t* gp = (gl_param_t*)get_gl_param();
    if (gp == gl_param_desktop && fmt.planeCount() == 2) {
        gp = (gl_param_t*)gl_param_desktop_fallback; // nv12 UV plane is 16bit, but we use rg
        qDebug("desktop_fallback for bi-plane format");
    }
    for (int p = 0; p < fmt.planeCount(); ++p) {
        // for packed rgb(swizzle required) and planar formats
        const int c = (fmt.channels(p)-1) + 4*((fmt.bitsPerComponent() + 7)/8 - 1);
        if (gp[c].format == 0)
            return false;
        const gl_param_t& f = gp[c];
        *(i_f++) = f.internal_format;
        *(d_f++) = f.format;
        *(d_t++) = f.type;
    }
    if (mat)
        *mat = channelMap(fmt);
    return true;
}

// TODO: format + datatype? internal format == format?
//https://www.khronos.org/registry/gles/extensions/EXT/EXT_texture_format_BGRA8888.txt
int bytesOfGLFormat(GLenum format, GLenum dataType)
{
    int component_size = 0;
    switch (dataType) {
#ifdef GL_UNSIGNED_INT_8_8_8_8_REV
    case GL_UNSIGNED_INT_8_8_8_8_REV:
        return 4;
#endif
#ifdef GL_UNSIGNED_BYTE_3_3_2
    case GL_UNSIGNED_BYTE_3_3_2:
        return 1;
#endif //GL_UNSIGNED_BYTE_3_3_2
#ifdef GL_UNSIGNED_BYTE_2_3_3_REV
    case GL_UNSIGNED_BYTE_2_3_3_REV:
            return 1;
#endif
        case GL_UNSIGNED_SHORT_5_5_5_1:
#ifdef GL_UNSIGNED_SHORT_1_5_5_5_REV
        case GL_UNSIGNED_SHORT_1_5_5_5_REV:
#endif //GL_UNSIGNED_SHORT_1_5_5_5_REV
#ifdef GL_UNSIGNED_SHORT_5_6_5_REV
        case GL_UNSIGNED_SHORT_5_6_5_REV:
#endif //GL_UNSIGNED_SHORT_5_6_5_REV
    case GL_UNSIGNED_SHORT_5_6_5: // gles
#ifdef GL_UNSIGNED_SHORT_4_4_4_4_REV
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
#endif //GL_UNSIGNED_SHORT_4_4_4_4_REV
    case GL_UNSIGNED_SHORT_4_4_4_4:
        return 2;
    case GL_UNSIGNED_BYTE:
        component_size = 1;
        break;
        // mpv returns 2
#ifdef GL_UNSIGNED_SHORT_8_8_APPLE
    case GL_UNSIGNED_SHORT_8_8_APPLE:
    case GL_UNSIGNED_SHORT_8_8_REV_APPLE:
        return 2;
#endif
    case GL_UNSIGNED_SHORT:
        component_size = 2;
        break;
    }
    switch (format) {
    case GL_RED:
    case GL_R8:
    case GL_R16: //?
        return component_size;
    case GL_RG:
    case GL_RG8:
    case GL_RG16: //?
        return 2*component_size;
#ifdef GL_YCBCR_422_APPLE
      case GL_YCBCR_422_APPLE:
        return 2;
#endif
#ifdef GL_RGB_422_APPLE
      case GL_RGB_422_APPLE:
        return 2;
#endif
#ifdef GL_BGRA //ifndef GL_ES
      case GL_BGRA:
#endif
    case GL_RGBA:
    case GL_RGBA8:
    case GL_RGBA16:
        return 4*component_size;
#ifdef GL_BGR //ifndef GL_ES
      case GL_BGR:
#endif
    case GL_RGB:
    case GL_RGB8:
    case GL_RGB16:
        return 3*component_size;
      case GL_LUMINANCE_ALPHA:
        // mpv returns 2
        return 2*component_size;
      case GL_LUMINANCE:
      case GL_ALPHA:
        return 1*component_size;
#ifdef GL_LUMINANCE16
    case GL_LUMINANCE16:
        return 2*component_size;
#endif //GL_LUMINANCE16
#ifdef GL_ALPHA16
    case GL_ALPHA16:
        return 2*component_size;
#endif //GL_ALPHA16
      default:
        qWarning("bytesOfGLFormat - Unknown format %u", format);
        return 1;
      }
}

} //namespace OpenGLHelper
} //namespace QtAV
