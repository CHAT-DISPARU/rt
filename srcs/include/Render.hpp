/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:06:54 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/16 15:16:08 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Camera.hpp"
#include "HitRecord.hpp"
#include "Hittable.hpp"
#include "Scene.hpp"
#include "ThreadPool.hpp"
#include "Sunlight.hpp"
#define THREAD_MAX 16

enum class	BvhDebugMode
{
	OFF,
	HEATMAP,
	DEPTH_SLICE
};

struct BvhDebugConfig
{
	BvhDebugMode mode = BvhDebugMode::OFF;
	int	min_depth = 0;
	int	max_depth = 5;
	int	max_tests_heatmap = 35;
};

struct Render
{
	Render(Camera &c, Scene &s, BvhDebugConfig &bvh) : cam(c), scene(s), bvh_config(bvh) {};
	~Render(){};
	size_t				start_y;
	size_t				end_y;
	size_t				start_x;
	size_t				end_x;

	BvhDebugConfig		&bvh_config;
	size_t				width;
	size_t				height;
	Camera				&cam;
	Scene				&scene;
	int					samples;
	int					depth_max;
	float				inv_w;
	float				inv_h;
	unsigned int		*seed;
	size_t				frame_count;
	Vec3f				*accum_buffer;
	uint32_t			*definitive;
	float				cam_speed  = 0.2f;
	float				cam_rotate = 0.04f;
	SunLight			sun_light;
	bool				shadow_ray;
};

void	render(Render &render);