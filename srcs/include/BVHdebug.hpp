/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BVHdebug.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 11:34:34 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/25 18:04:21 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Render.hpp"


enum class	BvhDebugMode
{
	OFF,
	HEATMAP,
	DEPTH_SLICE
};

struct BvhDebugConfig
{
	BvhDebugMode	mode = BvhDebugMode::OFF;
	int		max_tests = 100;
	int		min_depth = 0;
	int		max_depth = 10;
	int		tree_depth = 0;
	bool		show_triangles = true;
	bool		show_spheres = true;
	bool		show_quads = true;
};

// bleu(0) → cyan(0.25) → vert(0.5) → jaune(0.75) → rouge(1)
inline Vec3f	color_heatmap(int tests, int max_tests)
{
	float	t = std::fmin(1.0f, (float)tests / (float)max_tests);
	Vec3f	c;

	if (t < 0.25f)
	{
		float s = t / 0.25f;
		c = Vec3f(0.0f, s, 1.0f);// bleu cyan
	}
	else if (t < 0.5f)
	{
		float s = (t - 0.25f) / 0.25f;
		c = Vec3f(0.0f, 1.0f, 1.0f - s);// cyan vert
	}
	else if (t < 0.75f)
	{
		float s = (t - 0.5f) / 0.25f;
		c = Vec3f(s, 1.0f, 0.0f);// vert jaune
	}
	else
	{
		float s = (t - 0.75f) / 0.25f;
		c = Vec3f(1.0f, 1.0f - s, 0.0f);//jaune rouge
	}
	return (c);
}