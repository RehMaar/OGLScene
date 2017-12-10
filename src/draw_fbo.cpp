#include <stdio.h>
#include <assert.h>
#include "draw_fbo.hpp"


DrawFBO::DrawFBO(uint64_t w, uint64_t h) {

    glGenFramebuffers(1, &fbo);

    glGenTextures(1, &txr);
    glBindTexture(GL_TEXTURE_2D, txr);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 w,
                 h,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 NULL
     );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           txr,
                           0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "error: on init FBO %d\n", status);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        assert(false);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
}

DrawFBO::~DrawFBO() {
    if (fbo != 0) {
        glDeleteFramebuffers(1, &fbo);
    }

    if (txr != 0) {
        glDeleteFramebuffers(1, &txr);
    }
}

void DrawFBO::bind(void) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void DrawFBO::unbind(void) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void DrawFBO::activate(GLenum txr_id) {
    glActiveTexture(txr_id);
    glBindTexture(GL_TEXTURE_2D, txr);
}
