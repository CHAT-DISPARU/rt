/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:06:54 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/09 23:12:51 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Camera.hpp"
#include "HitRecord.hpp"
#include "Hittable.hpp"
#include "Scene.hpp"
#define THREAD_MAX 16

struct Render
{
	Render(Camera &c, Scene &s) : cam(c), scene(s) {};
	~Render(){};
	size_t				start_y;
	size_t				end_y;
	size_t				start_x;
	size_t				end_x;

	size_t				width;
	size_t				height;
	Camera				&cam;
	Scene				&scene;
	int					samples;
	size_t				depth_max;
	float				inv_w;
	float				inv_h;
	unsigned int		*seed;
	size_t				frame_count;
	Vec3f				*accum_buffer;
	uint32_t			*definitive;
};

void	render(Render &render);