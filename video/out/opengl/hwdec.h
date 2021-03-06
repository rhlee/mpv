#ifndef MPGL_HWDEC_H_
#define MPGL_HWDEC_H_

#include "common.h"
#include "video/hwdec.h"

struct gl_hwdec {
    const struct gl_hwdec_driver *driver;
    struct mp_log *log;
    struct mpv_global *global;
    GL *gl;
    struct mp_hwdec_devices *devs;
    // For free use by hwdec driver
    void *priv;
    // For working around the vdpau vs. vaapi mess.
    bool probing;
    // hwdec backends must set this to an IMGFMT_ that has an equivalent
    // internal representation in gl_video.c as the hardware texture.
    // It's used to build the rendering chain. For example, setting it to
    // IMGFMT_RGB0 indicates that the video texture is RGB.
    int converted_imgfmt;
    // Normally this is GL_TEXTURE_2D, but the hwdec driver can set it to
    // GL_TEXTURE_RECTANGLE. This is needed because VideoToolbox is shit.
    GLenum gl_texture_target;
};

struct gl_hwdec_driver {
    // Name of the interop backend. This is used for informational purposes only.
    const char *name;
    // Used to explicitly request a specific API.
    enum hwdec_type api;
    // The hardware surface IMGFMT_ that must be passed to map_image later.
    int imgfmt;
    // Create the hwdec device. It must add it to hw->devs, if applicable.
    // This also must set hw->converted_imgfmt.
    int (*create)(struct gl_hwdec *hw);
    // Prepare for rendering video. (E.g. create textures.)
    // Called on initialization, and every time the video size changes.
    // *params must be set to the format the hw textures return.
    // This also can update hw->converted_imgfmt.
    int (*reinit)(struct gl_hwdec *hw, struct mp_image_params *params);
    // Return textures that contain a copy or reference of the given hw_image.
    int (*map_image)(struct gl_hwdec *hw, struct mp_image *hw_image,
                     GLuint *out_textures);

    void (*destroy)(struct gl_hwdec *hw);
};

struct gl_hwdec *gl_hwdec_load_api(struct mp_log *log, GL *gl,
                                   struct mpv_global *g,
                                   struct mp_hwdec_devices *devs,
                                   enum hwdec_type api);

void gl_hwdec_uninit(struct gl_hwdec *hwdec);

#endif
