/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Plane.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 18:26:20 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/10 17:55:37 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"

class	Plane : public Hittable
{
	public:
		Plane(Vec3f point, Vec3f normal, const Mat4f &m, Material *mat);
		~Plane(){};
		bool	hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const;
		bool	bbox(AABB& output_box) const;

	private:
		Vec3f		_point;
		Mat4f		_inverse;
		Mat4f		_transform;
		Material	*_mat;
		Vec3f		_normal;
};
