

#ifndef STRUCTS_GLSL
#define STRUCTS_GLSL


#define MAT_LAMBERTIAN 0
#define MAT_METAL      1
#define MAT_DIELECTRIC 2
#define MAT_PBR        3
#define MAT_EMISSIVE   4
#define PRIM_TRIANGLE 0
#define PRIM_SPHERE   1
#define PRIM_QUAD     2

const float PI       = 3.14159265358979323846;
const float FLT_MAX  = 3.402823466e+38;
const float EPSILON  = 1e-6;

struct GPUMaterial
{
	vec3  color;              // 12
	float roughness;          // 4   bloc 1

	vec3  emission;           // 12
	float metallic;           // 4   bloc 2

	float ior;                // 4
	int   type;                // 4
	int   albedo_tex_idx;      // 4
	int   normal_tex_idx;      // 4   bloc 3

	int   roughness_tex_idx;   // 4
	int   metallic_tex_idx;    // 4
	int   emission_tex_idx;    // 4
	int   is_opaq;             // 4   bloc 4

	int   is_spec;             // 4
	int   _pad0_0;
	int   _pad0_1;
	int   _pad0_2;             // 4   bloc 5
};

struct GPUTriangle
{
	vec3  v0;      // 12
	int   mat_idx; // 4   bloc 1

	vec3  v1;      // 12
	float uv0_x;   // 4   bloc 2

	vec3  v2;      // 12
	float uv0_y;   // 4   bloc 3

	float uv1_x;
	float uv1_y;
	float uv2_x;
	float uv2_y;   // 16  bloc 4
};

struct GPUSphere
{
	mat4  inverse_transform; // 64  4 blocs de 16

	vec3  center;  // 12
	float radius;  // 4   bloc 5

	int   mat_idx; // 4
	float _pad0;
	float _pad1;
	float _pad2;   // 12  bloc 6
};

struct GPUBVHNode
{
	vec3 aabb_min;                   // 12
	int  left_child_or_prim_offset;  // 4   bloc 1

	vec3 aabb_max;                   // 12
	int  right_child;                // 4   bloc 2

	int  primitive_count;            // 4  (>0 => feuille)
	int  axis;                       // 4
	int  _pad0;
	int  _pad1;                      // 8   bloc 3
};


struct GPUPlane
{
	mat4  inverse_transform; // 64

	vec3  point;   // 12
	int   mat_idx; // 4   bloc 5

	vec3  normal;  // 12
	int   _pad0;   // 4   bloc 6
};


struct GPUQuad
{
	mat4  inverse_transform; // 64

	vec3  center;  // 12
	int   mat_idx; // 4   bloc 5

	vec3  normal;  // 12
	float w;       // 4   bloc 6

	float h;       // 4
	float _pad0;
	float _pad1;
	float _pad2;   // 12  bloc 7
};

struct GPULight
{
	int prim_type; // 4
	int prim_idx;  // 4
	int _pad0;
	int _pad1;     // 8   bloc 1
};

struct Ray
{
	vec3 origin;
	vec3 dir;
};

vec3 ray_at(Ray r, float t)
{
	return (r.dir * t) + r.origin;
}

struct HitRecord
{
	float t;
	float u;
	float v;
	vec3  normal;
	vec3  point;
	bool  front_face;
	int   mat_idx;
	float ni_from;
};

void set_face_normal(inout HitRecord rec, Ray r, vec3 outward_normal)
{
	rec.front_face = dot(r.dir, outward_normal) < 0.0;
	rec.normal = rec.front_face ? outward_normal : -outward_normal;
}

#endif
