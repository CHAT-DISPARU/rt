/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hittable.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:10:26 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/12 18:01:41 by gajanvie         ###   ########.fr       */
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
		virtual bool	hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const = 0;

		/*
			function bounding box
			- output_box = ref to fill
			return true if ok
			false (plane)
		*/
		virtual bool	bbox(AABB& output_box) const = 0;
		Material	*getMat() const
		{
			return (_mat);
		}

	protected :
		Material	*_mat;
};
