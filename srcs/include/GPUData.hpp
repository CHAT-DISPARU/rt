/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GPUData.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:53:13 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/25 21:00:22 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cstdint>
#include "Math3D.hpp" 

enum	GPUMaterialType
{
	MAT_LAMBERTIAN = 0,
	MAT_METAL = 1,
	MAT_DIELECTRIC = 2,
	MAT_PBR = 3,
	MAT_EMISSIVE = 4
};

// 64 bit 4 bloc de 16 ok
struct alignas(16)	GPUMaterial
{
	Vec3f	color;// 12 bytes
	float	roughness;// 4 bytes  premier bloc

	Vec3f	emission;// 12 bytes
	float	metallic;// 4 bytes  deuxieme bloc

	float	ior;// 4 bytes
	int	type;// 4 bytes
	int	albedo_tex_idx;// 4 bytes => -1 soi pas de texture
	int	normal_tex_idx;// 4 bytes troisieme bloc

	int	roughness_tex_idx;// 4 bytes
	int	metallic_tex_idx;// 4 bytes
	int	emission_tex_idx;// 4 bytes
	int	pad0;// 4 bytes quatrieme bloc
};

// 64  4 x 16 ok
struct alignas(16)	GPUTriangle
{
	Vec3f	v0;// 12 bytes
	int	mat_idx;// 4 bytes bloc 1

	Vec3f	v1;// 12 bytes
	float	uv0_x;// 4 bytes bloc 2

	Vec3f	v2;// 12 bytes
	float	uv0_y;// 4 bytes bloc 3

	float	uv1_x;// 4 bytes
	float	uv1_y;// 4 bytes
	float	uv2_x;// 4 bytes
	float	uv2_y;// 4 bytes bloc 4
};

// 96  6x16 ok
struct alignas(16)	GPUSphere
{
	Mat4f	inverse_transform;// 64 bytes 4 bloc de 16
	
	Vec3f	center;// 12 bytes
	float	radius;// 4 bytes bloc 5
	
	int		mat_idx;// 4 bytes
	float	pad[3];// 12 bytes bloc 6 (bite de padding)
};

// 48  3x16 ok
struct alignas(16)	GPUBVHNode
{
	Vec3f	aabb_min;// 12 bytes
	int		left_child_or_prim_offset;// 4 bytes bloc 1

	Vec3f	aabb_max;// 12 bytes
	int		right_child;// 4 bytes bloc 2

	int	primitive_count;// 4 bytes (>0 si c'est une feuille)
	int	axis;// 4 bytes
	int	pad[2];// 8 bytes bloc 3 (16 bytes padding)
};

// 96  6x16 ok 
struct alignas(16)	GPUPlane
{
	Mat4f	inverse_transform;// 64 bytes 4 bloc de 16
	
	Vec3f	point;// 12 bytes
	int		mat_idx;// 4 bytes bloc 5
	
	Vec3f	normal;// 12 bytes
	int		pad0;// 4 bytes bloc 6
};

// 112  7x16 ok
struct alignas(16)	GPUQuad
{
	Mat4f	inverse_transform;// 64 bytes 4 bloc de 16
	
	Vec3f	center;// 12 bytes
	int		mat_idx;// 4 bytes bloc 5
	
	Vec3f	normal;// 12 bytes
	float	w;// 4 bytes bloc 6
	
	float	h;// 4 bytes
	float	pad[3];// 12 bytes bloc 7 (bite de padding)
};

struct alignas(16)	GPUPushConstants
{
	Vec3f		cam_origin;
	float		pad0;
	Vec3f		cam_forward;
	float		pad1;
	Vec3f		cam_right;
	float		pad2;
	Vec3f		cam_up;
	float		fov;
	int			frame_count;
	int			max_depth;
	uint32_t	seed;
	int			pad3;
};
