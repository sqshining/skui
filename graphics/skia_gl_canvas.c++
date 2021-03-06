/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Ruben Van Boxem
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 **/

#include "graphics/skia_gl_canvas.h++"

#include <GrBackendSurface.h>
#include <GrContext.h>
#include <GrGLInterface.h>

#include <SkCanvas.h>
#include <SkColor.h>
#include <SkGradientShader.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glext.h>

namespace skui
{
  namespace graphics
  {
    namespace implementation
    {
      sk_sp<SkSurface> create_gl_surface(const pixel_size& size,
                                         const GrGLInterface& gl_interface,
                                         GrContext& gr_context)
      {
        // Wrap the frame buffer object attached to the screen in a Skia render target so Skia can
        // render to it
        GrGLFramebufferInfo framebuffer_info;
        GrGLint buffer;
        gl_interface.fFunctions.fGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);
        framebuffer_info.fFBOID = static_cast<GrGLuint>(buffer);


        GrBackendRenderTarget render_target(static_cast<int>(size.width),
                                            static_cast<int>(size.height),
                                            0,
                                            8,
                                            kSkia8888_GrPixelConfig,
                                            framebuffer_info);

        // setup SkSurface
        SkSurfaceProps props(SkSurfaceProps::kUseDeviceIndependentFonts_Flag, // distance field text
                              SkSurfaceProps::kLegacyFontHost_InitType);

        return SkSurface::MakeFromBackendRenderTarget(&gr_context, render_target, kBottomLeft_GrSurfaceOrigin, nullptr, &props);
      }
    }

    skia_gl_canvas::skia_gl_canvas(const pixel_size& size,
                                const GrGLInterface& gr_gl_interface,
                                canvas_flags flags)
      : skia_canvas(0.5f, flags)
      , gr_context(GrContext::Create(kOpenGL_GrBackend, reinterpret_cast<GrBackendContext>(&gr_gl_interface)))
    {
      surface = implementation::create_gl_surface(size, gr_gl_interface, *gr_context);
      SkASSERT(gr_context);
    }
  }
}
