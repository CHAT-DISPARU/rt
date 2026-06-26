/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Plane.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 18:26:20 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/25 17:38:22 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"

class	Plane : public Hittable
{
	public:
		Plane(Vec3f point, Vec3f normal, const Mat4f &m, Material *mat);
		~Plane(){};
		bool	hit(const Ray& ray, float t_min, float t_max, HitRecord& rec, int* node_tests = nullptr) const;
		bool	bbox(AABB& output_box) const;
		const Mat4f&	getInverseTransform() const { return _inverse; }
		Vec3f			getPoint() const { return _point; }
		Vec3f			getNormal() const { return _normal; }

	private:
		Vec3f		_point;
		Mat4f		_inverse;
		Mat4f		_transform;
		Vec3f		_normal;
};
