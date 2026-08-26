/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GPUData.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:53:13 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/08/26 15:27:55 by gajanvie         ###   ########.fr       */
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
	Vec3f	color;// 12 
	float	roughness;// 4   premier bloc

	Vec3f	emission;// 12 
	float	metallic;// 4   deuxieme bloc

	float	ior;// 4 
	int	type;// 4 
	int	albedo_tex_idx;// 4  => -1 soit pas de texture
	int	normal_tex_idx;// 4  troisieme bloc

	int	roughness_tex_idx;// 4 
	int	metallic_tex_idx;// 4 
	int	emission_tex_idx;// 4 
	int is_opaq;// 4    bloc 4

    int	is_spec;// 4
    int	pad0[3];// bloc 5
};

// 64  4 x 16 ok
struct alignas(16)	GPUTriangle
{
	Vec3f	v0;// 12 
	int	mat_idx;// 4  bloc 1

	Vec3f	v1;// 12 
	float	uv0_x;// 4  bloc 2

	Vec3f	v2;// 12 
	float	uv0_y;// 4  bloc 3

	float	uv1_x;// 4 
	float	uv1_y;// 4 
	float	uv2_x;// 4 
	float	uv2_y;// 4  bloc 4
};

// 96  6x16 ok
struct alignas(16)	GPUSphere
{
	Mat4f	inverse_transform;// 64  4 bloc de 16
	
	Vec3f	center;// 12 
	float	radius;// 4  bloc 5
	
	int		mat_idx;// 4 
	float	pad[3];// 12  bloc 6 (bite de padding)
};

// 48  3x16 ok
struct alignas(16)	GPUBVHNode
{
	Vec3f	aabb_min;// 12 
	int		left_child_or_prim_offset;// 4  bloc 1

	Vec3f	aabb_max;// 12 
	int		right_child;// 4  bloc 2

	int	primitive_count;// 4  (>0 si c'est une feuille)
	int	axis;// 4 
	int	pad[2];// 8  bloc 3 (16  padding)
};

// 96  6x16 ok 
struct alignas(16)	GPUPlane
{
	Mat4f	inverse_transform;// 64  4 bloc de 16
	
	Vec3f	point;// 12 
	int		mat_idx;// 4  bloc 5
	
	Vec3f	normal;// 12 
	int		pad0;// 4  bloc 6
};

// 112  7x16 ok
struct alignas(16)	GPUQuad
{
	Mat4f	inverse_transform;// 64  4 bloc de 16
	
	Vec3f	center;// 12 
	int		mat_idx;// 4  bloc 5
	
	Vec3f	normal;// 12 
	float	w;// 4  bloc 6
	
	float	h;// 4 
	float	pad[3];// 12  bloc 7 (bite de padding)
};


//  7 x16
struct alignas(16) GPUPushConstants
{
	Vec3f	cam_origin;//12 
	float	m_lens_radius;//4  bloc 1
	Vec3f	cam_forward;// 12 
	float	m_focus_dist;//4   bloc 2
	Vec3f	cam_right;//12 
	float	pad2;//4  bloc 3
	Vec3f	cam_up;//12 
	float	fov;//4  bloc 4
	int			frame_count;//4 
	int			max_depth;//4 
	uint32_t	seed;//4 
	float		time;//4  bloc 5
	int			w_h;// 4
	int			w_w;//4
	uint32_t	light_count;//4
	int			shadow_ray;//4 bloc 6
	int			ru_enabled;//4
	int			pad[3];//12 bloc7
	
};


//16 ok
struct alignas(16) GPULight
{
	int	prim_type;//4
	int	prim_idx;//4
	int	pad[2];//8 bloc 1
};