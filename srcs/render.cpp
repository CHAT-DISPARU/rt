/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:53:29 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/09 14:11:04 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Render.hpp"


Vec3f	traceRay(Ray ray, Scene scene, int depth, unsigned int *seed)
{
	if (depth <= 0)
        return (Vec3f());
	
	HitRecord	rec;
	Vec3f		color;
	if (scene.hit(ray, FLT_EPSILON, FLT_MAX, rec))
	{
		Ray		new_ray;

		if (!rec.material->scatter(ray, rec, color, new_ray, seed))
			return (Vec3f());
		else
			color += traceRay(new_ray, scene, depth - 1, seed);
	}
	return (color);
}



void	render(Render &render)
{
	if (render.start_y == 0 && render.start_x == 0 && render.cam.hasMoved())
	{
		render.frame_count = 1;
		render.cam.resetMovedFlag();
	}

	for (size_t y = render.start_y; y < render.end_y; y++)
	{
		for (size_t x = render.start_x; x < render.end_x; x++)
		{
			float u, v;
			if (render.samples)
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

			size_t	pixel_index = y * render.width + x;
			if (render.frame_count == 1)
				render.accum_buffer[pixel_index] = color;
			else
				render.accum_buffer[pixel_index] += color;

			//moyenne
			Vec3f final_color = render.accum_buffer[pixel_index] / (float)render.frame_count;

		}
	}
	if (render.start_y == 0 && render.start_x == 0) 
		render.frame_count++;
}
