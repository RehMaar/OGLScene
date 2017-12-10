#pragma once

/* FBO -- frame buffer object. */
class ShadowMapFBO {

	public:
    	ShadowMapFBO();
    	~ShadowMapFBO();

    	int init(uint64_t width, uint64_t height);

    	void bind_fbo(void);
    	void unbind_fbo(void);
    	void activate_shadow_map(GLenum txr);

	private:

    	GLuint fbo;
    	GLuint shadow_map;
};
