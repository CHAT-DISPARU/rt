

void set_face_normal(inout HitRecord rec, Ray r, vec3 outward_normal) 
{
	rec.front_face = dot(r.dir, outward_normal) < 0.0;
	rec.normal = rec.front_face ? outward_normal : -outward_normal;
	rec.ni_from = r.current_ior;
}

//applique matrice invers
Ray	transform_ray(Ray r, mat4 inv_mat)
{
	vec3	local_o = (inv_mat * vec4(r.o, 1.0)).xyz;
	vec3	local_dir = (inv_mat * vec4(r.dir, 0.0)).xyz;
	return (Ray(local_o, local_dir, r.current_ior));
}

//triangle
bool hit_triangle(Ray ray, GPUTriangle tri, float t_min, float t_max, inout HitRecord rec)
{
	vec3	edge1 = tri.v1 - tri.v0;
	vec3	edge2 = tri.v2 - tri.v0;
	vec3	pvec = cross(ray.dir, edge2);
	float	det = dot(edge1, pvec);

	if (abs(det) < 1e-6)
		return false;
	float	inv_det = 1.0 / det;
	vec3	tvec = ray.o - tri.v0;
	float	u = dot(tvec, pvec) * inv_det;
	if (u < 0.0 || u > 1.0)
		return false;
	vec3	qvec = cross(tvec, edge1);
	float	v = dot(ray.dir, qvec) * inv_det;
	if (v < 0.0 || u + v > 1.0)
		return false;
	float	t = dot(edge2, qvec) * inv_det;
	if (t < t_min || t > t_max)
		return false;
	rec.t = t;
	rec.point = ray.o + t * ray.dir;
	rec.u = u;
	rec.v = v;
	rec.mat_idx = tri.mat_idx;
	vec3	outward_normal = normalize(cross(edge1, edge2));
	set_face_normal(rec, ray, outward_normal);
	return true;
}

//sphere
#define M_PI 3.14159265358979323846

bool hit_sphere(Ray ray, GPUSphere sph, float t_min, float t_max, inout HitRecord rec)
{
	Ray		l_ray = transform_ray(ray, sph.inverse_transform);
	float	a = dot(l_ray.dir, l_ray.dir);
	float	b = dot(l_ray.o, l_ray.dir);
	float	c = dot(l_ray.o, l_ray.o) - 1.0;
	float	delta = b * b - a * c;

	if (delta < 0.0)
		return false;
	float	sqrtd = sqrt(delta);
	float	t = (-b - sqrtd) / a;

	if (t < t_min || t > t_max)
	{
		t = (-b + sqrtd) / a;
		if (t < t_min || t > t_max)
			return false;
	}
	rec.t = t;
	rec.point = ray.o + t * ray.dir;
	vec3	outward_normal = normalize(rec.point - sph.center);
	set_face_normal(rec, ray, outward_normal);
	//uv
	vec3	local_p = l_ray.o + t * l_ray.dir;
	float	phi = atan(local_p.z, local_p.x);
	float	y_clamped = clamp(local_p.y, -1.0, 1.0);
	float	theta = asin(y_clamped);
	rec.u = 1.0 - ((phi + M_PI) / (2.0 * M_PI));
	rec.v = (theta + M_PI / 2.0) / M_PI;
	rec.mat_idx = sph.mat_idx;
	return true;
}

//quad
bool hit_quad(Ray ray, GPUQuad quad, float t_min, float t_max, inout HitRecord rec)
{
	Ray		l_ray = transform_ray(ray, quad.inverse_transform);
	float	denom = l_ray.dir.y;
	
	if (abs(denom) < 1e-6)
		return false;
	float	t = -l_ray.o.y / denom;
	if (t < t_min || t > t_max)
		return false;
	vec3	p = l_ray.o + l_ray.dir * t;
	if (abs(p.x) > 1.0 || abs(p.z) > 1.0)
		return false;
	rec.t = t;
	rec.point = ray.o + t * ray.dir;
	set_face_normal(rec, ray, quad.normal);
	rec.u = ((p.x + 1.0) * 0.5) * quad.w;
	rec.v = ((p.z + 1.0) * 0.5) * quad.h;
	rec.mat_idx = quad.mat_idx;
	return true;
}


//plane
bool hit_plane(Ray ray, GPUPlane plane, float t_min, float t_max, inout HitRecord rec)
{
	Ray		l_ray = transform_ray(ray, plane.inverse_transform);
	float	denom = l_ray.dir.y;

	if (abs(denom) < t_min)
		return false;
	float	t = -l_ray.o.y / denom;
	if (t < t_min || t > t_max)
		return false;
	rec.t = t;
	rec.point = ray.o + t * ray.dir;
	set_face_normal(rec, ray, plane.normal);
	vec3	local_p = l_ray.o + l_ray.dir * t;
	rec.u = local_p.x;
	rec.v = local_p.z;
	rec.mat_idx = plane.mat_idx;
	return true;
}

bool	hit_aabb(vec3 aabb_min, vec3 aabb_max, Ray ray, vec3 invDir, float tMin, float tMax)
{
	vec3	t0 = (aabb_min - ray.o) * invDir;
	vec3	t1 = (aabb_max - ray.o) * invDir;
	vec3	tmin_vec = min(t0, t1);
	vec3	tmax_vec = max(t0, t1);
	float	tmin_final = max(tMin, max(tmin_vec.x, max(tmin_vec.y, tmin_vec.z)));
	float	tmax_final = min(tMax, min(tmax_vec.x, min(tmax_vec.y, tmax_vec.z)));
	return tmax_final > tmin_final;
}