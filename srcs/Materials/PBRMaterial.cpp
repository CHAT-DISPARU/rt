/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PBRMaterial.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 12:03:32 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/20 20:53:14 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PBRMaterial.hpp"
#include "rt.hpp"

float	PBRMaterial::sampleRoughness(float u, float v) const
{
	if (!hasRoughness())
		return (_roughness_scalar);
	float	val = sampleScalarLinear(roughness, u * _texScale, v * _texScale, 0);
	return (val < 0.0f ? _roughness_scalar : val);
}

float	PBRMaterial::sampleMetallic(float u, float v) const
{
	if (!hasMetallic())
		return (_metallic_scalar);
	float	val = sampleScalarLinear(metalic, u * _texScale, v * _texScale, 0);
	return (val < 0.0f ? _metallic_scalar : val);
}

float	PBRMaterial::sampleAO(float u, float v) const
{
	if (!hasOcclusion())
		return (1.0f);
	float	val = sampleScalarLinear(occlusion, u * _texScale, v * _texScale, 0);
	return (val < 0.0f ? 1.0f : val);
}

Vec3f	PBRMaterial::sampleNormalTangentSpace(float u, float v) const
{
	if (!hasNormal())
		return (Vec3f(0.0f, 0.0f, 1.0f));

	u = u * _texScale - std::floor(u * _texScale);
	v = v * _texScale - std::floor(v * _texScale);
	int	x = std::min((int)(u * normal->w), normal->w - 1);
	int	y = std::min((int)((1.0f - v) * normal->h), normal->h - 1);
	Uint8*	pixels = (Uint8*)normal->pixels;
	int		bpp = SDL_BYTESPERPIXEL(normal->format);
	Uint8*	p = pixels + y * normal->pitch + x * bpp;

	return (Vec3f(
		p[0] / 255.0f * 2.0f - 1.0f,
		p[1] / 255.0f * 2.0f - 1.0f,
		p[2] / 255.0f * 2.0f - 1.0f
	));
}


static float	distribution_ggx(const Vec3f& N, const Vec3f& H, float roughness)
{
	float	a = roughness * roughness;
	float	a2 = a * a;
	float	NdotH = std::fmax(Vec3f::dot(N, H), 0.0f);
	float	NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = (float)M_PI * denom * denom;
	return (a2 / std::fmax(denom, 1e-6f));
}

static float	geometry_smith(const Vec3f& N, const Vec3f& V, const Vec3f& L, float roughness)
{
	float	r = roughness + 1.0f;
	float	k = (r * r) / 8.0f; //karis
	float	NdotV = std::fmax(Vec3f::dot(N, V), 0.0f);
	float	NdotL = std::fmax(Vec3f::dot(N, L), 0.0f);

	float	ggx1 = NdotV / (NdotV * (1.0f - k) + k);
	float	ggx2 = NdotL / (NdotL * (1.0f - k) + k);
	return (ggx1 * ggx2);
}

static Vec3f	fresnel_schlick(float cosTheta, const Vec3f& F0)
{
	float t = std::pow(std::fmax(1.0f - cosTheta, 0.0f), 5.0f);
	return (F0 + (Vec3f(1.0f) - F0) * t);
}

