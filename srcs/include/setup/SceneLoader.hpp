/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SceneLoader.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 16:41:16 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/19 11:31:57 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "Material.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Quad.hpp"
#include "Triangle.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "Lambertian.hpp"
#include "Dielectric.hpp"
#include "DiffuseLight.hpp"
#include "Metal.hpp"

struct	AppContext
{
	AppContext(){};
	~AppContext(){};
	Scene	scene;
	Camera	camera;

	std::unordered_map<std::string, std::shared_ptr<Material>>	materials;
	int		width = 1920;
	int		height = 1080;
	int		samples = -1;
	size_t	frame_count = 0;
};

// ordre mult matrice 	Translation * Rotation * Scale

class	SceneLoader
{
	private :
		static void	set_mesh(AppContext& app, std::istringstream &iss, std::string token);
		static void	set_res(AppContext& app, std::istringstream &iss, std::string token);
		static void	set_cam(AppContext& app, std::istringstream &iss, std::string token);
		static void	set_mat(AppContext& app, std::istringstream &iss, std::string token, std::string &current_mtl_name, std::shared_ptr<Material> &current_mtl);

	public:

		static bool	loadRT(const std::string& filepath, AppContext& app)
		{
			std::ifstream	file(filepath);

			if (!file.is_open())
			{
				std::cerr << "error ouverture -> " << filepath << "\n";
				return false;
			}

			std::string					line;
			std::string					current_mtl_name = "";
			std::shared_ptr<Material>	current_mtl = nullptr;

			while (std::getline(file, line))
			{
				if (line.empty())
					continue;

				std::istringstream	iss(line);
				std::string			token;

				iss >> token;

				if (token.empty() || token[0] == '#' || (token.size() >= 2 && token[0] == '/' && token[1] == '/'))
					continue ;

				set_res(app, iss, token);
				set_cam(app, iss, token);
				
				set_mat(app, iss, token, current_mtl_name, current_mtl);
				set_mesh(app, iss, token);

				if (token == "sphere" || token == "sp")
				{
					float		x = 0, y = 0, z = 0, radius = 0;
					float		nx = 0, ny = 1, nz = 0;
					std::string	mtl_name;

					if (iss >> x >> y >> z >> radius >> nx >> ny >> nz >> mtl_name)
					{
						if (app.materials.find(mtl_name) != app.materials.end())
						{
							Vec3f		center(x, y, z);
							Vec3f		normal(nx, ny, nz);
							Vec3f		rot = normal.to_rotation_angles();
							Mat4f		translation = Mat4f::translate(center);
							Mat4f		rotX = Mat4f::rotateX(rot._x);
							Mat4f		rotY = Mat4f::rotateY(rot._y);
							Mat4f		scale = Mat4f::scale(Vec3f(radius, radius, radius));
							Mat4f		transform = translation * rotY * rotX * scale;
							Material*	mat_ptr = app.materials[mtl_name].get();

							auto	new_sphere = std::make_shared<Sphere>(radius, center, normal, transform, mat_ptr);
							if (dynamic_cast<DiffuseLight*>(mat_ptr) != nullptr)
								app.scene.add_light(new_sphere);
							else
								app.scene.add(new_sphere);
						}
						else
							std::cerr << "Erreur: Materiau " << mtl_name << " not known.\n";
					}
					else
					{
						std::cerr << "error syntax sphere (x y z radius nx ny nz mat_name)" << std::endl;
					}
				}
				else if (token == "plane" || token == "pl")
				{
					float		x = 0, y = 0, z = 0;
					float		nx = 0, ny = 1, nz = 0;
					std::string	mtl_name;

					if (iss >> x >> y >> z >> nx >> ny >> nz >> mtl_name)
					{
						if (app.materials.find(mtl_name) != app.materials.end())
						{
							Vec3f	point(x, y, z);
							Vec3f	normal(nx, ny, nz);
							normal = Vec3f::normalize(normal);

							Vec3f	up(0, 1, 0);
							Vec3f	axis = Vec3f::cross(up, normal);
							float	sinA = axis.length();
							float	cosA = Vec3f::dot(up, normal);
							Mat4f	rotation;

							if (sinA < 1e-6f)
							{
								if (cosA < 0.0f)
									rotation = Mat4f::rotateX(M_PI);
							}
							else
							{
								axis = Vec3f::normalize(axis);
								rotation = Mat4f::rotate((float)std::atan2(sinA, cosA), axis);
							}
							Mat4f translation = Mat4f::translate(point);
							Mat4f transform = translation * rotation;
							Material* mat_ptr = app.materials[mtl_name].get();
							auto new_pl = std::make_shared<Plane>(point, normal, transform, mat_ptr);
							if (dynamic_cast<DiffuseLight*>(mat_ptr) != nullptr)
								app.scene.add_light(new_pl);
							else
								app.scene.add(new_pl);
						}
						else
							std::cerr << "error: Materiau " << mtl_name << " not known\n";
					}
					else
					{
						std::cerr << "error syntax plane (x y z nx ny nz mat_name)" << std::endl;
					}
				}

				else if (token == "triangle" || token == "tr")
				{
					float		x1 = 0, y1 = 0, z1 = 0;
					float		x2 = 0, y2 = 0, z2 = 0;
					float		x3 = 0, y3 = 0, z3 = 0;
					std::string	mtl_name;

					if (iss >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3 >> mtl_name)
					{
						if (app.materials.find(mtl_name) != app.materials.end())
						{
							Vec3f	v0(x1, y1, z1);
							Vec3f	v1(x2, y2, z2);
							Vec3f	v2(x3, y3, z3);
							Material*	mat_ptr = app.materials[mtl_name].get();
							auto		new_tr = std::make_shared<Triangle>(v0, v1, v2, mat_ptr);
							if (dynamic_cast<DiffuseLight*>(mat_ptr) != nullptr)
								app.scene.add_light(new_tr);
							else
								app.scene.add(new_tr);
						}
						else
							std::cerr << "Erreur: Materiau " << mtl_name << " not known.\n";
					}
					else
					{
						std::cerr << "error syntax triangle (x1 y1 z1 x2 y2 z2 x3 y3 z3 mat_name)" << std::endl;
					}
				}
				else if (token == "quad" || token == "qu")
				{
					float		x = 0, y = 0, z = 0;
					float		nx = 0, ny = 1, nz = 0;
					float		w, h;
					std::string	mtl_name;

					if (iss >> x >> y >> z >> nx >> ny >> nz >> w >> h >> mtl_name)
					{
						if (app.materials.find(mtl_name) != app.materials.end())
						{
							Vec3f	center(x, y, z);
							Vec3f	normal(nx, ny, nz);
							normal = Vec3f::normalize(normal);

							Vec3f	up(0, 1, 0);
							Vec3f	axis = Vec3f::cross(up, normal);
							float	sinA = axis.length();
							float	cosA = Vec3f::dot(up, normal);
							Mat4f	rotation;

							if (sinA < 1e-6f)
							{
								if (cosA < 0.0f)
									rotation = Mat4f::rotateX(M_PI);
							}
							else
							{
								axis = Vec3f::normalize(axis);
								rotation = Mat4f::rotate((float)std::atan2(sinA, cosA), axis);
							}
							Mat4f	mat_scale;
							Vec3f	scale(w / 2.0f, 1.0, h / 2.0f);
							mat_scale = mat_scale.scale(scale);
							Mat4f	trans;
							trans = trans.translate(center);
							Mat4f final = trans * rotation * mat_scale;
							Material*	mat_ptr = app.materials[mtl_name].get();
							auto		new_tr = std::make_shared<Quad>(center, normal, final, mat_ptr, w, h);
							if (dynamic_cast<DiffuseLight*>(mat_ptr) != nullptr)
								app.scene.add_light(new_tr);
							else
								app.scene.add(new_tr);
						}
						else
							std::cerr << "Erreur: Materiau " << mtl_name << " not known.\n";
					}
					else
					{
						std::cerr << "error syntax triangle (x y z nx ny nz mat_name width height)" << std::endl;
					}
				}
			}
			file.close();
			return true;
		}
};
