/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:53:29 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/09 11:32:12 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Render.hpp"

void	render(Render &render)
{
	float	u, v;
	for (int y = render.start_y; y < render.end_y; y++)
	{
		for (int x = render.start_x; x < render.end_x; x++)
		{
			Vec3f	color;
			Ray		ray;

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
			ray = render.cam.getRay(u, v);
			color = traceRay();
			//tab sdl set color
		}
	}
	
}