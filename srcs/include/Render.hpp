/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:06:54 by gajanvie          #+#    #+#             */
/*   Updated: 2026/08/27 13:33:49 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Camera.hpp"
#include "EnvironmentMap.hpp"
#include "BlackHole.hpp"

class	Scene;
struct	BvhDebugConfig;
struct	HitRecord;
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
	bool				shadow_ray;
	EnvironmentMap		&env_map;
	bool				hdri;
	bool                bloom_enabled = true;
    float               bloom_threshold = 1.5f;
    float               bloom_intensity = 0.25f;
    int                 blur_radius = 15;
	bool				ru_enabled = true;
	bool				cpu = false;
	int					light_tech = 0;
};

void	render(Render &render);
Vec3f	calculate_environment(const Ray& ray, const Render& render);
bool	evaluate_blackholes(Ray& ray, const Render& render, Vec3f &accumulated_light, Vec3f throughput);
Vec3f	evaluate_depth_slice(const Ray& ray, const Render& render);
Vec3f	calculate_direct_lighting(const Ray& ray, const HitRecord& rec, const Render& render, Vec3f albedo, int* counter);
Vec3f	apply_mis_weight(Vec3f emitted, const Ray& ray, const Render& render, float prev_pdf, bool prev_was_specular, int depth);
bool	apply_russian_roulette(Vec3f& throughput, unsigned int* seed);
void	applyNormalMap(HitRecord& rec);
