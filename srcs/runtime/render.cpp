/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 10:51:20 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/21 08:50:45 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Render.hpp"
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
		Vec3f	mat_color = hit.material->getColor();
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

Vec3f	traceRay(Ray ray, const Render &render)
{
	Vec3f	accumulated_light(0.0f);
	Vec3f	throughput(1.0f);
	int		node_tests = 0;
	int*	counter = (render.bvh_debug.mode == BvhDebugMode::HEATMAP) ? &node_tests : nullptr;
	float	prev_pdf_scatter = 1.0f;
	bool	prev_was_specular = false;

	if (render.bvh_debug.mode == BvhDebugMode::DEPTH_SLICE)
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
	for (int depth = 0; depth < render.depth_max; ++depth)
	{

		if (render.black_hole_enabled)
		{
			Vec3f	bh_color;
			Ray		bent_ray;
			bool	consumed = render.black_hole.march(ray, bh_color, bent_ray);

			if (consumed)
			{
				accumulated_light += throughput * bh_color;
				break ;
			}
			ray = bent_ray;
		}
		HitRecord	rec;
		
		if (!render.scene.hit(ray, 1e-3f, FLT_MAX, rec, counter))
		{
			// ciel / soleil
			if (render.sun_light.enabled)
			{
				if (render.env_map.isLoaded())
					accumulated_light += throughput * render.env_map.sample(ray._dir);
			}
			break ;
		}

		// hit valide
		if (!rec.material)
		{
			accumulated_light += throughput * Vec3f(0.0f, 0.0f, 0.0f);
			break;
		}
		Vec3f	emitted = rec.material->emitted(rec.u, rec.v, rec.point);

		if (emitted.length_sq() > 0.0f)
		{
			if (!render.shadow_ray || depth == 0)
				accumulated_light += throughput * emitted;
			else
			{
				float weight = 1.0f;
				if (render.shadow_ray && depth > 0 && !prev_was_specular)
				{
					float	pdf_light = 0.0f;
					Vec3f	norm_dir = Vec3f::normalize(ray._dir);
					
					for (auto& light : render.scene.getLights())
						pdf_light += light->pdf_value(ray._o, norm_dir);

					if (pdf_light > 0.0f)
						weight = power_heuristic(prev_pdf_scatter, pdf_light);
					else
						weight = 0.0f;
				}
				accumulated_light += throughput * emitted * weight;
			}
			break; // émissif = fin du chemin
		}
		float	pdf_mat_scatter = 1.0f;
		Ray		new_ray;
		Vec3f	albedo;

		if (!rec.material->scatter(ray, rec, albedo, new_ray, pdf_mat_scatter, render.seed))
			break ;
		if (pdf_mat_scatter <= 1e-6f)
			break ;
		if (render.shadow_ray && rec.material->isOpaq() && !rec.material->isSpecular())
		{
			for (auto &light : render.scene.getLights())
			{
				AABB	box;
				light->bbox(box);
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
				//mis
				float	pdf_light = light->pdf_value(rec.point, to_light_n);
				if (pdf_light <= 1e-6f)
					continue;
				//proba lum
				float pdf_mat_eval = rec.material->scattering_pdf(ray, rec, Ray(rec.point, to_light_n));
				//poid Veach
				float weight_light = power_heuristic(pdf_light, pdf_mat_eval);
				
				Vec3f	light_color = light->getMat()->emitted(rec.u, rec.v, light_pos);
				//throughput -> rebond d avant 
				//albedo
				//light_color
				//transmit filtre verre si ya
				//cos_theta  beer lambert
				accumulated_light += throughput * albedo * light_color * transmit * cos_theta * weight_light / pdf_light;
			}
		}
		throughput *= albedo;

		// roulette russe
		if (depth > 3)
		{
			float	max_comp = std::fmax(throughput._x, std::fmax(throughput._y, throughput._z));
			if (max_comp < 0.01f)
				break ;
			float	rr = Vec3f::randomFloat(render.seed);
			if (rr > max_comp)
				break ;
			throughput /= max_comp;
		}
		prev_was_specular = rec.material->isSpecular();
		prev_pdf_scatter = pdf_mat_scatter;
		ray = new_ray;
	}
	if (render.bvh_debug.mode == BvhDebugMode::HEATMAP)
		return (color_heatmap(node_tests, render.bvh_debug.max_tests));
	return (accumulated_light);
}

void	render(Render &render_job)
{
	for (size_t y = render_job.start_y; y < render_job.end_y; ++y)
	{
		for (size_t x = render_job.start_x; x < render_job.end_x; ++x)
		{
			float	u, v;

			if (render_job.samples != 1)
			{
				u = ((float)x + Vec3f::randomFloat(render_job.seed)) * render_job.inv_w;
				v = ((float)y + Vec3f::randomFloat(render_job.seed)) * render_job.inv_h;
			}
			else
			{
				u = ((float)x + 0.5f) * render_job.inv_w;
				v = ((float)y + 0.5f) * render_job.inv_h;
			}
			Ray		ray = render_job.cam.getRay(u, v);
			Vec3f	color = traceRay(ray, render_job);
			size_t	idx = y * render_job.width + x;

			Vec3f	fc;
			if (render_job.bvh_debug.mode != BvhDebugMode::OFF)
				fc = color;
			else
			{
				if (render_job.frame_count == 1)
					render_job.accum_buffer[idx] = color;
				else
					render_job.accum_buffer[idx] += color;
				//moyenne
				fc = render_job.accum_buffer[idx] / (float)render_job.frame_count;
			}
			fc._x = std::fmax(0.0f, fc._x);
			fc._y = std::fmax(0.0f, fc._y);
			fc._z = std::fmax(0.0f, fc._z);
			//reinnhard loi tone mapping	gamma correction 
			fc._x = std::pow(fc._x / (fc._x + 1.0f), 1.0f / 2.2f);
			fc._y = std::pow(fc._y / (fc._y + 1.0f), 1.0f / 2.2f);
			fc._z = std::pow(fc._z / (fc._z + 1.0f), 1.0f / 2.2f);
			int	ir = (int)(255.999f * fc._x);
			int	ig = (int)(255.999f * fc._y);
			int	ib = (int)(255.999f * fc._z);
			render_job.definitive[idx] = (ir << 24) | (ig << 16) | (ib << 8) | 0xFF;
		}
	}
}
