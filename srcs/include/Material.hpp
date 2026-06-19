/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:09:50 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/19 10:50:41 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Math3D.hpp"
#include "Ray.hpp"
#include "HitRecord.hpp"
#include <SDL3/SDL.h>

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
		virtual float ior() const
		{
			return (1.0f);
		};
		virtual bool	isSpecular() const
		{
			return false;
		};

		virtual bool	isOpaq() const
		{
			return true;
		};
		void	setIntensity(float intensity)
		{
			_intensity = intensity;
		};

		float	getIntensity() const
		{
			return (_intensity);
		};
		void setTexture(SDL_Surface* texture)
		{
			tex = texture;
			_hasTexture = (texture != nullptr);
		}

		SDL_Surface* getTexture() const
		{
			return tex;
		}

		bool hasTexture() const
		{
			return _hasTexture;
		}
		void setNormal(SDL_Surface* normalMap)
		{
			normal = normalMap;
			_hasNormal = (normalMap != nullptr);
		}

		SDL_Surface* getNormal() const
		{
			return normal;
		}

		bool hasNormal() const
		{
			return _hasNormal;
		}
		void setRoughness(SDL_Surface* roughnessMap)
		{
			roughness = roughnessMap;
			_hasRoughness = (roughnessMap != nullptr);
		}

		SDL_Surface* getRoughness() const
		{
			return roughness;
		}

		bool hasRoughness() const
		{
			return _hasRoughness;
		}
		void setMetallic(SDL_Surface* metallicMap)
		{
			metalic = metallicMap;
			_hasMetallic = (metallicMap != nullptr);
		}

		SDL_Surface* getMetallic() const
		{
			return metalic;
		}

		bool hasMetallic() const
		{
			return _hasMetallic;
		}

		void setOcclusion(SDL_Surface* occlusionMap)
		{
			occlusion = occlusionMap;
			_hasOcclusion = (occlusionMap != nullptr);
		}

		SDL_Surface* getOcclusion() const
		{
			return occlusion;
		}

		bool hasOcclusion() const
		{
			return _hasOcclusion;
		}

		void setEmissive(SDL_Surface* emissiveMap)
		{
			emissive = emissiveMap;
			_hasEmissive = (emissiveMap != nullptr);
		}

		SDL_Surface* getEmissive() const
		{
			return emissive;
		}

		bool hasEmissive() const
		{
			return _hasEmissive;
		}
		void	setTexScale(float texScale)
		{
			_texScale = texScale;
		};
		float	getTexScale() const
		{
			return (_texScale);
		};
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
			float		_texScale = 1.0f;
			
};