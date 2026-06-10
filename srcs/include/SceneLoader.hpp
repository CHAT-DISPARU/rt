/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SceneLoader.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 16:41:16 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/10 17:56:16 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "Material.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
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

				// de base
				if (token == "resolution" || token == "res")
				{
					if (!(iss >> app.width >> app.height))
						std::cerr << "Erreur syntaxe resolution " << std::endl;
				}
				else if (token == "samples" || token == "s")
				{
					std::string	s_val;

					if (iss >> s_val)
					{
						if (s_val == "inf")
							app.samples = -1;
						else
						{
							try
							{
								app.samples = std::stoi(s_val);
							}
							catch (...)
							{
								std::cerr << "error sample mode auto -> inf" << std::endl;
								app.samples = -1;
							}
						}
					}
				}
				
				// cam
				else if (token == "camera")
				{
					float	px = 0, py = 0, pz = 0;
					float	dx = 0, dy = 0, dz = -1;
					float	fov = 90.0f;

					if (iss >> px >> py >> pz >> dx >> dy >> dz >> fov)
					{
						Vec3f	lookFrom(px, py, pz);
						Vec3f	lookAt = lookFrom + Vec3f(dx, dy, dz);
						app.camera = Camera(lookFrom, lookAt, Vec3f(0, 1, 0), fov, (float)app.width / (float)app.height);
					}
					else
					{
						std::cerr << "Erreur syntaxe camera" << std::endl;
					}
				}
				
				// mat
				else if (token == "newmat" || token == "mat")
				{
					if (iss >> current_mtl_name)
					{
						current_mtl = std::make_shared<Lambertian>(); 
						app.materials[current_mtl_name] = current_mtl;
					}
					else
					{
						std::cerr << "error: 'newmat' sans name" << std::endl;
					}
				}
				else if (token == "type")
				{
					if (current_mtl == nullptr)
					{
						std::cerr << "error type not in mat" << std::endl;
						continue;
					}
					std::string	type;
					if (iss >> type)
					{
						if (type == "metal")
						{
							app.materials[current_mtl_name] = std::make_shared<Metal>();
							current_mtl = app.materials[current_mtl_name];
						}
						else if (type == "emissive")
						{
							app.materials[current_mtl_name] = std::make_shared<DiffuseLight>(); 
							current_mtl = app.materials[current_mtl_name];
						}
						else if (type == "dielectric" || type == "glass")
						{
							app.materials[current_mtl_name] = std::make_shared<Dielectric>(); 
							current_mtl = app.materials[current_mtl_name];
						}
					}
				}
				else if (token == "albedo" || token == "color")
				{
					if (current_mtl == nullptr)
					{
						std::cerr << "error albedo or color not in mat" << std::endl;
						continue ;
					}
					float	r = 0.0f, g = 0.0f, b = 0.0f;

					if (iss >> r >> g >> b)
						current_mtl->setColor(Vec3f(r, g, b));
					else
						std::cerr << "Erreur syntax color or albedo" << std::endl;
				}
				else if (token == "fuzz")
				{
					if (current_mtl == nullptr)
					{
						std::cerr << "error fuzz not in mat" << std::endl;
						continue ;
					}
					
					float	fuzz_val = 0.0f;
					if (iss >> fuzz_val)
					{
						if (auto metal_mat = std::dynamic_pointer_cast<Metal>(current_mtl))
							metal_mat->setFuzz(fuzz_val);
						else if (auto glass_mat = std::dynamic_pointer_cast<Dielectric>(current_mtl))
							glass_mat->setFuzz(fuzz_val);
						else
							std::cerr << "fuzz ignored only metal or dielectric" << std::endl;
					}
					else
						std::cerr << "Error syntax fuzz" << std::endl;
				}
				else if (token == "ni" || token == "ior")
				{
					if (current_mtl == nullptr)
					{
						std::cerr << "error ni not in mat" << std::endl;
						continue ;
					}
					
					float	ni_val = 0.0f;
					if (iss >> ni_val)
					{
						if (auto glass_mat = std::dynamic_pointer_cast<Dielectric>(current_mtl))
							glass_mat->setNi(ni_val);
						else
							std::cerr << "ni ignored only dielectric" << std::endl;
					}
					else
						std::cerr << "Error syntax fuzz" << std::endl;
				}

				//geo
				else if (token == "sphere" || token == "sp")
				{
					float		x = 0, y = 0, z = 0, radius = 0;
					float		nx = 0, ny = 1, nz = 0;
					std::string	mtl_name;

					if (iss >> x >> y >> z >> radius >> nx >> ny >> nz >> mtl_name)
					{
						if (app.materials.find(mtl_name) != app.materials.end())
						{
							Vec3f	center(x, y, z);
							Vec3f	normal(nx, ny, nz);
							Vec3f	rot = normal.to_rotation_angles();
							Mat4f	translation = Mat4f::translate(center);
							Mat4f	rotX = Mat4f::rotateX(rot._x);
							Mat4f	rotY = Mat4f::rotateY(rot._y);
							Mat4f	scale = Mat4f::scale(Vec3f(radius, radius, radius));
							Mat4f	transform = translation * rotY * rotX * scale;

							app.scene.add(std::make_shared<Sphere>(radius, center, normal, transform, app.materials[mtl_name].get()));
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
							Vec3f	rot = normal.to_rotation_angles();
							Mat4f	translation = Mat4f::translate(point);
							Mat4f	rotX = Mat4f::rotateX(rot._x);
							Mat4f	rotY = Mat4f::rotateY(rot._y);
							Mat4f	transform = translation * rotY * rotX;

							app.scene.add(std::make_shared<Plane>(point, normal, transform, app.materials[mtl_name].get()));
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

							app.scene.add(std::make_shared<Triangle>(v0, v1, v2, app.materials[mtl_name].get()));
						}
						else
							std::cerr << "Erreur: Materiau " << mtl_name << " not known.\n";
					}
					else
					{
						std::cerr << "error syntax triangle (x1 y1 z1 x2 y2 z2 x3 y3 z3 mat_name)" << std::endl;
					}
				}
			}

			file.close();
			return true;
		}
};
