/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScenePacker.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 17:23:28 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/07/09 13:10:43 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ScenePacker.hpp"
#include "Lambertian.hpp"
#include "Metal.hpp"
#include "Dielectric.hpp"
#include "PBRMaterial.hpp"
#include "DiffuseLight.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Triangle.hpp"
#include "Quad.hpp"
#include "Mesh.hpp"

void	ScenePacker::pack_scene_to_gpu(const AppContext& app)
{
	pack_materials(app.materials);
	pack_geometry(app.scene);
	const BVHNode*	bvh_tri_ptr = app.scene.get_bvh_triangles();
	const BVHNode*	bvh_sph_ptr = app.scene.get_bvh_spheres();
	const BVHNode*	bvh_qd_ptr = app.scene.get_bvh_quads();

	if (bvh_tri_ptr != nullptr)
		pack_bvh(*bvh_tri_ptr, gpu_bvh_triangles);
	if (bvh_sph_ptr != nullptr)
		pack_bvh(*bvh_sph_ptr, gpu_bvh_spheres);
	if (bvh_qd_ptr != nullptr)
		pack_bvh(*bvh_qd_ptr, gpu_bvh_quads);
}

void	ScenePacker::pack_materials(const std::unordered_map<std::string, std::shared_ptr<Material>>& materials)
{
	for (const auto& [name, mat_ptr] : materials)
	{
		Material*	raw_mat = mat_ptr.get();

		// deja process
		if (mat_to_idx.find(raw_mat) != mat_to_idx.end())
			continue;

		GPUMaterial	gpu_mat = {};
		
		gpu_mat.color = raw_mat->getColor();
		gpu_mat.emission = raw_mat->sampleEmissive(0.0f, 0.0f);
		gpu_mat.ior = raw_mat->ior();
		
		//pas encore gerre juste 0 ou -1
		gpu_mat.albedo_tex_idx = raw_mat->hasTexture() ? register_texture(raw_mat->getTexture()) : -1;
		gpu_mat.normal_tex_idx = raw_mat->hasNormal() ? register_texture(raw_mat->getNormal()) : -1;
		gpu_mat.roughness_tex_idx = raw_mat->hasRoughness() ? register_texture(raw_mat->getRoughness()) : -1;
		gpu_mat.metallic_tex_idx = raw_mat->hasMetallic() ? register_texture(raw_mat->getMetallic()) : -1;
		gpu_mat.emission_tex_idx = raw_mat->hasEmissive() ? register_texture(raw_mat->getEmissive()) : -1;
		gpu_mat.is_opaq = raw_mat->isOpaq() ? 1 : 0;
		gpu_mat.is_spec = raw_mat->isSpecular() ? 1 : 0;
		if (dynamic_cast<Lambertian*>(raw_mat))
		{
			gpu_mat.type = MAT_LAMBERTIAN;
			gpu_mat.roughness = 1.0f;
			gpu_mat.metallic = 0.0f;
		}
		else if (auto* metal = dynamic_cast<Metal*>(raw_mat))
		{
			gpu_mat.type = MAT_METAL;
			gpu_mat.roughness = metal->getFuzz();
			gpu_mat.metallic = 1.0f;
		}
		else if (auto* glass = dynamic_cast<Dielectric*>(raw_mat))
		{
			gpu_mat.type = MAT_DIELECTRIC;
			gpu_mat.roughness = glass->getFuzz();
			gpu_mat.metallic = 0.0f;
		}
		else if (auto* pbr = dynamic_cast<PBRMaterial*>(raw_mat))
		{
			gpu_mat.type = MAT_PBR;
			gpu_mat.roughness = pbr->getRoughnessScalar();
			gpu_mat.metallic = pbr->getMetallicScalar();
		}
		else if (dynamic_cast<DiffuseLight*>(raw_mat))
		{
			gpu_mat.type = MAT_EMISSIVE;
			gpu_mat.emission = raw_mat->getColor() * raw_mat->getIntensity();
		}
		mat_to_idx[raw_mat] = gpu_materials.size();
		gpu_materials.push_back(gpu_mat);
	}
}

