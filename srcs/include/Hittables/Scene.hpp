/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scene.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:41:12 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/10 02:24:11 by CHAT-DISPAR      ###   ########.fr       */
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
		
		void build()
		{
			_bvh = std::make_shared<BVHNode>(_objects);
		};

	bool hit_naive(const Ray& ray, float t_min, float t_max, HitRecord& rec) const {
		HitRecord temp_rec;
		bool hit_anything = false;
		float closest_so_far = t_max;

		for (const auto& object : _objects) { // objects est ta liste de Hittable
			if (object->hit(ray, t_min, closest_so_far, temp_rec)) {
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}
		return hit_anything;
	}
		bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const
		{
			return (hit_naive(r, t_min, t_max, rec));
		};
		
		bool bbox(AABB& output_box) const
		{
			return (_bvh->bbox(output_box));
		};

	private:
		std::vector<std::shared_ptr<Hittable>> _objects;
		std::shared_ptr<BVHNode> _bvh;
};
