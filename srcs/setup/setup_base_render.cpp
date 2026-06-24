/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_base_render.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:29:02 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/23 19:06:47 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.hpp"

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
	render_total.hdri = false;
}

void	cleanup_render(Render &render_total)
{
	delete[] render_total.accum_buffer;
	delete[] render_total.definitive;
}
