/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:09:50 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/12 09:20:02 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Math3D.hpp"
#include "Ray.hpp"
#include "HitRecord.hpp"

struct HitRecord;

class Material
{
	public:

		virtual ~Material() = default;
		void	setColor(const Vec3f& color)
		{
			_color = color;
		};
		Vec3f	getColor() const
		{
			return _color;
		};
		/*
			- r_in : ray qui arrive (incident)
			- rec : le HitRecord
			- attenuation : couleur mat
			- scattered : nouveau rebond
			true si rebond false si absorbe
		*/
		virtual bool	scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, unsigned int* seed) const = 0;

		virtual Vec3f	emitted(float u, float v, const Vec3f& p) const
		{
			(void)u;
			(void)v;
			(void)p;
			return Vec3f(0.0f, 0.0f, 0.0f);
		}
		virtual float ior() const
		{
			return (1.0f);
		};
		virtual bool	isSpecular() const
		{
			return false;
		};

		protected:
			Vec3f	_color;
			bool	_hasTexture = false;
			bool	_hasNormal = false;
			bool	_hasRoughness = false;
			bool	_hasMetallic = false;
			bool	_hasOcclusion = false;
			bool	_hasEmissive = false;
};