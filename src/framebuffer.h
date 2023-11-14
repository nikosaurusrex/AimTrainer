#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "common.h"

struct Framebuffer {
    s32 width;
    s32 height;
    GLuint id = 0;
    Array<GLint> color_attachments_types;
    Array<GLuint> color_attachments;
    GLint depth_attachment_type = 0;
    GLuint depth_attachment;
};

Framebuffer *CreateFramebuffer(s32 width, s32 height, Array<GLint> attachments);
void DestroyFramebuffer(Framebuffer *fb);

void Reload(Framebuffer *fb);

void Bind(Framebuffer *fb);
void Unbind(Framebuffer *fb);

void Resize(Framebuffer *fb, s32 width, s32 height);

GLuint GetAttachment(Framebuffer *fb, u32 index);
s32 Read(Framebuffer *fb, u32 index, s32 x, s32 y);
void Clear(Framebuffer *fb, u32 index, s32 value);

#endif
