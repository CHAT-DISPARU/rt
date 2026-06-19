/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quad.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 16:34:08 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/19 11:14:37 by gajanvie         ###   ########.fr       */
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

	private:
		Vec3f		_center;
		float		_w;
		float		_h;
		Mat4f		_inverse;
		Mat4f		_transform;
		Vec3f		_normal;
		AABB		_box;
};
