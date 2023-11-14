#include "framebuffer.h"

static void attach_color_texture(GLuint id, GLint type, s32 width, s32 height, s32 index) {
    GLint internal;
    if (type == GL_RGBA) {
        internal = GL_RGBA8;
    } else if (type == GL_RED_INTEGER) {
        internal = GL_R32I;
    } else {
        LogFatal("Specified invalid color attachment type for framebuffer");
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, type, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, id, 0);
}

static void attach_depth_texture(GLuint id, GLint type, s32 width, s32 height) {
    glTexStorage2D(GL_TEXTURE_2D, 1, type, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, id, 0);
}

Framebuffer *CreateFramebuffer(s32 width, s32 height, Array<GLint> attachments) {
    // TODO: not use new - replace stl containers with custom ones
    Framebuffer *fb = new Framebuffer;
    fb->width = width;
    fb->height = height;
    
    for (GLint attachment : attachments) {
        if (attachment == GL_DEPTH24_STENCIL8) {
            fb->depth_attachment_type = attachment;
        } else {
            fb->color_attachments_types.push_back(attachment);
        }
    }

    Reload(fb);

    return fb;
}

void DestroyFramebuffer(Framebuffer *fb) {
    glDeleteFramebuffers(1, &fb->id);
    glDeleteTextures(fb->color_attachments.size(), fb->color_attachments.data());
    glDeleteTextures(1, &fb->depth_attachment);

    // TODO: not use delete - replace stl containers with custom ones
    delete fb;
}

void Reload(Framebuffer *fb) {
    if (fb->id) {
        glDeleteFramebuffers(1, &fb->id);
        glDeleteTextures(fb->color_attachments.size(), fb->color_attachments.data());
        glDeleteTextures(1, &fb->depth_attachment);

        fb->color_attachments.clear();
        fb->depth_attachment = 0;
    }

    glGenFramebuffers(1, &fb->id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->id);

    if (!fb->color_attachments_types.empty()) {
        fb->color_attachments.resize(fb->color_attachments_types.size());
        glGenTextures(fb->color_attachments.size(), fb->color_attachments.data());

        for (s32 i = 0; i < fb->color_attachments.size(); ++i) {
            glBindTexture(GL_TEXTURE_2D, fb->color_attachments[i]);
            attach_color_texture(fb->color_attachments[i], fb->color_attachments_types[i], fb->width, fb->height, i);
        }
    }

    if (fb->depth_attachment_type) {
        glGenTextures(1, &fb->depth_attachment);
        glBindTexture(GL_TEXTURE_2D, fb->depth_attachment);

        attach_depth_texture(fb->depth_attachment, fb->depth_attachment_type, fb->width, fb->height);
    }

    if (fb->color_attachments.size() > 1) {
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(fb->color_attachments.size(), buffers);
    } else if (fb->color_attachments.empty()) {
        glDrawBuffer(GL_NONE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bind(Framebuffer *fb) {
    glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
    glViewport(0, 0, fb->width, fb->height);
}

void Unbind(Framebuffer *fb) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Resize(Framebuffer *fb, s32 width, s32 height) {
    fb->width = width;
    fb->height = height;

    Reload(fb);
}

GLuint GetAttachment(Framebuffer *fb, u32 index) {
    return fb->color_attachments[index];
}

s32 Read(Framebuffer *fb, u32 index, s32 x, s32 y) {
    glReadBuffer(GL_COLOR_ATTACHMENT0 + index);

    s32 value;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &value);
    return value;
}

void Clear(Framebuffer *fb, u32 index, s32 value) {
    // glClearTexImage(color_attachments[index], 0, color_attachments_types[index], GL_INT, &value);
}

