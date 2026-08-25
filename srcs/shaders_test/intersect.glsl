
#ifndef INTERSECT_GLSL
#define INTERSECT_GLSL


bool aabb_hit(vec3 bmin, vec3 bmax, vec3 origin, vec3 invDir, float tMin, float tMax)
{
	for (int a = 0; a < 3; ++a)
	{
		float t0 = (bmin[a] - origin[a]) * invDir[a];
		float t1 = (bmax[a] - origin[a]) * invDir[a];
		if (invDir[a] < 0.0)
		{
			float tmp = t0;
			t0 = t1;
			t1 = tmp;
		}
		tMin = t0 > tMin ? t0 : tMin;
		tMax = t1 < tMax ? t1 : tMax;
		if (tMax <= tMin)
			return false;
	}
	return true;
}


bool intersect_sphere(int idx, Ray ray, float tMin, float tMax, out HitRecord rec)
{
	GPUSphere s = spheres[idx];

	Ray l_ray = transform_ray(ray, s.inverse_transform);

	float a = dot(l_ray.dir, l_ray.dir);
	float b = dot(l_ray.origin, l_ray.dir);
	float c = dot(l_ray.origin, l_ray.origin) - 1.0;

	float delta = b * b - a * c;
	if (delta < 0.0)
		return false;

	float sqrtd = sqrt(delta);
	float t = (-b - sqrtd) / a;

	if (t < tMin || t > tMax)
	{
		t = (-b + sqrtd) / a;
		if (t < tMin || t > tMax)
			return false;
	}

	rec.t = t;
	rec.mat_idx = s.mat_idx;
	rec.point = ray_at(ray, t);

	vec3 out_normal = normalize(rec.point - s.center);
	set_face_normal(rec, ray, out_normal);

	vec3 local_p = ray_at(l_ray, t);
	float phi = atan(local_p.z, local_p.x);
	float y_clamped = clamp(local_p.y, -1.0, 1.0);
	float theta = asin(y_clamped);

	rec.u = 1.0 - ((phi + PI) / (2.0 * PI));
	rec.v = (theta + PI / 2.0) / PI;
	rec.ni_from = 1.0;
	return true;
}

bool intersect_quad(int idx, Ray ray, float tMin, float tMax, out HitRecord rec)
{
	GPUQuad q = quads[idx];

	Ray l_ray = transform_ray(ray, q.inverse_transform);

	float denom = l_ray.dir.y;
	if (abs(denom) < 1e-6)
		return false;

	float t = -l_ray.origin.y / denom;
	if (t < tMin || t > tMax)
		return false;

	vec3 p = l_ray.origin + (l_ray.dir * t);
	if (abs(p.x) > 1.0 || abs(p.z) > 1.0)
		return false;

	rec.t = t;
	rec.mat_idx = q.mat_idx;
	rec.point = ray_at(ray, t);
	set_face_normal(rec, ray, q.normal);
	rec.u = ((p.x + 1.0) * 0.5) * q.w;
	rec.v = ((p.z + 1.0) * 0.5) * q.h;
	rec.ni_from = 1.0;
	return true;
}

bool intersect_triangle(int idx, Ray ray, float tMin, float tMax, out HitRecord rec)
{
	GPUTriangle tri = triangles[idx];

	vec3 edge1 = tri.v1 - tri.v0;
	vec3 edge2 = tri.v2 - tri.v0;
	vec3 pvec = cross(ray.dir, edge2);
	float det = dot(edge1, pvec);

	if (abs(det) < 1e-6)
		return false;

	float inv_det = 1.0 / det;
	vec3 tvec = ray.origin - tri.v0;
	float u = dot(tvec, pvec) * inv_det;
	if (u < 0.0 || u > 1.0)
		return false;

	vec3 qvec = cross(tvec, edge1);
	float v = dot(ray.dir, qvec) * inv_det;
	if (v < 0.0 || u + v > 1.0)
		return false;

	float t = dot(edge2, qvec) * inv_det;
	if (t < tMin || t > tMax)
		return false;

	rec.t = t;
	rec.point = ray_at(ray, t);
	rec.mat_idx = tri.mat_idx;

	float w = 1.0 - u - v;
	rec.u = w * tri.uv0_x + u * tri.uv1_x + v * tri.uv2_x;
	rec.v = w * tri.uv0_y + u * tri.uv1_y + v * tri.uv2_y;

	vec3 normal = normalize(cross(edge1, edge2));
	set_face_normal(rec, ray, normal);
	rec.ni_from = 1.0;
	return true;
}


bool intersect_plane(int idx, Ray ray, float tMin, float tMax, out HitRecord rec)
{
	GPUPlane pl = planes[idx];

	Ray l_ray = transform_ray(ray, pl.inverse_transform);

	float denom = l_ray.dir.y;
	if (abs(denom) < tMin)
		return false;

	float t = -l_ray.origin.y / denom;
	if (t < tMin || t > tMax)
		return false;

	rec.t = t;
	rec.mat_idx = pl.mat_idx;
	rec.point = ray_at(ray, t);
	set_face_normal(rec, ray, pl.normal);

	vec3 local_p = l_ray.origin + (l_ray.dir * t);
	rec.u = local_p.x;
	rec.v = local_p.z;
	rec.ni_from = 1.0;
	return true;
}

#endif // INTERSECT_GLSL
