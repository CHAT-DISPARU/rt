/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:53:29 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/12 17:08:32 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Render.hpp"



bool	shadow_ray(const Scene &scene, HitRecord &rec, const Vec3f &target_pos)
{
	Vec3f	light_dir = target_pos - rec.point;
	float	light_dist = light_dir.length();
	Vec3f	dir_norm = Vec3f::normalize(light_dir);

	if (Vec3f::dot(dir_norm, rec.normal) <= 0.0f)
		return (false);

	Ray	shadow;
	
	shadow._o = rec.point + rec.normal * 0.001f;
	shadow._dir = dir_norm;

	if (!scene.hit_shadow(shadow, 0.001f, light_dist, rec))
		return (false);
	return (true);
}

Vec3f	traceRay(Ray ray, const Render &render)
{
	Vec3f	accumulated_light(0.0f);
	Vec3f	throughput(1.0f);
	for (int depth = 0; depth < render.depth_max; depth++)
	{
		HitRecord	rec;
		HitRecord	shadow_rec;

		if (render.scene.hit(ray, 0.001f, FLT_MAX, rec))
		{
			Vec3f	emitted = rec.material->emitted(rec.u, rec.v, rec.point);
			Ray		new_ray;
			Vec3f	albedo;
	
			accumulated_light += throughput * emitted;
			if (!rec.material->scatter(ray, rec, albedo, new_ray, render.seed))
				break ;
			if (render.shadow_ray)
			{
				for (auto& light : render.scene.getLights())
				{
					AABB	box;
					light->bbox(box);
					Vec3f	lightPos = box._min + Vec3f::randomFloat(render.seed) * (box._max - box._min);
					Vec3f	toLight = lightPos - rec.point;
					float	cosTheta = Vec3f::dot(Vec3f::normalize(toLight), rec.normal);

					if (cosTheta <= 0.0f)
						continue;
					HitRecord	shadow_rec;
					if (!shadow_ray(render.scene, shadow_rec, lightPos))
						continue ;
					Vec3f lightColor = shadow_rec.material->emitted(shadow_rec.u, shadow_rec.v, shadow_rec.point);

					accumulated_light += throughput * albedo  * lightColor * cosTheta;
				}
			}
			throughput *= albedo;
			
			ray = new_ray;
		}
		else if (render.sun_light.enabled)
		{
			//sky
			Vec3f	unit_dir = Vec3f::normalize(ray._dir);
			float	y = unit_dir._y;
			Vec3f	sky_color;
			if (y >= 0.0f)
			{
				float	t = y;
				float	t_sq = t * t;
				float	h = std::exp(-t * 8.0f);
				Vec3f	horizon_color(0.52f, 0.83f, 1.0f);
				Vec3f	low_color(0.40f, 0.65f, 0.98f);
				Vec3f	zenith_color(0.05f, 0.25f, 0.85f);
	
				sky_color = horizon_color * h + low_color * (1.0f - h)
							* (1.0f - t_sq) + zenith_color * t_sq;
			}
			else
			{
				float	t = -y;
				Vec3f	horizon_color(0.52f, 0.83f, 1.0f);
				Vec3f	ground_color(0.05f, 0.25f, 0.85f);

				sky_color = horizon_color * (1.0f - t) + ground_color * t;
			}

			float	sky_intensity = 0.5f; 
			sky_color = sky_color * sky_intensity;
			//sun
			float	alignment = std::fmax(0.0f, std::fmin(Vec3f::dot(unit_dir, render.sun_light.direction), 1.0f));
			float	disk = std::pow(alignment, render.sun_light.size) * render.sun_light.intensity;
			float	glow = std::pow(alignment, render.sun_light.glow_size) * render.sun_light.glow_intensity;
			Vec3f	sun_final_color = (render.sun_light.color * disk) + (render.sun_light.glow_color * glow);

			sky_color += sun_final_color;
			accumulated_light += throughput * sky_color;
			break ;
		}
		else
		{
			Vec3f sky_color(0.0f);
			accumulated_light += throughput * sky_color;
			break;
		}
	}
	return (accumulated_light);
}


void	render(Render &render)
{
	for (size_t y = render.start_y; y < render.end_y; y++)
	{
		for (size_t x = render.start_x; x < render.end_x; x++)
		{
			float	u, v;
			if (render.samples != 1)
			{
				u = ((float)x + Vec3f::randomFloat(render.seed)) * render.inv_w;
				v = ((float)y + Vec3f::randomFloat(render.seed)) * render.inv_h;
			}
			else
			{
				u = ((float)x + 0.5f) * render.inv_w;
				v = ((float)y + 0.5f) * render.inv_h;
			}

			Ray		ray = render.cam.getRay(u, v);
			Vec3f	color = traceRay(ray, render);

			size_t	pixel_idx = y * render.width + x;
			if (render.frame_count == 1)
				render.accum_buffer[pixel_idx] = color;
			else
				render.accum_buffer[pixel_idx] += color;

			//moyenne
			Vec3f	final_color = render.accum_buffer[pixel_idx] / (float)render.frame_count;

			final_color._x = final_color._x / (final_color._x + 1.0f);
			final_color._y = final_color._y / (final_color._y + 1.0f);
			final_color._z = final_color._z / (final_color._z + 1.0f);
			final_color._x = pow(final_color._x, 0.45454545454);
			final_color._y = pow(final_color._y, 0.45454545454);
			final_color._z = pow(final_color._z, 0.45454545454);
			int	ir, ig, ib;
			ir = (int)(255.999 * final_color._x);
			ig = (int)(255.999 * final_color._y);
			ib = (int)(255.999 * final_color._z);
			render.definitive[pixel_idx] = (ir << 24) | (ig << 16) | (ib << 8) | 0xFF;
		}
	}
}
