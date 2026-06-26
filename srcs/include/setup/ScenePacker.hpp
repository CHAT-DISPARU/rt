/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScenePacker.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 17:22:23 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/25 20:59:34 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GPUData.hpp"
#include "Scene.hpp"
#include "Material.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include "rt.hpp"

class ScenePacker
{
public:
	ScenePacker() = default;
	~ScenePacker() = default;

	std::vector<GPUMaterial>	gpu_materials;
	std::vector<GPUTriangle>	gpu_triangles;
	std::vector<GPUSphere>		gpu_spheres;
	std::vector<GPUPlane>		gpu_planes;
	std::vector<GPUQuad>		gpu_quads;
	std::vector<GPUBVHNode>		gpu_bvh_triangles;
	std::vector<GPUBVHNode>		gpu_bvh_spheres;
	std::vector<GPUBVHNode>		gpu_bvh_quads;
	void	pack_scene_to_gpu(const AppContext& app);

private:
	// dictionner lie pointeur a structure applati
	std::unordered_map<Material*, int>	mat_to_idx;
	void	pack_materials(const std::unordered_map<std::string, std::shared_ptr<Material>>& materials);
	void	pack_geometry(const Scene& scene);
	void	pack_bvh(const BVHNode& root_bvh, std::vector<GPUBVHNode>& target_buffer);
	int		register_texture(SDL_Surface* tex);
};