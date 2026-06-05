/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:09:50 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/05 18:26:25 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Math3D.hpp"
#include "Ray.hpp"
struct HitRecord;

class Material
{
	public:

		virtual ~Material() = default;

		/*
			- r_in : ray qui arrive (incident)
			- rec : le HitRecord
			- attenuation : couleur mat
			- scattered : nouveau rebond
			true si rebond false si absorbe
		*/
		virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered) const = 0;

		virtual Vec3f emitted(float u, float v, const Vec3f& p) const
		{
			(void)u;
			(void)v;
			(void)p;
			return Vec3f(0.0f, 0.0f, 0.0f);
		}
};