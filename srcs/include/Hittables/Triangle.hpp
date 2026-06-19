/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Triangle.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 18:26:16 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/19 12:17:52 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"

class	Triangle : public Hittable
{
	public:
		Triangle(Vec3f v0, Vec3f v1, Vec3f v2, Material *mat, Vec3f normal);
		Triangle(Vec3f v0, Vec3f v1, Vec3f v2, Material *mat);
		Triangle(Vec3f v0, Vec3f v1, Vec3f v2, Material* mat,
					float uv0[2], float uv1[2], float uv2[2])
			: Triangle(v0, v1, v2, mat)
		{
			_uv0[0] = uv0[0]; _uv0[1] = uv0[1];
			_uv1[0] = uv1[0]; _uv1[1] = uv1[1];
			_uv2[0] = uv2[0]; _uv2[1] = uv2[1];
			_has_uv = true;
		}
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
		float		_uv0[2], _uv1[2], _uv2[2];
		bool		_has_uv = false;
};