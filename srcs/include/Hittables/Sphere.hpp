/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sphere.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 18:26:18 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/10 17:47:17 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"

class	Sphere : public Hittable
{
	public:
		Sphere(float d, Vec3f center, Vec3f normal, const Mat4f &m, Material *mat);
		~Sphere(){};
		bool	hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const;
		bool	bbox(AABB& output_box) const;
				AABB		_box;
	private:
		Vec3f		_center;
		Mat4f		_inverse;
		Mat4f		_transform;
		float		_diameter;
		Material	*_mat;
		Vec3f		_normal;
		//AABB		_box;
};
