#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class DrawFBO {

    public:

		DrawFBO(uint64_t w, uint64_t h);
		~DrawFBO();

		void bind(void);
		void unbind(void);
		void activate(GLenum txr_id);
        
    private:

        GLuint fbo;
        GLuint txr;
};
