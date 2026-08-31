
#ifndef TEXTURES_GLSL
#define TEXTURES_GLSL

#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 16) uniform sampler2D scene_textures[];

vec2 cpu_uv(vec2 uv)
{
	return vec2(uv.x, 1.0 - uv.y);
}


vec3 get_albedo(GPUMaterial mat, vec2 uv)
{
	if (mat.albedo_tex_idx < 0)
		return mat.color;

	vec3 tex_color = texture(scene_textures[nonuniformEXT(mat.albedo_tex_idx)], cpu_uv(uv)).rgb;
	return tex_color;
	// return tex_color * mat.color; // <- variante "tint"
}


float get_scalar_tex(int tex_idx, vec2 uv, int channel, float fallback)
{
	if (tex_idx < 0)
		return fallback;

	vec4 texel = texture(scene_textures[nonuniformEXT(tex_idx)], cpu_uv(uv));
	if (channel == 0) return texel.r;
	if (channel == 1) return texel.g;
	return texel.b;
}


float get_roughness(GPUMaterial mat, vec2 uv)
{
	return get_scalar_tex(mat.roughness_tex_idx, uv, 0, mat.roughness);
}

float get_metallic(GPUMaterial mat, vec2 uv)
{
	return get_scalar_tex(mat.metallic_tex_idx, uv, 0, mat.metallic);
}


vec3 apply_normal_map(GPUMaterial mat, vec2 uv, vec3 geom_normal)
{
	if (mat.normal_tex_idx < 0)
		return geom_normal;

	float scale = mat.normal_uv_scale;
	vec2  tiled_uv = uv * scale;

	vec3 raw = texture(scene_textures[nonuniformEXT(mat.normal_tex_idx)], cpu_uv(tiled_uv)).rgb;
	float strength = mat.normal_strength;


	vec3 tangent_normal = vec3(
		raw.x * 2.0 - strength,
		raw.y * 2.0 - strength,
		raw.z * 2.0 - 1.0
	);
	// vec3 tangent_normal = (raw * 2.0 - vec3(1.0)) * strength;

	tangent_normal = normalize(tangent_normal);

	vec3 N = geom_normal;
	vec3 up_guide = vec3(0.0, 1.0, 0.0);
	if (abs(dot(N, up_guide)) > 0.99)
		up_guide = vec3(0.0, 0.0, 1.0);

	vec3 T = normalize(cross(up_guide, N));
	vec3 B = normalize(cross(N, T));

	return normalize(T * tangent_normal.x + B * tangent_normal.y + N * tangent_normal.z);
}

#endif
