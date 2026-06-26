/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 12:27:17 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/25 17:39:52 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"
#include "BVHNode.hpp"
#include "Triangle.hpp"
#include <vector>
#include <memory>

class	Mesh : public Hittable
{
	public:
		Mesh() = default;
		void	addTriangle(std::shared_ptr<Triangle> tri)
		{
			_triangles.push_back(tri);
		}

		void	build()
		{
			std::vector<std::shared_ptr<Hittable>> objs(_triangles.begin(), _triangles.end());
			_bvh = std::make_shared<BVHNode>(objs);
			_bvh->bbox(_box);
		}

		bool	hit(const Ray& ray, float tMin, float tMax, HitRecord& rec, int* counter = nullptr) const override
		{
			if (!_bvh)
				return false;
			return _bvh->hit(ray, tMin, tMax, rec, counter);
		}

		bool	bbox(AABB& output_box) const override
		{
			output_box = _box;
			return true;
		}

		void	setTransform(const Mat4f& m)
		{
			_transform = m;
			for (auto& tri : _triangles)
				tri->applyTransform(m);
			build();
		}

		void	translate(const Vec3f& offset)
		{
			Mat4f t = Mat4f::translate(offset) * _transform;
			setTransform(t);
		}

		void	rotate(const Vec3f& axis, float angle)
		{
			Mat4f r = Mat4f::rotate(angle, axis) * _transform;
			setTransform(r);
		}

		void	scale(float s)
		{
			Mat4f sc = Mat4f::scale(Vec3f(s)) * _transform;
			setTransform(sc);
		}

		size_t	triCount() const
		{
			return _triangles.size();
		}
		void hit_box_depth(const Ray& ray, int depth_min, int depth_max, 
				   float t_geom, Vec3f& color_out, float& alpha_out, 
				   int current_depth = 0) const override
		{
			if (_bvh)
				_bvh->hit_box_depth(ray, depth_min, depth_max, t_geom, color_out, alpha_out, current_depth);
		}
		int	getMaxDepth() const override 
		{
			if (_bvh)
				return _bvh->getMaxDepth();
			return 0;
		}
		const std::vector<std::shared_ptr<Triangle>>&	getTriangles() const { return _triangles; }
	private:
		std::vector<std::shared_ptr<Triangle>>	_triangles;
		std::shared_ptr<BVHNode>	_bvh;
		AABB						_box;
		Mat4f						_transform;
};
