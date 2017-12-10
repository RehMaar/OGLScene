#version 330 core 
 
in vec2 UV; 
in vec3 normals_out;
in vec3 world;
in vec4 light_space_pos1;
in vec4 light_space_pos2;

out vec4 FragColor;
 
struct direct_light 
{ 
    vec3 direct;
    vec3 color; 
    float ai; 
    float di;
}; 

struct attenuation
{
    float a_const;
    float a_linear;
    float a_exp;
};

struct point_light
{
    attenuation a;
    vec3 pos;
    vec3 color; 
    float ai; 
    float di;
};

struct spot_light
{
    point_light pl;
    vec3 direct;
    float cutoff;
};

uniform direct_light s_dl; 
uniform spot_light s_sl1;
uniform spot_light s_sl2;

uniform sampler2D s_sampler; 
uniform sampler2D s_shadow_map1;
uniform sampler2D s_shadow_map2;

uniform sampler2D transparency_map1;
uniform sampler2D transparency_map2;

uniform float trans;
uniform vec3 s_view_pos;
uniform float s_mat_ref_i;
uniform float s_spec_power;

float get_shadow_factor(vec4 light_space, sampler2D shadow_map, sampler2D t_map)
{

    /* Fragment's light position in range [-1, 1] */
    vec3 ProjCoords = light_space.xyz / light_space.w;

//  float bias = 0.005;
//  float vis = 1.0;
//
//    if (texture2D(s_sampler, UV).z < ProjCoords.z - bias) {
//      vis = 0.5;
//    }
    
    /* To [0, 1] range */
    ProjCoords = 0.5 * ProjCoords + 0.5;
    if (ProjCoords.x < 0 || ProjCoords.x > 1) {
        return 1.0;
    }
    if (ProjCoords.y < 0 || ProjCoords.y > 1) {
        return 1.0;
    }

    float Depth = texture2D(shadow_map, ProjCoords.xy).r;
    float eps = 0.00000;
    if (Depth < ProjCoords.z + eps) {
        return 0.0;
    }

    float t = texture2D(t_map, ProjCoords.xy).r;
    return 1.0 - t;
}

vec4 get_light(vec3 color, float ai, float di, vec3 dir, vec3 normal, float sf) { 
    vec4 d_color = vec4(0, 0, 0, 0);        /* diffuse color */
    vec4 s_color = vec4(0, 0, 0, 0);        /* specular color */
    vec4 a_color  = vec4(color * ai, 1.0f); /* ambient color */

    float d_factor = dot(normal, -dir); /* scalar product */
    if (d_factor > 0) {
        d_color = vec4(color * di * d_factor, 1.0f);
    }


    float s_factor = dot(normalize(s_view_pos - world),
                         normalize(reflect(dir, normal)));
    if (s_factor > 0) {
        s_factor = pow(s_factor, s_spec_power);
        s_color = vec4(color * s_mat_ref_i * s_factor, 1.0f);
    }
    return a_color + sf * (d_color + s_color);
}

vec4 get_point_light(point_light pl, vec3 normal, vec4 light_space, sampler2D shadow_map, sampler2D t_map) {

    vec3 l_dir = world - pl.pos;
    float dist = length(l_dir);
    l_dir = normalize(l_dir);

    float sf = get_shadow_factor(light_space, shadow_map, t_map);
    vec4 color = get_light(pl.color, pl.ai, pl.di, l_dir, normal, sf);
    float at = pl.a.a_const + pl.a.a_linear * dist + pl.a.a_exp * dist * dist;

    return color / at;
}

vec4 get_direct_light(vec3 normal) {
    return get_light(s_dl.color, s_dl.ai, s_dl.di, s_dl.direct, normal, 1.0);
}

vec4 get_spot_light(spot_light l, vec3 normal, vec4 light_space, sampler2D shadow_map, sampler2D t_map) {
    vec3 ltp = normalize(world - l.pl.pos);
    float spot_factor = dot(ltp, l.direct);

    if (spot_factor > l.cutoff) {
        vec4 color = get_point_light(l.pl, normal, light_space, shadow_map, t_map);
        return color * (1.0 - (1.0 - spot_factor) * 1.0 / (1.0 - l.cutoff));
    } else {
        return vec4(0, 0, 0, 0);
    }
}

void main() {
    vec3 normal = normalize(normals_out);
    vec4 total = get_direct_light(normal);

    total += get_spot_light(s_sl1, normal, light_space_pos1, s_shadow_map1, transparency_map1);
    total += get_spot_light(s_sl2, normal, light_space_pos2, s_shadow_map2, transparency_map2);

    FragColor = texture2D(s_sampler, UV) * total;
    FragColor.a = trans;
}
