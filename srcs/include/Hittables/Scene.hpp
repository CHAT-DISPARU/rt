/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scene.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:41:12 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/10 03:22:16 by CHAT-DISPAR      ###   ########.fr       */
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

		bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const
		{
			return (_bvh->hit(r, t_min, t_max, rec));
		};
		
		bool bbox(AABB& output_box) const
		{
			return (_bvh->bbox(output_box));
		};

	private:
		std::vector<std::shared_ptr<Hittable>> _objects;
		std::shared_ptr<BVHNode> _bvh;
};
