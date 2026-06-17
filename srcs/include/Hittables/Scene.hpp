/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scene.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:41:12 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/17 12:54:36 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"
#include "BVHNode.hpp"

class	Scene : public Hittable
{
	public:

		void	add(std::shared_ptr<Hittable> object)
		{
			_objects.push_back(object);
		};
		
		void	add_light(std::shared_ptr<Hittable> object)
		{
			_objects.push_back(object);
			_light.push_back(object);
		};
		
		
		void	build()
		{
			_bvh = std::make_shared<BVHNode>(_objects);
		};

		bool	hit(const Ray& r, float t_min, float t_max, HitRecord& rec, int* node_tests = nullptr) const
		{
			return (_bvh->hit(r, t_min, t_max, rec, node_tests));
		};
		void	hit_box_depth(const Ray& r, int depth_min, int depth_max,
			float t_geom, Vec3f& color_out, float& alpha_out) const
		{
			_bvh->hit_box_depth(r, depth_min, depth_max, t_geom, color_out, alpha_out);
		};
		bool	bbox(AABB& output_box) const
		{
			return (_bvh->bbox(output_box));
		};
		std::vector<std::shared_ptr<Hittable>>	getLights() const
		{
			return (_light);
		};
		int getMaxDepth() const
		{
			if (!_bvh)
				return 0;
			return (_bvh->getMaxDepth());
		}
		

	private:
		std::vector<std::shared_ptr<Hittable>> _objects;
		std::vector<std::shared_ptr<Hittable>> _light;
		std::shared_ptr<BVHNode> _bvh;
};
