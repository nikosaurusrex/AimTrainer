#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "common.h"

struct framebuffer_t {
    s32             width;
    s32             height;
    GLuint          id = 0;
    array_t<GLint>  color_attachments_types;
    array_t<GLuint> color_attachments;
    GLint           depth_attachment_type = 0;
    GLuint          depth_attachment;
};

framebuffer_t   *CreateFramebuffer(s32 width, s32 height, array_t<GLint> attachments);
void            DestroyFramebuffer(framebuffer_t *fb);

void Reload(framebuffer_t *fb);
void Bind(framebuffer_t *fb);
void Unbind(framebuffer_t *fb);
void Resize(framebuffer_t *fb, s32 width, s32 height);

GLuint  GetAttachment(framebuffer_t *fb, u32 index);
s32     Read(framebuffer_t *fb, u32 index, s32 x, s32 y);
void    Clear(framebuffer_t *fb, u32 index, s32 value);

#endif
