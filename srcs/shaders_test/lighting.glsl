
#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

#define MAX_SHADOW_BOUNCES 80

float power_heuristic(float pdf_a, float pdf_b)
{
	float a2 = pdf_a * pdf_a;
	float b2 = pdf_b * pdf_b;
	if (a2 + b2 == 0.0)
		return 0.0;
	return a2 / (a2 + b2);
}

vec3 sphere_sample(int idx, vec3 origin, inout uint seed)
{
	GPUSphere s = spheres[idx];

	vec3 oc = s.center - origin;
	float dist2 = dot(oc, oc);
	float dist = sqrt(dist2);

	if (dist2 <= s.radius * s.radius)
		return s.center + random_unit_vector(seed) * s.radius;

	float cos_max = sqrt(1.0 - (s.radius * s.radius) / dist2);
	float r1 = random_float(seed);
	float r2 = random_float(seed);
	float cos_t = 1.0 - r1 * (1.0 - cos_max);
	float sin_t = sqrt(max(0.0, 1.0 - cos_t * cos_t));
	float phi = 2.0 * PI * r2;

	vec3 w = oc / dist;
	vec3 up = (abs(w.x) > 0.9) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	vec3 u = normalize(cross(up, w));
	vec3 v = cross(w, u);

	vec3 dir = u * (cos(phi) * sin_t) + v * (sin(phi) * sin_t) + w * cos_t;
	return origin + normalize(dir) * dist;
}

float sphere_pdf_value(int idx, vec3 origin, vec3 dir)
{
	HitRecord rec;
	if (!intersect_sphere(idx, Ray(origin, dir), 1e-4, FLT_MAX, rec))
		return 0.0;

	GPUSphere s = spheres[idx];
	vec3 oc = s.center - origin;
	float dist2 = dot(oc, oc);

	if (dist2 <= s.radius * s.radius)
		return 1.0 / (4.0 * PI * s.radius * s.radius);

	float cos_max = sqrt(1.0 - (s.radius * s.radius) / dist2);
	float solid_angle = 2.0 * PI * (1.0 - cos_max);
	if (solid_angle < 1e-6)
		return 0.0;
	return (solid_angle < 1e-8) ? 0.0 : (1.0 / solid_angle);
}


vec3 quad_sample(int idx, inout uint seed)
{
	GPUQuad q = quads[idx];
	mat4 fwd = inverse(q.inverse_transform);

	float r1 = random_float(seed);
	float r2 = random_float(seed);
	vec3 local_p = vec3(r1 * 2.0 - 1.0, 0.0, r2 * 2.0 - 1.0);

	return transform_dir(fwd, local_p);
}


float quad_pdf_value(int idx, vec3 origin, vec3 dir)
{
	HitRecord rec;
	if (!intersect_quad(idx, Ray(origin, dir), 1e-4, FLT_MAX, rec))
		return 0.0;

	GPUQuad q = quads[idx];
	float area = q.w * q.h;
	float distance_squared = rec.t * rec.t;
	float cosine = max(abs(dot(dir, rec.normal)), 1e-3);

	if (cosine < 1e-6 || area < 1e-6)
		return 0.0;
	return distance_squared / (cosine * area);
}


vec3 triangle_sample(int idx, inout uint seed)
{
	GPUTriangle tri = triangles[idx];

	float r1 = random_float(seed);
	float r2 = random_float(seed);
	if (r1 + r2 > 1.0)
	{
		r1 = 1.0 - r1;
		r2 = 1.0 - r2;
	}

	vec3 edge1 = tri.v1 - tri.v0;
	vec3 edge2 = tri.v2 - tri.v0;
	return tri.v0 + edge1 * r1 + edge2 * r2;
}

// Triangle::pdf_value
float triangle_pdf_value(int idx, vec3 origin, vec3 dir)
{
	HitRecord rec;
	if (!intersect_triangle(idx, Ray(origin, dir), 1e-4, FLT_MAX, rec))
		return 0.0;

	GPUTriangle tri = triangles[idx];
	vec3 edge1 = tri.v1 - tri.v0;
	vec3 edge2 = tri.v2 - tri.v0;
	float area = 0.5 * length(cross(edge1, edge2));
	float distance_squared = rec.t * rec.t;
	float cosine = max(abs(dot(dir, rec.normal)), 1e-3);

	if (cosine < 1e-6 || area < 1e-6)
		return 0.0;
	return distance_squared / (cosine * area);
}

vec3 light_sample(GPULight light, vec3 origin, inout uint seed)
{
	if (light.prim_type == PRIM_SPHERE)
		return sphere_sample(light.prim_idx, origin, seed);
	else if (light.prim_type == PRIM_QUAD)
		return quad_sample(light.prim_idx, seed);
	else
		return triangle_sample(light.prim_idx, seed);
}

float light_pdf_value(GPULight light, vec3 origin, vec3 dir)
{
	if (light.prim_type == PRIM_SPHERE)
		return sphere_pdf_value(light.prim_idx, origin, dir);
	else if (light.prim_type == PRIM_QUAD)
		return quad_pdf_value(light.prim_idx, origin, dir);
	else
		return triangle_pdf_value(light.prim_idx, origin, dir);
}

int light_get_mat_idx(GPULight light)
{
	if (light.prim_type == PRIM_SPHERE)
		return spheres[light.prim_idx].mat_idx;
	else if (light.prim_type == PRIM_QUAD)
		return quads[light.prim_idx].mat_idx;
	else
		return triangles[light.prim_idx].mat_idx;
}

vec3 sample_environment(vec3 dir)
{
	return vec3(0.0);
}

