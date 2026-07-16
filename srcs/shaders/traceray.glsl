

bool	apply_russian_roulette(inout vec3 throughput, inout uint seed)
{
	float	max_comp = max(throughput.x, max(throughput.y, throughput.z));

	if (max_comp < 0.01f)
		return (false);
	float rr = randomFloat(seed);
	if (rr > max_comp)
		return (false);
	throughput /= max_comp;
	return (true);
}

vec3	apply_mis_weight(vec3 emitted, Ray ray, float prev_pdf, bool prev_was_specular, int depth)
{
	if (pc.shadow_ray == 0 || depth == 0 || prev_was_specular)
		return emitted;
	float	pdf_light = 0.0;
	vec3	norm_dir = normalize(ray.dir);

	for (int i = 0; i < pc.light_count; ++i)
	{
		GPULight	light = lights[i];
		pdf_light += geometry_pdf_value(light.prim_type, light.prim_idx, ray.o, norm_dir);
	}
	if (pdf_light <= 0.0)
		return vec3(0.0);
	
	float	weight = power_heuristic(prev_pdf, pdf_light);
	return emitted * weight;
}



vec3	calculate_environment(Ray ray)
{
	//if (render.hdri && render.env_map.isLoaded())
		//return (render.env_map.sample(ray._dir));
	return (vec3(0.0f)); 
}



vec3	traceRay(Ray ray, inout uint seed)
{
	vec3	accumulated_light = vec3(0.0);
	vec3	throughput = vec3(1.0);
	float	prev_pdf_scatter = 1.0f;
	bool	prev_was_specular = false;
	//int		node_tests = 0;
	//int*	counter = (render.bvh_debug.mode == BvhDebugMode::HEATMAP) ? &node_tests : nullptr;

	// debug slice
	//if (render.bvh_debug.mode == BvhDebugMode::DEPTH_SLICE)
	//	return (evaluate_depth_slice(ray, render));

	for (int depth = 0; depth < pc.max_depth; ++depth)
	{
		// ray marching black hole
		// if (render.black_hole_enabled)
		// {
		// 	if (evaluate_blackholes(ray, render, accumulated_light, throughput))
		// 		break ;
		// }

		// hit geom
		HitRecord	rec;
		if (!hit_scene(ray, 1e-4, 1e30, rec))//!render.scene.hit(ray, 1e-3f, FLT_MAX, rec, counter))
		{
			accumulated_light += throughput * calculate_environment(ray);
			break ;
		}
		//applyNormalMap(rec);
		// light
		GPUMaterial	mat = materials[rec.mat_idx];
		bool is_opaq = (mat.is_opaq == 1);
		bool is_spec = (mat.is_spec == 1);
		bool can_receive = is_opaq && !is_spec;
		vec3	emitted = mat_emitted(mat, rec.u, rec.v, rec.point);
		if (dot(emitted, emitted) > 0.0f)
		{
			accumulated_light += throughput * apply_mis_weight(emitted, ray, prev_pdf_scatter, prev_was_specular, depth);
			break ;
		}

		// rebond indirect
		float	pdf_mat_scatter = 1.0f;
		Ray		new_ray;
		vec3	albedo;
		if (!mat_scatter(ray, rec, mat, albedo, new_ray, pdf_mat_scatter, seed))
			break ;
	
		// shadow rays /next event simulation
		if (pc.shadow_ray == 1 && can_receive)
		{
			// if (pc.light_count > 0 && depth == 0)
			// {
			// 	GPULight  dbg_light   = lights[0];
			// 	vec3      dbg_pos     = geometry_sample(dbg_light.prim_type, dbg_light.prim_idx, rec.point, seed);
			// 	vec3      dbg_trans   = shadow_transmittance(rec.point, rec.normal, dbg_pos);
			// 	float     dbg_len     = dot(dbg_trans, dbg_trans);

			// 	if (dbg_len < 1e-6)
			// 		return vec3(1.0, 0.0, 0.0);
			// 	else
			// 		return vec3(0.0, 1.0, 0.0);
			// }
			accumulated_light += throughput * calculate_direct_lighting(ray, rec, albedo, seed);
		}
		throughput *= albedo;

		//roulette russe
		if (pc.ru_enabled == 1)
		{
			if (depth > 3 && !apply_russian_roulette(throughput, seed))
				break ;
		}

		//next bounce
		prev_was_specular = is_spec;
		prev_pdf_scatter = pdf_mat_scatter;
		ray = new_ray;
	}

	return (accumulated_light);
}
