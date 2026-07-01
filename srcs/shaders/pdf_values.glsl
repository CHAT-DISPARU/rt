vec3 Triangle_sample(inout vec3 origin, GPUTriangle tri, inout uint seed)
{
	float	r1 = randomFloat(seed);
	float	r2 = randomFloat(seed);

	if (r1 + r2 > 1.0f)
	{
		r1 = 1.0f - r1;
		r2 = 1.0f - r2;
	}
	vec3	edge1 = tri.v1 - tri.v0;
	vec3	edge2 = tri.v2 - tri.v0;
	return (tri.v0 + edge1 * r1 + edge2 * r2);
}

float	Triangle_pdf_value(vec3 origin, GPUTriangle tri, vec3 dir)
{
	HitRecord	rec;

	if (!hit_triangle(Ray(origin, dir), tri, 1e-4, 1e30, rec))
		return (0.0f);

	vec3	edge1 = tri.v1 - tri.v0;
	vec3	edge2 = tri.v2 - tri.v0;
	float area = 0.5 * length(cross(edge1, edge2));
	float	distance_squared = rec.t * rec.t;
	float	cosine = max(abs(dot(dir, rec.normal)), 1e-3f);

	if (cosine < 1e-6f || area < 1e-6f)
		return (0.0f);
	// PDF = (Distance au carre) / (Cosinus * Aire)
	return (distance_squared / (cosine * area));
}

vec3	Sphere_sample(vec3 origin, GPUSphere sph, inout uint seed)
{
	vec3	oc = sph.center - origin;
	float	dist2 = dot(oc, oc);
	float	dist = sqrt(dist2);

	//dans sphere
	if (dist2 <= sph.radius * sph.radius)
		return (sph.center + randomUnitVector(seed) * sph.radius);

	// tire cone fisible
	float	cos_max = sqrt(1.0f - (sph.radius * sph.radius) / dist2);
	float	r1 = randomFloat(seed);
	float	r2 = randomFloat(seed);
	float	cos_t = 1.0f - r1 * (1.0f - cos_max);
	float	sin_t = sqrt(max(0.0f, 1.0f - cos_t * cos_t));
	float	phi = 2.0f * M_PI * r2;
	//base ortonorme aolign sur oc
	vec3	w = oc / dist;
	vec3	up = (abs(w.x) > 0.9f)
				? vec3(0.0f, 1.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3	u = normalize(cross(up, w));
	vec3	v = cross(w, u);

	//dir -> cone
	vec3	dir = u * (cos(phi) * sin_t)
				+ v * (sin(phi) * sin_t) + w * cos_t;
	return (origin + normalize(dir) * dist);
}

float	Sphere_pdf_value(vec3 origin, GPUSphere sph, vec3 dir)
{
	HitRecord	rec;
	if (!hit_sphere(Ray(origin, dir), sph, 1e-4, 1e30, rec))
		return (0.0f);
	vec3	oc = sph.center - origin;
	float	dist2 = dot(oc, oc);

	if (dist2 <= sph.radius * sph.radius)
		return (1.0f / (4.0f * M_PI * sph.radius * sph.radius));

	float	cos_max = sqrt(1.0f - (sph.radius * sph.radius) / dist2);
	float	solid_angle = 2.0f * M_PI * (1.0f - cos_max);
	if (solid_angle < 1e-6f)
		return (0.0f);
	return (solid_angle < 1e-8f) ? 0.0f : (1.0f / solid_angle);
}

vec3	Quad_sample(vec3 origin, GPUQuad quad, inout uint seed)
{
	float	r1 = randomFloat(seed);
	float	r2 = randomFloat(seed);
	float	local_x = r1 * 2.0 - 1.0;
	float	local_z = r2 * 2.0 - 1.0;
	vec3	local_p = vec3(local_x, 0.0, local_z);

	return (quad.transform * vec4(local_p, 1.0)).xyz;
}

float	Quad_pdf_value(vec3 origin, vec3 dir, GPUQuad quad)
{
	HitRecord	rec;

	if (!hit_quad(Ray(origin, dir), quad, 1e-4, 1e30, rec))
		return 0.0;
	float	area = quad.w * quad.h;
	float	distance_squared = rec.t * rec.t;
	float	cosine = max(abs(dot(dir, rec.normal)), 1e-3);
	if (cosine < 1e-6 || area < 1e-6)
		return 0.0;
	return (distance_squared / (cosine * area));
}
