/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:06:54 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/19 10:42:19 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Camera.hpp"
#include "HitRecord.hpp"
#include "Hittable.hpp"
#include "Scene.hpp"
#include "ThreadPool.hpp"
#include "Sunlight.hpp"
#include "BVHdebug.hpp"
#define THREAD_MAX 16

struct Render
{
	Render(Camera &c, Scene &s, BvhDebugConfig &dbg) : cam(c), scene(s), bvh_debug(dbg) {};
	~Render(){};
	size_t				start_y;
	size_t				end_y;
	size_t				start_x;
	size_t				end_x;

	size_t				width;
	size_t				height;
	Camera				&cam;
	Scene				&scene;
	BvhDebugConfig		&bvh_debug;
	int					samples;
	int					depth_max;
	float				inv_w;
	float				inv_h;
	unsigned int		*seed;
	size_t				frame_count;
	Vec3f				*accum_buffer;
	uint32_t			*definitive;
	float				cam_speed = 0.2f;
	float				cam_rotate = 0.04f;
	SunLight			sun_light;
	bool				shadow_ray;
};

void	render(Render &render);