bool	PBRMaterial::scatter(const Ray& r_in, const HitRecord& rec,
						   Vec3f& attenuation, Ray& scattered,
						   float& pdf, unsigned int* seed) const
{
	float	roughness = sampleRoughness(rec.u, rec.v);
	float	metallic = sampleMetallic(rec.u, rec.v);
	float	ao = sampleAO(rec.u, rec.v);
	Vec3f	base_color = hasTexture() ? sampleTextureFast(tex, rec.u * _texScale, rec.v * _texScale)
				: _color;
	Vec3f	N = rec.normal;

	if (hasNormal())
	{
		Vec3f	tangent_normal = sampleNormalTangentSpace(rec.u, rec.v);
		if (tangent_normal.length_sq() < 1e-6f)
			tangent_normal = Vec3f(0.0f, 0.0f, 1.0f);
		Vec3f N_perturbed = Vec3f::normalize(
			rec.tangent * tangent_normal._x +
			rec.bitangent * tangent_normal._y +
			rec.normal * tangent_normal._z
		);
		if (Vec3f::dot(N_perturbed, rec.normal) < 0.0f)
			N = rec.normal;
		else
			N = N_perturbed;
	}
	Vec3f	F0 = Vec3f(0.04f) * (1.0f - metallic) + base_color * metallic;
	Vec3f	V = Vec3f::normalize(-r_in._dir);
	float spec_prob = std::fmax(metallic, 0.04f);

	if (Vec3f::randomFloat(seed) < spec_prob)
	{
		float	a  = roughness * roughness;
		float	r1 = Vec3f::randomFloat(seed);
		float	r2 = Vec3f::randomFloat(seed);
		float	phi = 2.0f * (float)M_PI * r1;
		float	cosTheta = std::sqrt((1.0f - r2) / (1.0f + (a * a - 1.0f) * r2));
		float	sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
		Vec3f	up = (std::fabs(N._z) < 0.999f) ? Vec3f(0,0,1) : Vec3f(1,0,0);
		Vec3f	tangentX = Vec3f::normalize(Vec3f::cross(up, N));
		Vec3f	tangentY = Vec3f::cross(N, tangentX);
		Vec3f	H = tangentX * (sinTheta * std::cos(phi))
				+ tangentY * (sinTheta * std::sin(phi))
				+ N * cosTheta;
		H = Vec3f::normalize(H);
		Vec3f	L = Vec3f::reflect(-V, H);
		if (Vec3f::dot(N, L) <= 0.0f)
			return (false);
		scattered = Ray(rec.point, L);
		float	D = distribution_ggx(N, H, roughness);
		float	G = geometry_smith(N, V, L, roughness);
		Vec3f	F = fresnel_schlick(std::fmax(Vec3f::dot(H, V), 0.0f), F0);
		float	NdotL = std::fmax(Vec3f::dot(N, L), 0.0f);
		float	NdotV = std::fmax(Vec3f::dot(N, V), 1e-4f);
		Vec3f specular = (F * D * G) / (4.0f * NdotV * NdotL + 1e-6f);
		float NdotH = std::fmax(Vec3f::dot(N, H), 0.0f);
		float pdf_H = D * NdotH / (4.0f * std::fmax(Vec3f::dot(H, V), 1e-4f));
		pdf = pdf_H * spec_prob;
		if (pdf < 1e-6f)
			return (false);
		attenuation = specular * NdotL * ao / pdf;
		return (true);
	}
	else
	{
		Vec3f	scatterDir = N + Vec3f::randomUnitVector(seed);
		if (scatterDir.nearZero())
			scatterDir = N;
		scattered = Ray(rec.point, scatterDir);
		float	cosine = std::fmax(Vec3f::dot(N, Vec3f::normalize(scatterDir)), 0.0f);
		float	pdf_diffuse = cosine / (float)M_PI;
		pdf = pdf_diffuse * (1.0f - spec_prob);
		if (pdf < 1e-6f)
			return (false);
		Vec3f	kD = (Vec3f(1.0f) - F0) * (1.0f - metallic);
		attenuation = (kD * base_color * ao) / (1.0f - spec_prob);
		return (true);
	}
}

float	PBRMaterial::scattering_pdf(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	float	roughness = sampleRoughness(rec.u, rec.v);
	float	metallic = sampleMetallic(rec.u, rec.v);
	Vec3f	N = rec.normal;
	Vec3f	V = Vec3f::normalize(-r_in._dir);
	Vec3f	L = Vec3f::normalize(scattered._dir);
	float	NdotL = Vec3f::dot(N, L);

	if (NdotL <= 0.0f)
		return (0.0f);

	float	spec_prob = std::fmax(metallic, 0.04f);
	float	pdf_diffuse = NdotL / (float)M_PI;
	Vec3f	H = Vec3f::normalize(V + L);
	float	NdotH = std::fmax(Vec3f::dot(N, H), 0.0f);
	float	HdotV = std::fmax(Vec3f::dot(H, V), 1e-4f);
	float	D = distribution_ggx(N, H, roughness);
	float	pdf_specular = (D * NdotH) / (4.0f * HdotV);
	float	pdf_total = pdf_diffuse * (1.0f - spec_prob) + pdf_specular * spec_prob;

	return (std::fmax(pdf_total, 0.0f));
}


Vec3f	PBRMaterial::emitted(float u, float v, const Vec3f& p) const
{
	(void)p;

	if (!hasEmissive())
		return (Vec3f(0.0f));
	float	su = u * _texScale - std::floor(u * _texScale);
	float	sv = v * _texScale - std::floor(v * _texScale);
	int		x = std::min((int)(su * emissive->w), emissive->w - 1);
	int		y = std::min((int)((1.0f - sv) * emissive->h), emissive->h - 1);
	Uint8*	pixels = (Uint8*)emissive->pixels;
	int		bpp = SDL_BYTESPERPIXEL(emissive->format);
	Uint8*	pix = pixels + y * emissive->pitch + x * bpp;

	return (Vec3f(
		pix[0] / 255.0f,
		pix[1] / 255.0f,
		pix[2] / 255.0f
	) * _intensity);
}
