/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hittable.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:10:26 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/20 14:54:15 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Ray.hpp"
#include "HitRecord.hpp"
#include "AABB.hpp"
#include "Math3D.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <stdint.h>

class	Hittable
{
	public:
		virtual ~Hittable() = default;

		/*
			function intersection
			- ray
			- t_min / t_max intervale valid evite les auto intersections
			- rec = hitrecord rempli si objet touche

			return true si hit sinon false
		*/
		virtual bool	hit(const Ray& ray, float tMin, float tMax, HitRecord& rec, int* node_tests = nullptr) const = 0;

		/*
			function bounding box
			- output_box = ref to fill
			return true if ok
			false (plane)
		*/
		virtual bool	bbox(AABB& output_box) const = 0;
		virtual Vec3f	sample(const Vec3f& origin, uint32_t* seed) const
		{
			(void)seed;
			(void)origin;
			return (Vec3f(0.0f));
		}
		virtual float	pdf_value(const Vec3f& origin, const Vec3f& dir) const 
		{
			(void)origin;
			(void)dir;
			return (0.0f);
		}
		Material	*getMat() const
		{
			return (_mat);
		}
		virtual void hit_box_depth(const Ray& ray, int depth_min, int depth_max,
                           float t_geom, Vec3f& color_out, float& alpha_out, int current_depth = 0) const
		{
			(void)ray;
			(void)depth_min;
			(void)depth_max;
			(void)t_geom;
			(void)color_out;
			(void)alpha_out;
			(void)current_depth;
		}
		virtual int getMaxDepth() const
		{
			return 0;
		}

	protected :
		Material	*_mat = nullptr;
};
