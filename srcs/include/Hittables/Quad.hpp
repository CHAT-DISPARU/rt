/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quad.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 16:34:08 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/25 17:38:40 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"

class	Quad : public Hittable
{
	public:
		Quad(Vec3f center, Vec3f normal, const Mat4f &m, Material *mat, float w, float h);
		~Quad(){};
		bool	hit(const Ray& ray, float t_min, float t_max, HitRecord& rec, int* node_tests = nullptr) const;
		bool	bbox(AABB& output_box) const;
		Vec3f	sample(const Vec3f& origin, uint32_t* seed) const;
		float	pdf_value(const Vec3f& origin, const Vec3f& dir) const;
		const Mat4f&	getInverseTransform() const { return _inverse; }
		Vec3f			getCenter() const { return _center; }
		Vec3f			getNormal() const { return _normal; }
		float			getW() const { return _w; }
		float			getH() const { return _h; }

	private:
		Vec3f		_center;
		float		_w;
		float		_h;
		Mat4f		_inverse;
		Mat4f		_transform;
		Vec3f		_normal;
		AABB		_box;
};
