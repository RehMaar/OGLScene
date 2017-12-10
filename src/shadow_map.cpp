
#include <stdio.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <GLFW/glfw3.h>

#include "shadow_map.hpp"


ShadowMapFBO::ShadowMapFBO() : fbo(0), shadow_map(0) {}
ShadowMapFBO::~ShadowMapFBO() {
    if (fbo != 0) {
        glDeleteFramebuffers(1, &fbo);
    }

    if (shadow_map != 0) {
        glDeleteFramebuffers(1, &shadow_map);
    }
}

int ShadowMapFBO::init (uint64_t width, uint64_t height) {
    /* Create framebuffer object. */
    glGenFramebuffers(1, &fbo);

    /* Create shadow map. */
    glGenTextures(1, &shadow_map);
    glBindTexture(GL_TEXTURE_2D, shadow_map);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    /* Attach texture to fbo. */
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, shadow_map, 0);

    /* Disable render to color buffer. */
    glDrawBuffer(GL_NONE);
    /* Check state. */ 
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "error: on init FBO %d\n", status);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        return -1;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    return 0;
}


void ShadowMapFBO::bind_fbo(void) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void ShadowMapFBO::unbind_fbo(void) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void ShadowMapFBO::activate_shadow_map(GLenum txr) {
    glActiveTexture(txr);
    glBindTexture(GL_TEXTURE_2D, shadow_map);
}
