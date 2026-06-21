/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:06:54 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/20 13:06:14 by CHAT-DISPAR      ###   ########.fr       */
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
#include "EnvironmentMap.hpp"
#include "BlackHole.hpp"
#define THREAD_MAX 16

struct Render
{
	Render(Camera &c, Scene &s, BvhDebugConfig &dbg, EnvironmentMap &e) : cam(c), scene(s), bvh_debug(dbg), env_map(e)  {};
	~Render(){};
	size_t				start_y;
	size_t				end_y;
	size_t				start_x;
	size_t				end_x;

	bool				black_hole_enabled = false;
	BlackHole			black_hole = BlackHole(Vec3f(0, 0, 0), 1.0f);
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
	EnvironmentMap		&env_map;
};

void	render(Render &render);
