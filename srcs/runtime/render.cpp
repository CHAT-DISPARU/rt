/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:53:29 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/09 23:12:35 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Render.hpp"

Vec3f	traceRay(Ray ray, const Scene &scene, int max_depth, unsigned int *seed)
{
	Vec3f	accumulated_light(0.0f);
	Vec3f	throughput(1.0f);

	for (int depth = 0; depth < max_depth; depth++)
	{
		HitRecord	rec;

		if (scene.hit(ray, 0.001f, FLT_MAX, rec))
		{
			Vec3f	emitted = rec.material->emitted(rec.u, rec.v, rec.point);
			accumulated_light += throughput * emitted;

			Ray		new_ray;
			Vec3f	albedo;
			
			if (!rec.material->scatter(ray, rec, albedo, new_ray, seed))
				break;

			throughput *= albedo;
			ray = new_ray;
		}
		else
		{
			Vec3f	sky_color = Vec3f(0.05f, 0.05f, 0.05f);
			
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
			Vec3f	color = traceRay(ray, render.scene, render.depth_max, render.seed);

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
