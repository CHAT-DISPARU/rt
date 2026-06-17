/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Triangle.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 18:26:16 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/17 12:41:11 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"

class	Triangle : public Hittable
{
	public:
		Triangle(Vec3f v0, Vec3f v1, Vec3f v2, Material *mat, Vec3f normal);
		Triangle(Vec3f v0, Vec3f v1, Vec3f v2, Material *mat);
		~Triangle(){};
		bool	hit(const Ray& ray, float t_min, float t_max, HitRecord& rec, int* node_tests = nullptr) const;
		bool	bbox(AABB& output_box) const;
		Vec3f	sample(const Vec3f& origin, uint32_t* seed) const;
		float	pdf_value(const Vec3f& origin, const Vec3f& dir) const;

	private:
		Vec3f		_v0;
		Vec3f		_v1;
		Vec3f		_v2;
		Mat4f		_transform;
		AABB		_box;
		Vec3f		_normal;
};