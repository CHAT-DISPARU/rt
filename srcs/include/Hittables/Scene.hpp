/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scene.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:41:12 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/06 18:36:18 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"

class	Scene : public Hittable
{
	public:
		Scene(){};
		~Scene(){};
		bool	hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const;
		bool	bbox(AABB& output_box) const;

	private:
		std::vector<std::shared_ptr<Hittable> > _objs;
};