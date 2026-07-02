/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scene.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:41:12 by gajanvie          #+#    #+#             */
/*   Updated: 2026/07/02 17:23:15 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Hittable.hpp"
#include "BVHNode.hpp"
#include "Sphere.hpp"
#include "Quad.hpp"
#include "Triangle.hpp"
#include "Plane.hpp"
#include "Mesh.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include "BVHdebug.hpp"

class Scene : public Hittable
{
	public:
		void	add(std::shared_ptr<Hittable> object)
		{
			_objects.push_back(object);
		}
		
		void	add_light(std::shared_ptr<Hittable> object)
		{
			_objects.push_back(object);
			_light.push_back(object);
		}
		
		void	build_bvhs()
		{
			std::vector<std::shared_ptr<Hittable>>	flat_spheres;
			std::vector<std::shared_ptr<Hittable>>	flat_triangles;
			std::vector<std::shared_ptr<Hittable>>	flat_quads;
			
			_planes.clear();

			for (const auto& obj : _objects)
			{
				if (dynamic_cast<Sphere*>(obj.get()))
					flat_spheres.push_back(obj);
				else if (dynamic_cast<Quad*>(obj.get()))
					flat_quads.push_back(obj);
				else if (dynamic_cast<Triangle*>(obj.get()))
					flat_triangles.push_back(obj);
				else if (dynamic_cast<Plane*>(obj.get()))
					_planes.push_back(obj);
				else if (auto* mesh = dynamic_cast<Mesh*>(obj.get()))
				{
					for (const auto& tri : mesh->getTriangles())
						flat_triangles.push_back(tri);
				}
			}

			if (!flat_spheres.empty())
				_bvh_spheres = std::make_shared<BVHNode>(flat_spheres);
			if (!flat_triangles.empty())
				_bvh_triangles = std::make_shared<BVHNode>(flat_triangles);
			if (!flat_quads.empty())
				_bvh_quads = std::make_shared<BVHNode>(flat_quads);
		}

		bool	hit(const Ray& r, float t_min, float t_max, HitRecord& rec, int* node_tests = nullptr) const override
		{
			bool		hit_anything = false;
			float		closest = t_max;
			HitRecord	temp_rec;

			if (_bvh_triangles && _bvh_triangles->hit(r, t_min, closest, temp_rec, node_tests))
			{
				hit_anything = true;
				closest = temp_rec.t;
				rec = temp_rec;
			}
			if (_bvh_spheres && _bvh_spheres->hit(r, t_min, closest, temp_rec, node_tests))
			{
				hit_anything = true;
				closest = temp_rec.t;
				rec = temp_rec;
			}
			if (_bvh_quads && _bvh_quads->hit(r, t_min, closest, temp_rec, node_tests))
			{
				hit_anything = true;
				closest = temp_rec.t;
				rec = temp_rec;
			}
			for (const auto& plane : _planes)
			{
				if (plane->hit(r, t_min, closest, temp_rec, node_tests))
				{
					hit_anything = true;
					closest = temp_rec.t;
					rec = temp_rec;
				}
			}
			return (hit_anything);
		}

		bool	bbox(AABB& output_box) const override
		{
			AABB	temp;
			bool	first = true;

			if (_bvh_triangles && _bvh_triangles->bbox(temp))
			{
				output_box = temp;
				first = false;
			}
			if (_bvh_spheres && _bvh_spheres->bbox(temp))
			{
				output_box = first ? temp : AABB::AABB_union(output_box, temp);
				first = false;
			}
			if (_bvh_quads && _bvh_quads->bbox(temp))
			{
				output_box = first ? temp : AABB::AABB_union(output_box, temp);
				first = false;
			}
			return (!first);
		}

		std::vector<std::shared_ptr<Hittable>>	getLights() const { return _light; }
		unsigned int	getLightsCount() const {return _light.size(); }
		const std::vector<std::shared_ptr<Hittable>>&	getObjects() const { return _objects; }

		int	getMaxDepth() const override
		{
			return (0);
		}

		void	hit_box_depths(const Ray& r, const BvhDebugConfig& config,
					float t_geom, Vec3f& color_out, float& alpha_out, 
					int current_depth = 0) const
		{
			//accumulateur
			Vec3f	temp_color(0.0f);
			float	temp_alpha = 0.0f;

			if (_bvh_triangles && config.show_triangles)
			{
				_bvh_triangles->hit_box_depth(r, config.min_depth, config.max_depth, t_geom, temp_color, temp_alpha, current_depth);
				color_out += temp_color * (1.0f - alpha_out);
				alpha_out += temp_alpha * (1.0f - alpha_out);
			}
			if (_bvh_spheres && config.show_spheres)
			{
				temp_color = Vec3f(0.0f); temp_alpha = 0.0f;
				_bvh_spheres->hit_box_depth(r, config.min_depth, config.max_depth, t_geom, temp_color, temp_alpha, current_depth);
				color_out += temp_color * (1.0f - alpha_out);
				alpha_out += temp_alpha * (1.0f - alpha_out);
			}
			if (_bvh_quads && config.show_quads)
			{
				temp_color = Vec3f(0.0f); temp_alpha = 0.0f;
				_bvh_quads->hit_box_depth(r, config.min_depth, config.max_depth, t_geom, temp_color, temp_alpha, current_depth);
				color_out += temp_color * (1.0f - alpha_out);
				alpha_out += temp_alpha * (1.0f - alpha_out);
			}
		}

		const BVHNode*	get_bvh_triangles() const { return _bvh_triangles.get(); }
		const BVHNode*	get_bvh_spheres() const { return _bvh_spheres.get(); }
		const BVHNode*	get_bvh_quads() const { return _bvh_quads.get(); }

	private:
		std::vector<std::shared_ptr<Hittable>>	_objects;
		std::vector<std::shared_ptr<Hittable>>	_light;
		std::vector<std::shared_ptr<Hittable>>	_planes;

		std::shared_ptr<BVHNode>	_bvh_triangles;
		std::shared_ptr<BVHNode>	_bvh_spheres;
		std::shared_ptr<BVHNode>	_bvh_quads;
};
