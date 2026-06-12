/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scene.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:41:12 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/12 23:59:39 by CHAT-DISPAR      ###   ########.fr       */
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
		
		
		void build()
		{
			_bvh = std::make_shared<BVHNode>(_objects);
		};

		bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const
		{
			return (_bvh->hit(r, t_min, t_max, rec));
		};

		bool hit_shadow(const Ray& r, float t_min, float t_max, HitRecord& rec) const
		{
			return (_bvh->hit_shadow(r, t_min, t_max, rec));
		};
		
		bool bbox(AABB& output_box) const
		{
			return (_bvh->bbox(output_box));
		};
		std::vector<std::shared_ptr<Hittable>>	getLights() const
		{
			return (_light);
		};
		

	private:
		std::vector<std::shared_ptr<Hittable>> _objects;
		std::vector<std::shared_ptr<Hittable>> _light;
		std::shared_ptr<BVHNode> _bvh;
};
