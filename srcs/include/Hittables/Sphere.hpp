/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sphere.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 18:26:18 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/14 21:49:59 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"

class	Sphere : public Hittable
{
	public:
		Sphere(float d, Vec3f center, Vec3f normal, const Mat4f &m, Material *mat);
		~Sphere(){};
		bool	hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const;
		bool	bbox(AABB& output_box) const;
		Vec3f	sample(uint32_t *seed) const;
		float	pdf_value(const Vec3f& origin, const Vec3f& dir) const;
		
		AABB		_box;
	private:
		Vec3f		_center;
		Mat4f		_inverse;
		Mat4f		_transform;
		float		_diameter;
		Vec3f		_normal;
		float		_radius;
		//AABB		_box;
};
