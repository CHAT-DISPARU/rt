/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HitRecord.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 21:27:50 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/06 14:00:50 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Math3D.hpp"
#include "Ray.hpp"
#include "Material.hpp"

struct	HitRecord
{
	float		t;
	float		u;
	float		v;
	Vec3f		normal;
	Vec3f		point;
	bool		front_face;
	Material	*material;

	void	set_face_normal(const Ray &r, const Vec3f &outward_normal)
	{
		front_face = Vec3f::dot(r._dir, outward_normal) < 0.0f;
		normal = front_face ? outward_normal : -outward_normal;
	};
};
