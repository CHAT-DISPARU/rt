/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Triangle.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 18:26:16 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/20 13:06:03 by CHAT-DISPAR      ###   ########.fr       */
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
		void	translate(const Vec3f& offset)
		{
			_v0 += offset;
			_v1 += offset;
			_v2 += offset;
			_box = AABB();
			_box.add_point(_v0);
			_box.add_point(_v1);
			_box.add_point(_v2);
			_box._min -= Vec3f(0.001f);
			_box._max += Vec3f(0.001f);
		}
		void applyTransform(const Mat4f& m)
		{
			_v0 = Vec3f::Point_Mult_mat4(_v0, m);
			_v1 = Vec3f::Point_Mult_mat4(_v1, m);
			_v2 = Vec3f::Point_Mult_mat4(_v2, m);

			Vec3f edge1 = _v1 - _v0;
			Vec3f edge2 = _v2 - _v0;
			_normal = Vec3f::normalize(Vec3f::cross(edge1, edge2));
			_box = AABB();
			_box.add_point(_v0);
			_box.add_point(_v1);
			_box.add_point(_v2);
			_box._min -= Vec3f(0.001f);
			_box._max += Vec3f(0.001f);
		}
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