/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SceneLoader.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 14:48:47 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/19 13:13:43 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <SDL3_image/SDL_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "SceneLoader.hpp"

void	SceneLoader::set_res(AppContext& app, std::istringstream &iss, std::string token)
{
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
}

void	SceneLoader::set_cam(AppContext& app, std::istringstream &iss, std::string token)
{
	if (token == "camera")
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
}

void	SceneLoader::set_mat(AppContext& app, std::istringstream &iss, std::string token, std::string &current_mtl_name, std::shared_ptr<Material> &current_mtl)
{
	if (token == "newmat" || token == "mat")
	{
		if (iss >> current_mtl_name)
		{
			current_mtl = std::make_shared<Lambertian>(); 
			app.materials[current_mtl_name] = current_mtl;
		}
		else
			std::cerr << "error: 'newmat' sans name" << std::endl;
	}
	else if (token == "type")
	{
		if (current_mtl == nullptr)
		{
			std::cerr << "error type not in mat" << std::endl;
			return ;
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
			return ;
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
			return ;
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
			return ;
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
	else if (token == "intensity" || token == "power")
	{
		if (current_mtl == nullptr)
		{
			std::cerr << "error intensity not in mat" << std::endl;
			return ;
		}
		float	intensity_val = 1.0f;
		if (iss >> intensity_val)
			current_mtl->setIntensity(intensity_val);
		else
			std::cerr << "Error syntax intensity" << std::endl;
	}
	else if (token == "tex" || token == "texture")
	{
		if (current_mtl == nullptr)
		{
			std::cerr << "error tex not in mat" << std::endl;
			return ;
		}
		std::string	name;
		if (iss >> name)
		{
			SDL_Surface	*raw = IMG_Load(name.c_str());
			if (!raw)
			{
				std::cerr << "Error load tex" << std::endl;
				return ;
			}
			SDL_Surface	*converted = SDL_ConvertSurface(raw, SDL_PIXELFORMAT_RGBA32);
			SDL_DestroySurface(raw);

			if (!converted)
			{
				std::cerr << "Error convert tex: " << SDL_GetError() << std::endl;
				return ;
			}
			current_mtl->setTexture(converted);
		}
		else
			std::cerr << "Error syntax tex" << std::endl;
	}
}

void	SceneLoader::set_mesh(AppContext& app, std::istringstream &iss, std::string token)
{
	if (token != "mesh")
		return ;

	std::string	path;
	if (!(iss >> path))
	{
		std::cerr << "error syntax mesh (path.obj)" << std::endl;
		return ;
	}
	float		scale = 1.0f;
	Vec3f		pos(0.0f, 0.0f, 0.0f);
	Vec3f		normal(0.0f, 1.0f, 0.0f);
	float		roll = 0.0f;
	std::string	override_mat_name = "";

	if (iss >> scale)
	{
		float px, py, pz;
		if (iss >> px >> py >> pz)
		{
			pos = Vec3f(px, py, pz);
			
			std::string	next_token;
			if (iss >> next_token)
			{
				if (std::isdigit(next_token[0]) || next_token[0] == '-' || next_token[0] == '+')
				{
					float	nx = std::stof(next_token);
					float	ny, nz;
					if (iss >> ny >> nz)
					{
						normal = Vec3f(nx, ny, nz);
						float	length = std::sqrt(nx*nx + ny*ny + nz*nz);
						if (length > 0.0001f)
							normal = Vec3f(nx/length, ny/length, nz/length);
						std::string roll_token;
						if (iss >> roll_token)
						{
							if (std::isdigit(roll_token[0]) || roll_token[0] == '-' || roll_token[0] == '+')
							{
								roll = std::stof(roll_token);
								iss >> override_mat_name;
							}
							else
								override_mat_name = roll_token;
						}
					}
				}
				else
					override_mat_name = next_token;
			}
		}
	}
	Vec3f	rot = normal.to_rotation_angles();
	float	radX = rot._x;
	float	radY = rot._y;
	float	radZ = roll * M_PI / 180.0f;
	float	cx = std::cos(radX), sx = std::sin(radX);
	float	cy = std::cos(radY), sy = std::sin(radY);
	float	cz = std::cos(radZ), sz = std::sin(radZ);
	tinyobj::ObjReaderConfig	config;
	config.mtl_search_path = path.substr(0, path.find_last_of('/') + 1);

	tinyobj::ObjReader	reader;
	if (!reader.ParseFromFile(path, config))
	{
		std::cerr << "tinyobj error: " << reader.Error() << std::endl;
		return ;
	}
	if (!reader.Warning().empty())
		std::cerr << "tinyobj warning: " << reader.Warning() << std::endl;

	const auto&	attrib = reader.GetAttrib();
	const auto&	shapes = reader.GetShapes();
	const auto&	obj_materials = reader.GetMaterials();

	std::vector<std::shared_ptr<Material>>	converted_mats;
	for (const auto& m : obj_materials)
	{
		std::shared_ptr<Material>	mat;
		bool	is_emissive = (m.emission[0] + m.emission[1] + m.emission[2]) > 0.0f;

		if (is_emissive)
		{
			auto	light = std::make_shared<DiffuseLight>();
			light->setColor(Vec3f(m.emission[0], m.emission[1], m.emission[2]));
			mat = light;
		}
		else if (m.dissolve < 1.0f)
		{
			auto	glass = std::make_shared<Dielectric>();
			glass->setNi(m.ior > 0.0f ? m.ior : 1.5f);
			mat = glass;
		}
		else if (m.shininess > 200.0f) 
		{
			auto	metal = std::make_shared<Metal>();
			metal->setFuzz(1.0f - std::fmin(1.0f, m.shininess / 1000.0f));
			mat = metal;
		}
		else
			mat = std::make_shared<Lambertian>();

		mat->setColor(Vec3f(m.diffuse[0], m.diffuse[1], m.diffuse[2]));
		if (!m.diffuse_texname.empty())
		{
			std::string	tex_path = config.mtl_search_path + m.diffuse_texname;
			SDL_Surface	*raw = IMG_Load(tex_path.c_str());
			if (raw)
			{
				SDL_Surface	*converted = SDL_ConvertSurface(raw, SDL_PIXELFORMAT_RGBA32);
				SDL_DestroySurface(raw);
				if (converted)
					mat->setTexture(converted);
			}
			else
				std::cerr << "tinyobj: texture introuvable " << tex_path << std::endl;
		}

		converted_mats.push_back(mat);
		app.materials[std::string("mesh_mat_") + std::to_string(converted_mats.size() - 1)] = mat;
	}
	std::shared_ptr<Material>	default_mat = std::make_shared<Lambertian>();
	default_mat->setColor(Vec3f(0.7f, 0.7f, 0.7f));
	app.materials["default_mesh_mat"] = default_mat;
	Material*	override_mat_ptr = nullptr;
	if (!override_mat_name.empty())
	{
		if (app.materials.find(override_mat_name) != app.materials.end())
			override_mat_ptr = app.materials[override_mat_name].get();
		else
			std::cerr << "Warning: materiel override '" << override_mat_name << "' introuvable." << std::endl;
	}
	for (const auto& shape : shapes)
	{
		size_t	index_offset = 0;

		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
		{
			size_t	fv = shape.mesh.num_face_vertices[f];

			if (fv != 3)
			{
				index_offset += fv;
				continue ;
			}

			Vec3f	verts[3];
			float	uvs[3][2] = {{0}};
			bool	has_uv = true;
			for (size_t v = 0; v < 3; v++)
			{
				tinyobj::index_t	idx = shape.mesh.indices[index_offset + v];
				//scale
				float	vx = attrib.vertices[3 * idx.vertex_index + 0] * scale;
				float	vy = attrib.vertices[3 * idx.vertex_index + 1] * scale;
				float	vz = attrib.vertices[3 * idx.vertex_index + 2] * scale;
				//rot X
				float	y1 = vy * cx - vz * sx;
				float	z1 = vy * sx + vz * cx;
				vy = y1; vz = z1;

				//rot Y
				float	x2 = vx * cy + vz * sy;
				float	z2 = -vx * sy + vz * cy;
				vx = x2; vz = z2;

				//rot Z
				float	x3 = vx * cz - vy * sz;
				float	y3 = vx * sz + vy * cz;
				vx = x3; vy = y3;

				//translation
				verts[v] = Vec3f(vx + pos._x, vy + pos._y, vz + pos._z);
				if (idx.texcoord_index >= 0)
				{
					uvs[v][0] = attrib.texcoords[2 * idx.texcoord_index + 0];
					uvs[v][1] = attrib.texcoords[2 * idx.texcoord_index + 1];
				}
				else
					has_uv = false;
			}

			int			mat_id = shape.mesh.material_ids[f];
			Material*	mat_ptr = nullptr;

			if (override_mat_ptr != nullptr)
				mat_ptr = override_mat_ptr;
			else if (mat_id >= 0 && (size_t)mat_id < converted_mats.size())
				mat_ptr = converted_mats[mat_id].get();
			else
				mat_ptr = default_mat.get();
			std::shared_ptr<Triangle> new_tr;
			if (has_uv)
				new_tr = std::make_shared<Triangle>(verts[0], verts[1], verts[2],
						mat_ptr, uvs[0], uvs[1], uvs[2]);
			else
				new_tr = std::make_shared<Triangle>(verts[0], verts[1], verts[2], mat_ptr);

			if (dynamic_cast<DiffuseLight*>(mat_ptr) != nullptr)
				app.scene.add_light(new_tr);
			else
				app.scene.add(new_tr);

			index_offset += fv;
		}
	}
	std::cout << "mesh charge: " << shapes.size() << " shapes, "
		<< obj_materials.size() << " materiaux" << std::endl;
}
