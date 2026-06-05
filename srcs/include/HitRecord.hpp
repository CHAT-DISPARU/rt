/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HitRecord.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 21:27:50 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/05 17:11:00 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Math3D.hpp"
#include "Ray.hpp"

struct HitRecord
{
	float		t;
	Ray			l_ray;
	Vec3f		local_normal;
	Vec3f		point;
	bool		front_face;
	Material	*material;

	void	set_front_face(const Ray &r, const Vec3f &out_normal)
	{
		front_face = Vec3f::dot(r.dir, out_normal) < 0.0f;
		local_normal = front_face ? out_normal : -out_normal;
	};
};
