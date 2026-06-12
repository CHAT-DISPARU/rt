/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_base_render.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:29:02 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/12 17:07:07 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.hpp"

void	set_sun(SunLight &sun)
{
	sun.direction = Vec3f::normalize(Vec3f(1.0f, 1.5f, -2.0f));
	sun.color = Vec3f(1.0f, 0.65f, 0.0f);
	sun.glow_color = Vec3f(1.0f, 0.50f, 0.0f);
	sun.intensity = 3.0f;
	sun.glow_intensity = 0.08f;
	sun.size = 500.0f;
	sun.glow_size = 12.0f;
	sun.enabled = true;
}

void	setup_base_render(AppContext &app, Render &render_total)
{
	size_t		pixel_count = app.width * app.height;
	Vec3f*		accum_buffer = new Vec3f[pixel_count]();
	uint32_t*	definitive = new uint32_t[pixel_count]();

	render_total.shadow_ray = false;
	render_total.width = app.width;
	render_total.height = app.height;
	render_total.inv_w = 1.0f / (float)app.width;
	render_total.inv_h = 1.0f / (float)app.height;
	render_total.depth_max = 50;
	render_total.samples = app.samples;
	render_total.frame_count = 1;
	render_total.accum_buffer = accum_buffer;
	render_total.definitive = definitive;
	set_sun(render_total.sun_light);
}

void	cleanup_render(Render &render_total)
{
	delete[] render_total.accum_buffer;
	delete[] render_total.definitive;
}
