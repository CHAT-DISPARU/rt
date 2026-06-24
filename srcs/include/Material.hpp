/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:09:50 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/24 11:58:34 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Math3D.hpp"
#include "Ray.hpp"
#include "HitRecord.hpp"
#include <SDL3/SDL.h>

struct HitRecord;

Vec3f	sampleTextureFast(SDL_Surface* surface, float u, float v);
float	sampleScalarLinear(SDL_Surface* surface, float u, float v, int channel);

class Material
{
	public:

		virtual ~Material() = default;

		void	setColor(const Vec3f& color) { _color = color; }
		Vec3f	getColor() const { return _color; }

		virtual bool	scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, float& pdf, unsigned int* seed) const = 0;
		virtual float	scattering_pdf(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
		{
			(void)r_in;
			(void)rec;
			(void)scattered;
			return (0.0f);
		}

		virtual Vec3f	emitted(float u, float v, const Vec3f& p) const
		{
			(void)u;
			(void)v;
			(void)p;
			return Vec3f(0.0f, 0.0f, 0.0f);
		}

		virtual float	ior() const { return (1.0f); }
		virtual bool	isSpecular() const { return false; }
		virtual bool	isOpaq() const { return true; }

		void	setIntensity(float intensity) { _intensity = intensity; }
		float	getIntensity() const { return (_intensity); }

		void	setTexture(SDL_Surface* texture) { tex = texture; _hasTexture = (texture != nullptr); }
		SDL_Surface* getTexture() const { return tex; }
		bool	hasTexture() const { return _hasTexture; }
		void	setNormal(SDL_Surface* normalMap) { normal = normalMap; _hasNormal = (normalMap != nullptr); }
		SDL_Surface* getNormal() const { return normal; }
		bool	hasNormal() const { return _hasNormal; }
		void	setRoughness(SDL_Surface* roughnessMap) { roughness = roughnessMap; _hasRoughness = (roughnessMap != nullptr); }
		SDL_Surface* getRoughness() const { return roughness; }
		bool	hasRoughness() const { return _hasRoughness; }
		void	setMetallic(SDL_Surface* metallicMap) { metalic = metallicMap; _hasMetallic = (metallicMap != nullptr); }
		SDL_Surface* getMetallic() const { return metalic; }
		bool	hasMetallic() const { return _hasMetallic; }
		void	setOcclusion(SDL_Surface* occlusionMap) { occlusion = occlusionMap; _hasOcclusion = (occlusionMap != nullptr); }
		SDL_Surface* getOcclusion() const { return occlusion; }
		bool	hasOcclusion() const { return _hasOcclusion; }
		void	setEmissive(SDL_Surface* emissiveMap) { emissive = emissiveMap; _hasEmissive = (emissiveMap != nullptr); }
		SDL_Surface* getEmissive() const { return emissive; }
		bool	hasEmissive() const { return _hasEmissive; }
		void	setTexScale(float s)
		{
			_albedoScale = s;
			_normalScale = s;
			_roughnessScale = s;
			_metallicScale = s;
			_occlusionScale = s;
			_emissiveScale = s;
		}
		
		void	setAlbedoScale(float s) { _albedoScale = s; }
		void	setNormalScale(float s) { _normalScale = s; }
		void	setRoughnessScale(float s) { _roughnessScale = s; }
		void	setMetallicScale(float s) { _metallicScale = s; }
		void	setOcclusionScale(float s) { _occlusionScale = s; }
		void	setEmissiveScale(float s) { _emissiveScale = s; }
		void	setNormalStrength(float s) { _normalStrength = s; }
		float	getNormalStrength() const { return _normalStrength; }
		float	getAlbedoScale() const { return _albedoScale; }
		float	getNormalScale() const { return _normalScale; }
		float	getRoughnessScale() const { return _roughnessScale; }
		float	getMetallicScale() const { return _metallicScale; }
		float	getOcclusionScale() const { return _occlusionScale; }
		float	getEmissiveScale() const { return _emissiveScale; }

		Vec3f	sampleAlbedo(float u, float v) const
		{
			if (!hasTexture())
				return (_color);
			
			float	su = u * _albedoScale - std::floor(u * _albedoScale);
			float	sv = v * _albedoScale - std::floor(v * _albedoScale);
			return (sampleTextureFast(tex, su, sv));
		}

		float	sampleRoughness(float u, float v, float default_val) const
		{
			if (!hasRoughness())
				return (default_val);
			
			float	su = u * _roughnessScale - std::floor(u * _roughnessScale);
			float	sv = v * _roughnessScale - std::floor(v * _roughnessScale);
			float	val = sampleScalarLinear(roughness, su, sv, 0);
			
			return (val < 0.0f ? default_val : val * default_val);
		}

		float	sampleMetallic(float u, float v, float default_val) const
		{
			if (!hasMetallic())
				return (default_val);
			
			float	su = u * _metallicScale - std::floor(u * _metallicScale);
			float	sv = v * _metallicScale - std::floor(v * _metallicScale);
			float	val = sampleScalarLinear(metalic, su, sv, 0);
			
			return (val < 0.0f ? default_val : val * default_val);
		}

		float	sampleAO(float u, float v) const
		{
			if (!hasOcclusion())
				return (1.0f);
			
			float	su = u * _occlusionScale - std::floor(u * _occlusionScale);
			float	sv = v * _occlusionScale - std::floor(v * _occlusionScale);
			float	val = sampleScalarLinear(occlusion, su, sv, 0);
			
			return (val < 0.0f ? 1.0f : val);
		}

		Vec3f	sampleEmissive(float u, float v) const
		{
			if (!hasEmissive())
				return (_color * _intensity);
			
			float	su = u * _emissiveScale - std::floor(u * _emissiveScale);
			float	sv = v * _emissiveScale - std::floor(v * _emissiveScale);
			
			return (sampleTextureFast(emissive, su, sv) * _intensity);
		}

	protected:
		Vec3f	_color;
		float	_intensity = 1.0f;
		
		bool	_hasTexture = false;
		bool	_hasNormal = false;
		bool	_hasRoughness = false;
		bool	_hasMetallic = false;
		bool	_hasOcclusion = false;
		bool	_hasEmissive = false;
		
		SDL_Surface	*tex = nullptr;
		SDL_Surface	*roughness = nullptr;
		SDL_Surface	*metalic = nullptr;
		SDL_Surface	*occlusion = nullptr;
		SDL_Surface	*emissive = nullptr;
		SDL_Surface	*normal = nullptr;
		
		float	_albedoScale = 1.0f;
		float	_normalScale = 1.0f;
		float	_roughnessScale = 1.0f;
		float	_metallicScale = 1.0f;
		float	_occlusionScale = 1.0f;
		float	_emissiveScale = 1.0f;
		float	_normalStrength = 1.0f;
};