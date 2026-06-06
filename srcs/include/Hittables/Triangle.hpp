/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Triangle.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 18:26:16 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/06 18:15:30 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"

class	Triangle : public Hittable
{
	public:
		Triangle(Vec3f points, Material *mat, Vec3f normal);
		Triangle(Vec3f points, Material *mat);
		~Triangle(){};
		bool	hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const;
		bool	bbox(AABB& output_box) const;

	private:
		Vec3f		_points;
		Mat4f		_transform;
		Material	*_mat;
		AABB		_box;
		Vec3f		_normal;
};