void	ScenePacker::pack_geometry(const Scene& scene)
{
	const int	PRIM_TRIANGLE = 0;
	const int	PRIM_SPHERE = 1;
	const int	PRIM_QUAD = 2;

	if (const BVHNode* bvh_sph = scene.get_bvh_spheres())
	{
		for (const auto& obj_ptr : bvh_sph->getOrderedObjects())
		{
			if (auto* sphere = dynamic_cast<Sphere*>(obj_ptr.get()))
			{
				Material*	mat = sphere->getMat();
				int			current_mat_idx = (mat && mat_to_idx.count(mat)) ? mat_to_idx[mat] : 0;
				
				GPUSphere	gpu_sph{};
				gpu_sph.inverse_transform = sphere->getInverseTransform();
				gpu_sph.center = sphere->getCenter();
				gpu_sph.radius = sphere->getRadius();
				gpu_sph.mat_idx = current_mat_idx;
				
				int	current_idx = gpu_spheres.size();
				gpu_spheres.push_back(gpu_sph);
				
				if (gpu_materials[current_mat_idx].type == MAT_EMISSIVE)
				{
					GPULight light{};
					light.prim_type = PRIM_SPHERE;
					light.prim_idx = current_idx;
					gpu_lights.push_back(light);
				}
			}
		}
	}
	if (const BVHNode* bvh_qd = scene.get_bvh_quads())
	{
		for (const auto& obj_ptr : bvh_qd->getOrderedObjects())
		{
			if (auto* quad = dynamic_cast<Quad*>(obj_ptr.get()))
			{
				Material*	mat = quad->getMat();
				int			current_mat_idx = (mat && mat_to_idx.count(mat)) ? mat_to_idx[mat] : 0;

				GPUQuad		gpu_qd{};
				gpu_qd.inverse_transform = quad->getInverseTransform();
				gpu_qd.center = quad->getCenter();
				gpu_qd.mat_idx = current_mat_idx;
				gpu_qd.normal = quad->getNormal();
				gpu_qd.w = quad->getW();
				gpu_qd.h = quad->getH();
				
				int current_idx = gpu_quads.size();
				gpu_quads.push_back(gpu_qd);
				
				if (gpu_materials[current_mat_idx].type == MAT_EMISSIVE)
				{
					GPULight light{};
					light.prim_type = PRIM_QUAD;
					light.prim_idx = current_idx;
					gpu_lights.push_back(light);
				}
			}
		}
	}
	if (const BVHNode* bvh_tri = scene.get_bvh_triangles())
	{
		for (const auto& obj_ptr : bvh_tri->getOrderedObjects())
		{
			if (auto* tri = dynamic_cast<Triangle*>(obj_ptr.get()))
			{
				Material*	mat = tri->getMat();
				int			current_mat_idx = (mat && mat_to_idx.count(mat)) ? mat_to_idx[mat] : 0;

				GPUTriangle	gpu_tri{};
				gpu_tri.v0 = tri->getV0();
				gpu_tri.mat_idx = current_mat_idx;
				gpu_tri.v1 = tri->getV1();
				gpu_tri.uv0_x = tri->getUV0()[0];
				gpu_tri.v2 = tri->getV2();
				gpu_tri.uv0_y = tri->getUV0()[1];
				gpu_tri.uv1_x = tri->getUV1()[0];
				gpu_tri.uv1_y = tri->getUV1()[1];
				gpu_tri.uv2_x = tri->getUV2()[0];
				gpu_tri.uv2_y = tri->getUV2()[1];
				
				int	current_idx = gpu_triangles.size();
				gpu_triangles.push_back(gpu_tri);
				
				if (gpu_materials[current_mat_idx].type == MAT_EMISSIVE)
				{
					GPULight light{};
					light.prim_type = PRIM_TRIANGLE;
					light.prim_idx = current_idx;
					gpu_lights.push_back(light);
				}
			}
		}
	}
	for (const auto& obj_ptr : scene.getObjects())
	{
		if (auto* plane = dynamic_cast<Plane*>(obj_ptr.get()))
		{
			Material*	mat = plane->getMat();
			int			current_mat_idx = (mat && mat_to_idx.count(mat)) ? mat_to_idx[mat] : 0;

			GPUPlane	gpu_pl{};
			gpu_pl.inverse_transform = plane->getInverseTransform();
			gpu_pl.point = plane->getPoint();
			gpu_pl.mat_idx = current_mat_idx;
			gpu_pl.normal = plane->getNormal();
			gpu_planes.push_back(gpu_pl);
		}
	}
}

int	ScenePacker::register_texture(SDL_Surface* tex)
{
	if (!tex)
		return (-1);
	return (0);
}

void	ScenePacker::pack_bvh(const BVHNode& root_bvh, std::vector<GPUBVHNode>& target_buffer)
{
	const auto&	cpu_nodes = root_bvh.getNodes();
	
	target_buffer.reserve(cpu_nodes.size());

	for (const auto& cpu_node : cpu_nodes)
	{
		GPUBVHNode	gpu_node{};
		
		gpu_node.aabb_min = cpu_node.bbox._min;
		gpu_node.left_child_or_prim_offset = cpu_node.leftChildOrPrimOffset;
		gpu_node.aabb_max = cpu_node.bbox._max;
		gpu_node.right_child = cpu_node.rightChildOffset;
		gpu_node.primitive_count = cpu_node.primitiveCount;
		gpu_node.axis = cpu_node.axis;
		target_buffer.push_back(gpu_node);
	}
}
