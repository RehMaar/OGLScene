#pragma once

#include <glm/glm.hpp>

#include "shaders.hpp"

struct direct_light {
	glm::vec3 color;
	glm::vec3 direct;
	float di; /* Diffuse Intensity. */
	float ai; /* Ambient Intensity: (influence on color brightness).*/

	direct_light() {
		color  = glm::vec3(0.0f, 0.0f, 0.0f);
		direct = glm::vec3(0.0f, 0.0f, 0.0f);
		di = 0.0f;
		ai = 0.0f;
	}
};

struct point_light {
    glm::vec3 pos;
	glm::vec3 color;
	float ai;
	float di;

	/* fading = l1 / (const + linear*dist^2 + exp * dist^2) */
	struct fading_constants {
		float f_const;
		float f_linear;
		float f_exp;
	} fading;

	point_light() {
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		pos   = glm::vec3(0.0f, 0.0f, 0.0f);
		di = 1;
		ai = 1;
		fading = { 1.0f, 0.0f, 0.0f };
	}
};

struct spot_light {
    point_light pl;
	glm::vec3 direct;
	float cutoff;

	spot_light() {
		direct = glm::vec3(0, 0, 0);
		cutoff = 0;
	}
};

class Light {
public:
    Light() = default;
	int init();

	void set_wvp(const glm::mat4 & wvp);
	void set_world(const glm::mat4 & world);

	void set_light_wvp(const glm::mat4 & light_wvp, uint64_t n);
	void set_shadow_map(GLuint sm_idx, uint64_t n);

	void set_texture_unit(GLuint txr_idx);

	void set_direct_light(const direct_light & light);
	void set_spot_light(const spot_light & light, uint64_t n);

    void set_specular_power(float p);
    void set_view_point(const glm::vec3 & v);
    void set_material_reflect_i(float i);
    void set_transparency(float i);
    void set_transparency_map(GLuint idx, uint64_t n);

	void enable(void);
private:

	enum uniform {
		WVP,
		WORLD,
		SAMPLER,
		VIEW_POS,
		MAT_REF_I,
		SPEC_POWER,
		DL_COLOR,
		DL_DIRECT,
		DL_AI,
		DL_DI,

		SL_DIRECT1,
		SL_DIRECT2,

		SL_CUTOFF1,
		SL_CUTOFF2,

		SL_PL_CONST1,
		SL_PL_CONST2,

		SL_PL_LINEAR1,
		SL_PL_LINEAR2,

		SL_PL_EXP1,
		SL_PL_EXP2,

		SL_PL_POS1,
		SL_PL_POS2,

		SL_PL_COLOR1,
		SL_PL_COLOR2,

		SL_PL_DI1,
		SL_PL_DI2,

		SL_PL_AI1,
		SL_PL_AI2,


		SHADOW_MAP1,
		SHADOW_MAP2,

		LIGHT_WVP1,
		LIGHT_WVP2,

		TRANS_MAP1,
		TRANS_MAP2,
		TRANS,
		UNIFORM_LEN,
	};
  
	struct uniform_descr {
		GLuint idx;
		const char * name;
	} descr[UNIFORM_LEN] = {
		{INVALID_UNIFORM, "s_WVP"},
		{INVALID_UNIFORM, "s_world"},
		{INVALID_UNIFORM, "s_sampler"},
		{INVALID_UNIFORM, "s_view_pos"},
		{INVALID_UNIFORM, "s_mat_ref_i"},
		{INVALID_UNIFORM, "s_spec_power"},
		{INVALID_UNIFORM, "s_dl.color"},
		{INVALID_UNIFORM, "s_dl.direct"},
		{INVALID_UNIFORM, "s_dl.ai"},
		{INVALID_UNIFORM, "s_dl.di"},

		{INVALID_UNIFORM, "s_sl1.direct"},
		{INVALID_UNIFORM, "s_sl2.direct"},
		{INVALID_UNIFORM, "s_sl1.cutoff"},
		{INVALID_UNIFORM, "s_sl2.cutoff"},
		{INVALID_UNIFORM, "s_sl1.pl.a.a_const"},
		{INVALID_UNIFORM, "s_sl2.pl.a.a_const"},
		{INVALID_UNIFORM, "s_sl1.pl.a.a_linear"},
		{INVALID_UNIFORM, "s_sl2.pl.a.a_linear"},
		{INVALID_UNIFORM, "s_sl1.pl.a.a_exp"},
		{INVALID_UNIFORM, "s_sl2.pl.a.a_exp"},
		{INVALID_UNIFORM, "s_sl1.pl.pos"},
		{INVALID_UNIFORM, "s_sl2.pl.pos"},
		{INVALID_UNIFORM, "s_sl1.pl.color"},
		{INVALID_UNIFORM, "s_sl2.pl.color"},
		{INVALID_UNIFORM, "s_sl1.pl.di"},
		{INVALID_UNIFORM, "s_sl2.pl.di"},
		{INVALID_UNIFORM, "s_sl1.pl.ai"},
		{INVALID_UNIFORM, "s_sl2.pl.ai"},

		{INVALID_UNIFORM, "s_shadow_map1"},
		{INVALID_UNIFORM, "s_shadow_map2"},
		{INVALID_UNIFORM, "s_light_wvp1"},
		{INVALID_UNIFORM, "s_light_wvp2"},

		{INVALID_UNIFORM, "transparency_map1"},
		{INVALID_UNIFORM, "transparency_map2"},

		{INVALID_UNIFORM, "trans"}
   	};

    Shader l_shader;

};