vec3 shadow_transmittance(vec3 origin, vec3 surface_normal, vec3 target_pos)
{
	vec3 light_dir = target_pos - origin;
	float light_dist = length(light_dir);

	if (light_dist < 1e-6)
		return vec3(1.0);

	vec3 dir_norm = light_dir / light_dist;
	if (dot(dir_norm, surface_normal) <= 0.0)
		return vec3(0.0);

	Ray shadow;
	shadow.origin = origin + surface_normal * 1e-3;
	shadow.dir = dir_norm;
	vec3 transmittance = vec3(1.0);

	for (int bounce = 0; bounce < MAX_SHADOW_BOUNCES; ++bounce)
	{
		HitRecord hit;
		if (!scene_hit(shadow, 1e-4, light_dist, hit))
			break;

		vec3 emittedHit = material_emitted(hit.mat_idx, hit.u, hit.v, hit.point);
		if (dot(emittedHit, emittedHit) > 0.0)
			break;

		GPUMaterial hmat = materials[hit.mat_idx];
		if (hmat.is_opaq != 0)
			return vec3(0.0);

		vec3 mat_color = hmat.color;
		transmittance *= mat_color;

		if (dot(transmittance, transmittance) < 1e-6)
			return vec3(0.0);

		shadow.origin = hit.point + shadow.dir * 1e-3;
		light_dist = length(target_pos - shadow.origin);
	}
	return transmittance;
}

vec3	calculate_direct_lighting_rand(Ray ray, HitRecord rec, vec3 albedo, inout uint seed)
{
	if (pc.light_count == 0u)
		return vec3(0.0);

	uint num_samples = uint(clamp(pc.rand_light_samples, 1, int(pc.light_count)));
	vec3 total_light = vec3(0.0);

	for (uint s = 0u; s < num_samples; ++s)
	{
		float r = random_float(seed);
		uint light_idx = min(uint(r * float(pc.light_count)), pc.light_count - 1u);
		GPULight light = lights[light_idx];
		vec3 light_pos = light_sample(light, rec.point, seed);
		vec3 to_light = light_pos - rec.point;
		float dist = length(to_light);
		if (dist < 1e-6)
			continue;

		vec3 to_light_n = to_light / dist;
		float cos_theta = dot(to_light_n, rec.normal);
		if (cos_theta <= 0.0)
			continue;

		vec3 transmit = shadow_transmittance(rec.point, rec.normal, light_pos);
		if (dot(transmit, transmit) < 1e-6)
			continue;

		float base_pdf_light = light_pdf_value(light, rec.point, to_light_n);
		if (base_pdf_light <= 1e-6)
			continue;
		float pdf_selection = 1.0 / float(pc.light_count);
		float actual_pdf_light = base_pdf_light * pdf_selection;

		Ray toLightRay;
		toLightRay.origin = rec.point;
		toLightRay.dir = to_light_n;
		float pdf_mat_eval = material_scattering_pdf(rec.mat_idx, ray, rec, toLightRay);
		float weight = power_heuristic(actual_pdf_light, pdf_mat_eval);
		vec3 light_color = material_emitted(light_get_mat_idx(light), rec.u, rec.v, light_pos);

		total_light += albedo * light_color * transmit * cos_theta * weight / actual_pdf_light;
	}

	return total_light / float(num_samples);
}


vec3 calculate_direct_lighting(Ray ray, HitRecord rec, vec3 albedo, inout uint seed)
{
	vec3 total_light = vec3(0.0);

	for (uint i = 0u; i < pc.light_count; ++i)
	{
		GPULight light = lights[i];

		vec3 light_pos = light_sample(light, rec.point, seed);
		vec3 to_light = light_pos - rec.point;
		float dist = length(to_light);
		if (dist < 1e-6)
			continue;

		vec3 to_light_n = to_light / dist;
		float cos_theta = dot(to_light_n, rec.normal);
		if (cos_theta <= 0.0)
			continue;

		vec3 transmit = shadow_transmittance(rec.point, rec.normal, light_pos);
		if (dot(transmit, transmit) < 1e-6)
			continue;

		float pdf_light = light_pdf_value(light, rec.point, to_light_n);
		if (pdf_light <= 1e-6)
			continue;

		Ray toLightRay;
		toLightRay.origin = rec.point;
		toLightRay.dir = to_light_n;
		float pdf_mat_eval = material_scattering_pdf(rec.mat_idx, ray, rec, toLightRay);
		float weight = power_heuristic(pdf_light, pdf_mat_eval);


		vec3 light_color = material_emitted(light_get_mat_idx(light), rec.u, rec.v, light_pos);

		total_light += albedo * light_color * transmit * cos_theta * weight / pdf_light;
	}
	return total_light;
}


vec3 apply_mis_weight(vec3 emitted, Ray ray, float prev_pdf, bool prev_was_specular, int depth)
{
	if (pc.shadow_ray == 0 || depth == 0 || prev_was_specular)
		return emitted;

	float pdf_light = 0.0;
	vec3 norm_dir = normalize(ray.dir);

	for (uint i = 0u; i < pc.light_count; ++i)
		pdf_light += light_pdf_value(lights[i], ray.origin, norm_dir);

	if (pdf_light <= 0.0)
		return vec3(0.0);

	float weight = power_heuristic(prev_pdf, pdf_light);
	return emitted * weight;
}

bool apply_russian_roulette(inout vec3 throughput, inout uint seed)
{
	float max_comp = max(throughput.x, max(throughput.y, throughput.z));

	if (max_comp < 0.01)
		return false;

	float rr = random_float(seed);
	if (rr > max_comp)
		return false;

	throughput /= max_comp;
	return true;
}

#endif
