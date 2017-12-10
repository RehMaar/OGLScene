#pragma once

#include "transform.hpp"
#include "object.hpp"
#include "shaders.hpp"

#include <glm/glm.hpp>

class Shadow {

	public:
    	Shadow() = default;
    	int init();

        void add_object(Object *obj, Transform *t);
        void render();
        void set_light(const glm::vec3 & pos,
                       const glm::vec3 & tar,
                       const glm::vec3 & up);

	private:

		struct objs_prop {
			Object *obj;
			Transform *t;
		};
        std::vector<objs_prop> objs;
        glm::vec3 l_pos;
        glm::vec3 l_tar;
        glm::vec3 l_up;
    	
    	void set_wvp(const glm::mat4 & wvp);
		void set_texture_unit(GLuint txr_idx);

		void enable();
    	Shader s_shader;

		enum uniform {
    		WVP,
    		SHADOW_MAP,
			UNIFORM_LEN,
		};

    	struct uniform_descr {
			GLuint idx;
			const char * name;
    	} descr[UNIFORM_LEN] = {
			{INVALID_UNIFORM, "gWVP"},
			{INVALID_UNIFORM, "gShadowMap"},
    	};
};
