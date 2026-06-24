/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 18:08:08 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/24 11:21:32 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.hpp"

static constexpr int	MAX_SHADOW_BOUNCES = 80;

inline float	power_heuristic(float pdf_a, float pdf_b)
{
	float	a2 = pdf_a * pdf_a;
	float	b2 = pdf_b * pdf_b;
	//secu 1/10000000000000000000000000000 skibidi
	if (a2 + b2 == 0.0f)
		return (0.0f); 
	return (a2 / (a2 + b2));
}

Vec3f	shadow_transmittance(const Scene &scene, const Vec3f &origin, const Vec3f &surface_normal,
								const Vec3f &target_pos, int* counter)
{
	Vec3f	light_dir = target_pos - origin;
	float	light_dist = light_dir.length();

	if (light_dist < 1e-6f)
		return (Vec3f(1.0f));
	Vec3f	dir_norm = light_dir / light_dist;
	//oppose a la light
	if (Vec3f::dot(dir_norm, surface_normal) <= 0.0f)
		return (Vec3f(0.0f));
	//monte un peut l origine sur la normal pour pas touche soi meme
	Ray		shadow(origin + surface_normal * 1e-3f, dir_norm);
	Vec3f	transmittance(1.0f);

	for (int bounce = 0; bounce < MAX_SHADOW_BOUNCES; ++bounce)
	{
		HitRecord	hit;

		if (!scene.hit(shadow, 1e-4f, light_dist, hit, counter))
			break ;

		if (!hit.material)
			return (Vec3f(0.0f));
		//source
		if (hit.material->emitted(hit.u, hit.v, hit.point).length_sq() > 0.0f)
			break ;
		// opaq zero
		if (hit.material->isOpaq())
			return (Vec3f(0.0f));
		// beer lambert apllique la tinte....
		Vec3f	mat_color = hit.material->sampleAlbedo(hit.u, hit.v);
		transmittance *= mat_color;

		if (transmittance.length_sq() < 1e-6f)
			return (Vec3f(0.0f));

		/*float	ior = hit.material->ior();
		bool	enter = (Vec3f::dot(shadow._dir, hit.normal) < 0.0f);
		float	ratio = enter ? (1.0f / ior) : ior;
		Vec3f	n = enter ? hit.normal : -hit.normal;
		Vec3f	refracted = Vec3f::refract(Vec3f::normalize(shadow._dir), n, ratio);

		//refract echoue
		if (refracted.length_sq() < 0.5f)
			return (Vec3f(0.0f));

		Vec3f	offset_normal = enter ? -hit.normal : hit.normal;
		*/
		shadow._o = hit.point + shadow._dir * 1e-3f;
		//shadow._dir = Vec3f::normalize(refracted);
		light_dist = (target_pos - shadow._o).length();
	}
	return (transmittance);
}

Vec3f	evaluate_depth_slice(const Ray& ray, const Render& render)
{
	HitRecord	rec;
	float		t_geom = FLT_MAX;
	Vec3f		base(0.02f, 0.02f, 0.025f);

	if (render.scene.hit(ray, 1e-3f, FLT_MAX, rec))
	{
		t_geom = rec.t;
		Vec3f	albedo;
		Ray		dummy;
		float	pdf = 1.0f;
		rec.material->scatter(ray, rec, albedo, dummy, pdf, render.seed);
		base = albedo * 0.18f;
	}
	Vec3f	box_color(0.0f);
	float	box_alpha = 0.0f;
	render.scene.hit_box_depth(ray, render.bvh_debug.min_depth,
		render.bvh_debug.max_depth, t_geom, box_color, box_alpha);
	return ((base * (1.0f - box_alpha) + box_color * box_alpha));
}

bool	evaluate_blackholes(Ray& ray, const Render& render, Vec3f &accumulated_light, Vec3f throughput)
{
	Vec3f	bh_color;
	Ray		bent_ray;
	bool	consumed = render.black_hole.march(ray, bh_color, bent_ray);

	if (consumed)
	{
		accumulated_light += throughput * bh_color;
		return (false);
	}
	ray = bent_ray;
	return (true);
}

Vec3f	calculate_environment(const Ray& ray, const Render& render)
{
	if (render.hdri && render.env_map.isLoaded())
		return (render.env_map.sample(ray._dir));
	return (Vec3f(0.0f, 0.0f, 0.0f)); 
}

bool	apply_russian_roulette(Vec3f& throughput, unsigned int* seed)
{
	float	max_comp = std::fmax(throughput._x, std::fmax(throughput._y, throughput._z));

	if (max_comp < 0.01f)
		return (false);
	float rr = Vec3f::randomFloat(seed);
	if (rr > max_comp)
		return (false);
	throughput /= max_comp;
	return (true);
}

Vec3f	calculate_direct_lighting(const Ray& ray, const HitRecord& rec, const Render& render, Vec3f albedo, int* counter)
{
	Vec3f	total_light(0.0f);

	for (auto& light : render.scene.getLights())
	{
		Vec3f	light_pos = light->sample(rec.point, render.seed);
		Vec3f	to_light = light_pos - rec.point;
		float	dist = to_light.length();

		if (dist < 1e-6f)
			continue ;
		Vec3f	to_light_n = to_light / dist;
		float	cos_theta = Vec3f::dot(to_light_n, rec.normal);
		
		if (cos_theta <= 0.0f)
			continue ;

		Vec3f	transmit = shadow_transmittance(render.scene, rec.point, rec.normal, light_pos, counter);
		if (transmit.length_sq() < 1e-6f)
			continue ;

		float	pdf_light = light->pdf_value(rec.point, to_light_n);
		if (pdf_light <= 1e-6f)
			continue ;

		float	pdf_mat_eval = rec.material->scattering_pdf(ray, rec, Ray(rec.point, to_light_n));
		float	weight = power_heuristic(pdf_light, pdf_mat_eval);
		Vec3f	light_color = light->getMat()->emitted(rec.u, rec.v, light_pos);
		
		total_light += albedo * light_color * transmit * cos_theta * weight / pdf_light;
	}
	return (total_light);
}

Vec3f	apply_mis_weight(Vec3f emitted, const Ray& ray, const Render& render, float prev_pdf, bool prev_was_specular, int depth)
{
	if (!render.shadow_ray || depth == 0 || prev_was_specular)
		return (emitted);

	float	pdf_light = 0.0f;
	Vec3f	norm_dir = Vec3f::normalize(ray._dir);

	for (auto& light : render.scene.getLights())
		pdf_light += light->pdf_value(ray._o, norm_dir);

	if (pdf_light <= 0.0f)
		return (Vec3f(0.0f));
	
	float	weight = power_heuristic(prev_pdf, pdf_light);
	return (emitted * weight);
}
