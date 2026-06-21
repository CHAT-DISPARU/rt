/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PBRMaterial.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 11:54:38 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/20 13:41:30 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Material.hpp"

class PBRMaterial : public Material
{
public:
	bool	scatter(const Ray& r_in, const HitRecord& rec,
					Vec3f& attenuation, Ray& scattered,
					float& pdf, unsigned int* seed) const;

	float	scattering_pdf(const Ray& r_in, const HitRecord& rec,
							const Ray& scattered) const;

	Vec3f	emitted(float u, float v, const Vec3f& p) const override;

	bool	isSpecular() const override
	{
		return (false);
	}

	void	setMetallicScalar(float m)
	{
		_metallic_scalar = m;
	}
	void	setRoughnessScalar(float r)
	{
		_roughness_scalar = r;
	}

private:
	float	_metallic_scalar  = 0.0f;
	float	_roughness_scalar = 0.5f;
	float	sampleRoughness(float u, float v) const;
	float	sampleMetallic(float u, float v) const;
	float	sampleAO(float u, float v) const;
	Vec3f	sampleNormalTangentSpace(float u, float v) const;
};