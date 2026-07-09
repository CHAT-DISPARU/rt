
float	mat_scattering_pdf(GPUMaterial mat, vec3 r_in_dir, vec3 rec_normal, vec3 scattered_dir)
{

	if (mat.type == MAT_LAMBERTIAN)
	{
		float	cosine = dot(rec_normal, normalize(scattered_dir));
		return (cosine < 0.0) ? 0.0 : cosine / M_PI;
	}
	else if (mat.type == MAT_PBR)
	{
		float	roughness = mat.roughness;
		float	metallic = mat.metallic;
		vec3	N = rec_normal;
		vec3	V = normalize(-r_in_dir);
		vec3	L = normalize(scattered_dir);
		float	NdotL = dot(N, L);
		if (NdotL <= 0.0)
			return 0.0;
		float	spec_prob = max(metallic, 0.04);
		float	pdf_diffuse = NdotL / M_PI;
		vec3	H = normalize(V + L);
		float	NdotH = max(dot(N, H), 0.0);
		float	HdotV = max(dot(H, V), 1e-4);
		float	D = distribution_ggx(N, H, roughness);
		float	pdf_specular = (D * NdotH) / (4.0 * HdotV);
		float	pdf_total = pdf_diffuse * (1.0 - spec_prob) + pdf_specular * spec_prob;
		return max(pdf_total, 0.0);
	}
	return 0.0;
}

float	power_heuristic(float pdf_a, float pdf_b)
{
	float	a2 = pdf_a * pdf_a;
	float	b2 = pdf_b * pdf_b;
	//secu 1/10000000000000000000000000000 skibidi
	if (a2 + b2 == 0.0f)
		return (0.0f); 
	return (a2 / (a2 + b2));
}


vec3	shadow_transmittance(vec3 origin, vec3 surface_normal, vec3 target_pos)
{
	vec3	light_dir = target_pos - origin;
	float	light_dist = length(light_dir);

	if (light_dist < 1e-6f)
		return (vec3(1.0f));
	vec3	dir_norm = light_dir / light_dist;
	//oppose a la light
	if (dot(dir_norm, surface_normal) <= 0.0f)
		return (vec3(0.0f));
	//monte un peut l origine sur la normal pour pas touche soi meme
	Ray		shadow = Ray(origin + surface_normal * 1e-3f, dir_norm, 1.0f);
	
	vec3	transmittance = vec3(1.0f);
	int		MAX_SHADOW_BOUNCES = 80;
	for (int bounce = 0; bounce < MAX_SHADOW_BOUNCES; ++bounce)
	{
		HitRecord	hit;

		if (!hit_scene(shadow, 1e-4, light_dist, hit))
			break ;
		//source
		GPUMaterial mat = materials[hit.mat_idx];
		vec3 emit = mat.emission;
		if (dot(emit, emit) > 0.0f)
			break ;
		// opaq zero
		if (mat.is_opaq == 1)
			return vec3(0.0);
		// beer lambert apllique la tinte....
		vec3	mat_color = mat.color;
		transmittance *= mat_color;

		if (dot(transmittance, transmittance) < 1e-6f)
			return (vec3(0.0f));
		shadow.o = hit.point + shadow.dir * 1e-3f;
		light_dist = length(target_pos - shadow.o);
	}
	return (transmittance);
}


vec3	calculate_direct_lighting(Ray ray, HitRecord rec, vec3 albedo, inout uint seed)
{
	vec3	total_light = vec3(0.0);
	
	GPUMaterial surface_mat = materials[rec.mat_idx];
	for (int i = 0; i < pc.light_count; ++i)
	{
		GPULight light = lights[i];

		vec3	light_pos = geometry_sample(light.prim_type, light.prim_idx, rec.point, seed);
		vec3	to_light = light_pos - rec.point;
		float	dist = length(to_light);
		if (dist < 1e-6)
			continue;
		vec3	to_light_n = to_light / dist;
		float	cos_theta = dot(to_light_n, rec.normal);
		if (cos_theta <= 0.0)
			continue;

		vec3	transmit = shadow_transmittance(rec.point, rec.normal, light_pos);
		if (dot(transmit, transmit) < 1e-6)
			continue;

		float	pdf_light = geometry_pdf_value(light.prim_type, light.prim_idx, rec.point, to_light_n);
		if (pdf_light <= 1e-6)
			continue;

		float	pdf_mat_eval = mat_scattering_pdf(surface_mat, ray.dir, rec.normal, to_light_n);
		float	weight = power_heuristic(pdf_light, pdf_mat_eval);
		vec3	light_color = vec3(0.0);
		
		if (light.prim_type == 0)
			light_color = materials[triangles[light.prim_idx].mat_idx].emission;
		else if (light.prim_type == 1)
			light_color = materials[spheres[light.prim_idx].mat_idx].emission;
		else if (light.prim_type == 2) 
			light_color = materials[quads[light.prim_idx].mat_idx].emission;
		total_light += albedo * light_color * transmit * cos_theta * weight / pdf_light;
	}
	return total_light;
}
