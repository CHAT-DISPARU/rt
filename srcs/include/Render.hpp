/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:06:54 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/09 14:11:02 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Camera.hpp"
#include "HitRecord.hpp"
#include "Hittable.hpp"
#include "Scene.hpp"

struct Render
{
	size_t				start_y;
	size_t				end_y;
	size_t				start_x;
	size_t				end_x;
	size_t				width;
	Camera				&cam;
	Scene				&scene;
	bool				samples;
	size_t				depth_max;
	float				inv_w;
	float				inv_h;
	unsigned int		*seed;
	size_t				frame_count;
	Vec3f				*accum_buffer;
};
