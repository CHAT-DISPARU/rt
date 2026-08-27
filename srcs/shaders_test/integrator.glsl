
#ifndef INTEGRATOR_GLSL
#define INTEGRATOR_GLSL

vec3 trace_ray(Ray ray, inout uint seed)
{
	vec3  accumulated_light = vec3(0.0);
	vec3  throughput = vec3(1.0);
	float prev_pdf_scatter = 1.0;
	bool  prev_was_specular = false;

	for (int depth = 0; depth < pc.max_depth; ++depth)
	{
		HitRecord rec;
		if (!scene_hit(ray, 1e-3, FLT_MAX, rec))
		{
			accumulated_light += throughput * sample_environment(ray.dir);
			break;
		}

		vec3 emitted = material_emitted(rec.mat_idx, rec.u, rec.v, rec.point);
		if (dot(emitted, emitted) > 0.0)
		{
			accumulated_light += throughput * apply_mis_weight(emitted, ray, prev_pdf_scatter, prev_was_specular, depth);
			break;
		}

		float pdf_mat_scatter = 1.0;
		Ray   new_ray;
		vec3  albedo;
		if (!material_scatter(rec.mat_idx, ray, rec, albedo, new_ray, pdf_mat_scatter, seed))
			break;

		GPUMaterial mat = materials[rec.mat_idx];
		if (pc.shadow_ray != 0 && mat.is_opaq != 0 && mat.is_spec == 0)
		{
			if (pc.light_teck == 0)
				accumulated_light += throughput * calculate_direct_lighting(ray, rec, albedo, seed);
			else
				accumulated_light += throughput * calculate_direct_lighting_rand(ray, rec, albedo, seed);
		}
		throughput *= albedo;

		if (pc.ru_enabled != 0)
		{
			if (depth > 3 && !apply_russian_roulette(throughput, seed))
				break;
		}

		prev_was_specular = (mat.is_spec != 0);
		prev_pdf_scatter = pdf_mat_scatter;
		ray = new_ray;
	}

	return accumulated_light;
}

#endif
