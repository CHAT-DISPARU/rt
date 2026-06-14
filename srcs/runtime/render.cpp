/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 10:51:20 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/14 23:12:03 by CHAT-DISPAR      ###   ########.fr       */
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
							const Vec3f &target_pos)
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

		if (!scene.hit(shadow, 1e-4f, light_dist, hit))
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

	for (int depth = 0; depth < render.depth_max; ++depth)
	{
		HitRecord	rec;

		if (!render.scene.hit(ray, 1e-3f, FLT_MAX, rec))
		{
			// ciel / soleil
			if (render.sun_light.enabled)
			{
				Vec3f	unit_dir = Vec3f::normalize(ray._dir);
				float	y = unit_dir._y;
				Vec3f	sky_color;

				if (y >= 0.0f)
				{
					float	t = y;
					float	t_sq = t * t;
					float	h = std::exp(-t * 8.0f);
					Vec3f	horizon(0.52f, 0.83f, 1.0f);
					Vec3f	low(0.40f, 0.65f, 0.98f);
					Vec3f	zenith(0.05f, 0.25f, 0.85f);

					sky_color = horizon * h + low * ((1.0f - h) * (1.0f - t_sq))
								+ zenith * t_sq;
				}
				else
				{
					float	t = -y;
					Vec3f	horizon(0.52f, 0.83f, 1.0f);
					Vec3f	ground(0.05f, 0.15f, 0.35f);

					sky_color = horizon * (1.0f - t) + ground * t;
				}
				sky_color *= 0.5f;

				float	alignment = std::fmax(0.0f, Vec3f::dot(unit_dir, render.sun_light.direction));
				float	disk = std::pow(alignment, render.sun_light.size) * render.sun_light.intensity;
				float	glow = std::pow(alignment, render.sun_light.glow_size) * render.sun_light.glow_intensity;
				Vec3f	sun_color = render.sun_light.color * disk
									+ render.sun_light.glow_color * glow;

				accumulated_light += throughput * (sky_color + sun_color);
			}
			break ;
		}

		// hit valide
		Vec3f	emitted = rec.material->emitted(rec.u, rec.v, rec.point);
		Ray		new_ray;
		Vec3f	albedo;

		if (depth == 0 || !render.shadow_ray)
			accumulated_light += throughput * emitted;
		float	pdf_mat_scatter = 1.0f;
		if (!rec.material->scatter(ray, rec, albedo, new_ray, pdf_mat_scatter, render.seed))
			break ;
		if (pdf_mat_scatter <= 1e-6f)
			break ;
		if (render.shadow_ray && rec.material->isOpaq())
		{
			for (auto &light : render.scene.getLights())
			{
				AABB	box;
				light->bbox(box);
				Vec3f	light_pos = light->sample(render.seed);

				Vec3f	to_light = light_pos - rec.point;
				float	dist = to_light.length();
				if (dist < 1e-6f)
					continue ;
				Vec3f	to_light_n = to_light / dist;
				float	cos_theta = Vec3f::dot(to_light_n, rec.normal);

				if (cos_theta <= 0.0f)
					continue ;

				Vec3f	transmit = shadow_transmittance(render.scene, rec.point, rec.normal, light_pos);

				if (transmit.length_sq() < 1e-6f)
					continue ;
				//mis
				float	pdf_light = light->pdf_value(rec.point, to_light_n);
				if (pdf_light <= 0.0f)
					continue;
				//proba lum
				float pdf_mat_eval = rec.material->scattering_pdf(ray, rec, Ray(rec.point, to_light_n));
				//poid Veach
				float weight_light = power_heuristic(pdf_light, pdf_mat_eval);
				
				Vec3f light_color = light->getMat()->getColor();
				//throughput -> rebond d avant 
				//albedo
				//light_color
				//transmit filtre verre si ya
				//cos_theta  beer lambert
				accumulated_light += throughput * albedo * light_color * transmit * cos_theta * weight_light / pdf_light;
			}
			if (render.sun_light.enabled)
			{
				Vec3f	sun_dir = -render.sun_light.direction; 
				float	cos_theta = Vec3f::dot(sun_dir, rec.normal);

				if (cos_theta > 0.0f)
				{
					Vec3f	sun_target = rec.point + sun_dir * 1e5f;
					Vec3f	transmit = shadow_transmittance(render.scene, rec.point, rec.normal, sun_target);

					if (transmit.length_sq() > 0.0f)
					{
						Vec3f	sun_contribution = render.sun_light.color * render.sun_light.intensity;
						accumulated_light += throughput * albedo * sun_contribution * transmit * cos_theta;
					}
				}
			}
		}
		if (rec.material->isSpecular()) 
			throughput *= albedo; 
		else 
		{
			float	cos_theta_indirect = Vec3f::dot(rec.normal, Vec3f::normalize(new_ray._dir));
			if (cos_theta_indirect < 0.0f) 
				cos_theta_indirect = 0.0f;
				
			Vec3f	brdf = albedo / (float)M_PI;
			throughput *= (brdf * cos_theta_indirect) / pdf_mat_scatter;
		}

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

		ray = new_ray;
	}

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

			if (render_job.frame_count == 1)
				render_job.accum_buffer[idx] = color;
			else
				render_job.accum_buffer[idx] += color;
			//moyenne
			Vec3f	fc = render_job.accum_buffer[idx] / (float)render_job.frame_count;
			//reinnhard loi tone mapping
			fc._x = std::pow(fc._x / (fc._x + 1.0f), 1.0f / 2.2f);
			fc._y = std::pow(fc._y / (fc._y + 1.0f), 1.0f / 2.2f);
			fc._z = std::pow(fc._z / (fc._z + 1.0f), 1.0f / 2.2f);
			//gamma correction 
			int	ir = (int)(255.999f * fc._x);
			int	ig = (int)(255.999f * fc._y);
			int	ib = (int)(255.999f * fc._z);
			render_job.definitive[idx] = (ir << 24) | (ig << 16) | (ib << 8) | 0xFF;
		}
	}
}