/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SceneLoader.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 18:56:09 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/24 12:18:34 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <SDL3_image/SDL_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "SceneLoader.hpp"

std::string	SceneLoader::trim(const std::string& s)
{
	auto	start = s.begin();
	auto	end = s.end();

	while (start != s.end() && std::isspace(*start))
		start++;
	do
	{
		end--;
	} while (std::distance(start, end) > 0 && std::isspace(*end));
	return (std::string(start, end + 1));
}

Vec3f	SceneLoader::parseVec3(const std::string& val, Vec3f def)
{
	std::istringstream	iss(val);
	float				x, y, z;

	if (iss >> x >> y >> z)
		return (Vec3f(x, y, z));
	return (def);
}

bool	SceneLoader::loadRT(const std::string& filepath, AppContext& app)
{
	std::ifstream	file(filepath);
	std::string		token;
	std::string		name = "";
	std::string		next_token;

	if (!file.is_open())
	{
		std::cerr << "Erreur ouverture -> " << filepath << "\n";
		return (false);
	}

	while (file >> token)
	{
		if (token.empty() || token[0] == '#' || (token.size() >= 2 && token.substr(0, 2) == "//"))
		{
			std::string	dummy;
			
			std::getline(file, dummy);
			continue ;
		}

		file >> next_token;

		if (next_token != "{")
		{
			name = next_token;
			name.erase(std::remove(name.begin(), name.end(), '\"'), name.end());
			file >> next_token;
		}

		if (next_token != "{")
		{
			std::cerr << "Erreur de syntaxe: accolade '{' attendue apres " << token << "\n";
			continue ;
		}

		std::unordered_map<std::string, std::string>	params;
		std::string										key;
		std::string										val;
		
		while (file >> key && key != "}")
		{
			std::getline(file, val);
			params[key] = trim(val);
		}
		parseBlock(token, name, params, app);
	}
	file.close();
	return (true);
}

void	SceneLoader::parseBlock(const std::string& type, const std::string& name, std::unordered_map<std::string, std::string>& params, AppContext& app)
{
	if (type == "Settings" || type == "set")
		buildSettings(params, app);
	else if (type == "Camera" || type == "cam")
		buildCamera(params, app);
	else if (type == "Material" || type == "mat" || type == "newmat")
		buildMaterial(name, params, app);
	else if (type == "Sphere" || type == "sp")
		buildSphere(params, app);
	else if (type == "Plane" || type == "pl")
		buildPlane(params, app);
	else if (type == "Triangle" || type == "tr")
		buildTriangle(params, app);
	else if (type == "Quad" || type == "qu")
		buildQuad(params, app);
	else if (type == "Mesh" || type == "obj")
		buildMesh(params, app);
	else if (type == "Blackhole" || type == "bh")
		buildBlackHole(params, app);
	else if (type == "Environment" || type == "env" || type == "hdri")
		buildEnvironment(params, app);
	else
		std::cerr << "Type de bloc inconnu: " << type << "\n";
}


void	SceneLoader::buildSettings(std::unordered_map<std::string, std::string>& params, AppContext& app)
{
	if (params.count("resolution"))
	{
		std::istringstream	iss(params["resolution"]);
		
		iss >> app.width >> app.height;
	}
	if (params.count("samples"))
	{
		if (params["samples"] == "inf")
			app.samples = -1;
		else
			app.samples = std::stoi(params["samples"]);
	}
}

void	SceneLoader::buildCamera(std::unordered_map<std::string, std::string>& params, AppContext& app)
{
	Vec3f	lookFrom = params.count("position") ? parseVec3(params["position"]) : Vec3f(0.0f, 0.0f, 0.0f);
	Vec3f	dir = params.count("direction") ? parseVec3(params["direction"]) : Vec3f(0.0f, 0.0f, -1.0f);
	float	fov = params.count("fov") ? std::stof(params["fov"]) : 90.0f;
	float	lens_radius = params.count("lens_radius") ? std::stof(params["lens_radius"]) : 0.0f;
	float	focus_dist = params.count("focus_dist") ? std::stof(params["focus_dist"]) : 10.0f;
	Vec3f	lookAt = lookFrom + dir;
	
	app.camera = Camera(lookFrom, lookAt, Vec3f(0.0f, 1.0f, 0.0f), fov, (float)app.width / (float)app.height);
	app.camera.set_dof(lens_radius, focus_dist);
}

void	SceneLoader::buildMaterial(const std::string& name, std::unordered_map<std::string, std::string>& params, AppContext& app)
{
	std::string					type = params.count("type") ? params["type"] : "lambertian";
	std::shared_ptr<Material>	mat = nullptr;

	if (type == "metal")
		mat = std::make_shared<Metal>();
	else if (type == "emissive")
		mat = std::make_shared<DiffuseLight>();
	else if (type == "dielectric" || type == "glass")
		mat = std::make_shared<Dielectric>();
	else if (type == "pbr")
		mat = std::make_shared<PBRMaterial>();
	else
		mat = std::make_shared<Lambertian>();

	if (params.count("color"))
		mat->setColor(parseVec3(params["color"]));
	if (params.count("albedo"))
		mat->setColor(parseVec3(params["albedo"]));
	
	if (params.count("fuzz"))
	{
		float	f = std::stof(params["fuzz"]);
		
		if (auto m = std::dynamic_pointer_cast<Metal>(mat))
			m->setFuzz(f);
		if (auto d = std::dynamic_pointer_cast<Dielectric>(mat))
			d->setFuzz(f);
	}
	
	if (params.count("ior") || params.count("ni"))
	{
		float	ior = std::stof(params.count("ior") ? params["ior"] : params["ni"]);
		
		if (auto d = std::dynamic_pointer_cast<Dielectric>(mat))
			d->setNi(ior);
	}

	if (params.count("intensity"))
		mat->setIntensity(std::stof(params["intensity"]));

	auto load_mat_tex = [&](const std::string& path) -> SDL_Surface*
	{
		SDL_Surface	*raw = IMG_Load(path.c_str());
		if (!raw)
		{
			std::cerr << "Erreur load texture: " << path << "\n";
			return (nullptr);
		}
		SDL_Surface	*converted = SDL_ConvertSurface(raw, SDL_PIXELFORMAT_RGBA32);
		SDL_DestroySurface(raw);
		return (converted);
	};
	if (params.count("texture"))
		mat->setTexture(load_mat_tex(params["texture"]));
	if (params.count("normal"))
		mat->setNormal(load_mat_tex(params["normal"]));
	if (params.count("roughness"))
		mat->setRoughness(load_mat_tex(params["roughness"]));
	if (params.count("metallic"))
		mat->setMetallic(load_mat_tex(params["metallic"]));
	if (params.count("occlusion"))
		mat->setOcclusion(load_mat_tex(params["occlusion"]));
	if (params.count("emissive_map"))
		mat->setEmissive(load_mat_tex(params["emissive_map"]));
	if (params.count("tex_scale"))
		mat->setTexScale(std::stof(params["tex_scale"]));
	if (params.count("albedo_scale"))
		mat->setAlbedoScale(std::stof(params["albedo_scale"]));
	if (params.count("normal_scale"))
		mat->setNormalScale(std::stof(params["normal_scale"]));
	if (params.count("roughness_scale"))
		mat->setRoughnessScale(std::stof(params["roughness_scale"]));
	if (params.count("metallic_scale"))
		mat->setMetallicScale(std::stof(params["metallic_scale"]));
	if (params.count("occlusion_scale"))
		mat->setOcclusionScale(std::stof(params["occlusion_scale"]));
	if (params.count("emissive_scale"))
		mat->setEmissiveScale(std::stof(params["emissive_scale"]));
	if (params.count("normal_strength"))
		mat->setNormalStrength(std::stof(params["normal_strength"]));
	app.materials[name] = mat;
}

void	SceneLoader::buildSphere(std::unordered_map<std::string, std::string>& params, AppContext& app)
{
	Vec3f		center = params.count("position") ? parseVec3(params["position"]) : Vec3f(0.0f, 0.0f, 0.0f);
	Vec3f		normal = params.count("normal") ? parseVec3(params["normal"]) : Vec3f(0.0f, 1.0f, 0.0f);
	float		radius = params.count("radius") ? std::stof(params["radius"]) : 1.0f;
	float 		roll = params.count("roll") ? std::stof(params["roll"]) * (float)M_PI / 180.0f : 0.0f;
	std::string	mat_name = params["material"];

	if (app.materials.find(mat_name) == app.materials.end())
	{
		std::cerr << "Sphere: Material " << mat_name << " not found\n";
		return ;
	}

	Vec3f		rot = normal.to_rotation_angles();
	Mat4f		translation = Mat4f::translate(center);
	Mat4f		rotX = Mat4f::rotateX(rot._x);
	Mat4f		rotY = Mat4f::rotateY(rot._y);
	Mat4f		scale = Mat4f::scale(Vec3f(radius, radius, radius));
	Mat4f		roll_mat = Mat4f::rotate(roll, normal);
	Mat4f		transform = translation * roll_mat * rotY * rotX * scale;
	Material*	mat_ptr = app.materials[mat_name].get();
	auto		new_sphere = std::make_shared<Sphere>(radius, center, normal, transform, mat_ptr);

	if (dynamic_cast<DiffuseLight*>(mat_ptr))
		app.scene.add_light(new_sphere);
	else
		app.scene.add(new_sphere);
}

void	SceneLoader::buildPlane(std::unordered_map<std::string, std::string>& params, AppContext& app)
{
	Vec3f		point = params.count("position") ? parseVec3(params["position"]) : Vec3f(0.0f, 0.0f, 0.0f);
	Vec3f		normal = params.count("normal") ? parseVec3(params["normal"]) : Vec3f(0.0f, 1.0f, 0.0f);
	float		roll = params.count("roll") ? std::stof(params["roll"]) * (float)M_PI / 180.0f : 0.0f;
	std::string	mat_name = params["material"];

	normal = Vec3f::normalize(normal);
	if (app.materials.find(mat_name) == app.materials.end())
		return ;

	Vec3f	up(0.0f, 1.0f, 0.0f);
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

	Mat4f		roll_mat = Mat4f::rotate(roll, normal);
	Mat4f		transform = Mat4f::translate(point) * roll_mat * rotation;
	Material*	mat_ptr = app.materials[mat_name].get();
	auto		new_pl = std::make_shared<Plane>(point, normal, transform, mat_ptr);

	if (dynamic_cast<DiffuseLight*>(mat_ptr))
		app.scene.add_light(new_pl);
	else
		app.scene.add(new_pl);
}

void	SceneLoader::buildTriangle(std::unordered_map<std::string, std::string>& params, AppContext& app)
{
	Vec3f		v0 = params.count("v0") ? parseVec3(params["v0"]) : Vec3f(0.0f, 0.0f, 0.0f);
	Vec3f		v1 = params.count("v1") ? parseVec3(params["v1"]) : Vec3f(1.0f, 0.0f, 0.0f);
	Vec3f		v2 = params.count("v2") ? parseVec3(params["v2"]) : Vec3f(0.0f, 1.0f, 0.0f);
	std::string	mat_name = params["material"];

	if (app.materials.find(mat_name) == app.materials.end())
		return ;
	
	Material*	mat_ptr = app.materials[mat_name].get();
	auto		new_tr = std::make_shared<Triangle>(v0, v1, v2, mat_ptr);

	if (dynamic_cast<DiffuseLight*>(mat_ptr))
		app.scene.add_light(new_tr);
	else
		app.scene.add(new_tr);
}

void	SceneLoader::buildQuad(std::unordered_map<std::string, std::string>& params, AppContext& app)
{
	Vec3f		center = params.count("position") ? parseVec3(params["position"]) : Vec3f(0.0f, 0.0f, 0.0f);
	Vec3f		normal = params.count("normal") ? parseVec3(params["normal"]) : Vec3f(0.0f, 1.0f, 0.0f);
	float		w = params.count("width") ? std::stof(params["width"]) : 1.0f;
	float		h = params.count("height") ? std::stof(params["height"]) : 1.0f;
	float		roll = params.count("roll") ? std::stof(params["roll"]) * (float)M_PI / 180.0f : 0.0f;
	std::string	mat_name = params["material"];

	normal = Vec3f::normalize(normal);
	if (app.materials.find(mat_name) == app.materials.end())
		return ;

	Vec3f	up(0.0f, 1.0f, 0.0f);
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

	Mat4f		mat_scale = Mat4f::scale(Vec3f(w / 2.0f, 1.0f, h / 2.0f));
	Mat4f		roll_mat = Mat4f::rotate(roll, normal);
	Mat4f		final_transform = Mat4f::translate(center) * roll_mat * rotation * mat_scale;
	Material*	mat_ptr = app.materials[mat_name].get();
	auto		new_quad = std::make_shared<Quad>(center, normal, final_transform, mat_ptr, w, h);

	if (dynamic_cast<DiffuseLight*>(mat_ptr))
		app.scene.add_light(new_quad);
	else
		app.scene.add(new_quad);
}

void	SceneLoader::buildMesh(std::unordered_map<std::string, std::string>& params, AppContext& app)
{
	if (!params.count("path"))
		return ;

	std::string		path = params["path"];
	float			scale = params.count("scale") ? std::stof(params["scale"]) : 1.0f;
	Vec3f			pos = params.count("position") ? parseVec3(params["position"]) : Vec3f(0.0f, 0.0f, 0.0f);
	Vec3f			normal = params.count("normal") ? parseVec3(params["normal"]) : Vec3f(0.0f, 1.0f, 0.0f);
	float			roll = params.count("roll") ? std::stof(params["roll"]) : 0.0f;
	std::string		override_mat = params.count("material") ? params["material"] : "";

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

	const auto&	attrib = reader.GetAttrib();
	const auto&	shapes = reader.GetShapes();
	const auto&	obj_materials = reader.GetMaterials();

	auto loadTex = [&](const std::string& name) -> SDL_Surface*
	{
		if (name.empty())
			return (nullptr);
		std::string		tex_path = config.mtl_search_path + name;
		SDL_Surface*	raw = IMG_Load(tex_path.c_str());
		if (!raw)
			return (nullptr);
		SDL_Surface*	conv = SDL_ConvertSurface(raw, SDL_PIXELFORMAT_RGBA32);
		SDL_DestroySurface(raw);
		return (conv);
	};

	auto applyScales = [&](std::shared_ptr<Material> m)
	{
		if (params.count("tex_scale"))
			m->setTexScale(std::stof(params["tex_scale"]));
		if (params.count("albedo_scale"))
			m->setAlbedoScale(std::stof(params["albedo_scale"]));
		if (params.count("normal_scale"))
			m->setNormalScale(std::stof(params["normal_scale"]));
		if (params.count("roughness_scale"))
			m->setRoughnessScale(std::stof(params["roughness_scale"]));
		if (params.count("metallic_scale"))
			m->setMetallicScale(std::stof(params["metallic_scale"]));
		if (params.count("occlusion_scale"))
			m->setOcclusionScale(std::stof(params["occlusion_scale"]));
		if (params.count("emissive_scale"))
			m->setEmissiveScale(std::stof(params["emissive_scale"]));
	};
	std::vector<std::shared_ptr<Material>>	converted_mats;

	for (const auto& m : obj_materials)
	{
		std::shared_ptr<Material>	mat;
		bool						is_emissive = (m.emission[0] + m.emission[1] + m.emission[2]) > 0.0f || !m.emissive_texname.empty();
		
		if (is_emissive || m.illum == 0)
		{
			auto	light = std::make_shared<DiffuseLight>();
			Vec3f	emit = is_emissive ? Vec3f(m.emission[0], m.emission[1], m.emission[2]) : Vec3f(m.diffuse[0], m.diffuse[1], m.diffuse[2]);
			
			light->setColor(emit);
			
			std::string tex_name = m.emissive_texname.empty() ? m.diffuse_texname : m.emissive_texname;
			if (SDL_Surface* s = loadTex(tex_name))
			{
				light->setTexture(s);
				if (!m.emissive_texname.empty())
					light->setEmissiveScale(m.emissive_texopt.scale[0]);
				else if (!m.diffuse_texname.empty())
					light->setAlbedoScale(m.diffuse_texopt.scale[0]);
			}
			mat = light;
		}
		else if (m.illum == 4 || m.illum == 6 || m.illum == 7 || m.illum == 9 || m.dissolve < 1.0f)
		{
			auto	glass = std::make_shared<Dielectric>();
			
			glass->setNi(m.ior > 0.0f ? m.ior : 1.5f);
			glass->setColor(Vec3f(m.diffuse[0], m.diffuse[1], m.diffuse[2]));
			if (SDL_Surface* s = loadTex(m.diffuse_texname))
			{
				glass->setTexture(s);
				glass->setAlbedoScale(m.diffuse_texopt.scale[0]);
			}
			mat = glass;
		}
		else if (m.illum == 3 || m.illum == 5)
		{
			auto	metal = std::make_shared<Metal>();
			float	fuzz = (m.shininess > 0.0f) ? std::fmax(0.0f, 1.0f - m.shininess / 1000.0f) : 0.0f;
			
			metal->setColor(Vec3f(m.diffuse[0], m.diffuse[1], m.diffuse[2]));
			metal->setFuzz(fuzz);
			if (SDL_Surface* s = loadTex(m.diffuse_texname))
			{
				metal->setTexture(s);
				metal->setAlbedoScale(m.diffuse_texopt.scale[0]);
			}
			mat = metal;
		}
		else
		{
			auto	pbr = std::make_shared<PBRMaterial>();
			float	rough = (m.illum == 1) ? 1.0f : (m.roughness > 0.0f ? m.roughness : std::fmax(0.0f, 1.0f - m.shininess / 1000.0f));
			
			pbr->setColor(Vec3f(m.diffuse[0], m.diffuse[1], m.diffuse[2]));
			
			if (SDL_Surface* s = loadTex(m.diffuse_texname))
			{
				pbr->setTexture(s);
				pbr->setAlbedoScale(m.diffuse_texopt.scale[0]);
			}
			
			pbr->setRoughnessScalar(rough);
			if (SDL_Surface* s = loadTex(m.roughness_texname))
			{
				pbr->setRoughness(s);
				pbr->setRoughnessScale(m.roughness_texopt.scale[0]);
			}
			
			pbr->setMetallicScalar(m.metallic);
			if (SDL_Surface* s = loadTex(m.metallic_texname))
			{
				pbr->setMetallic(s);
				pbr->setMetallicScale(m.metallic_texopt.scale[0]);
			}
			
			std::string normal_name = m.normal_texname.empty() ? m.bump_texname : m.normal_texname;
			if (SDL_Surface* s = loadTex(normal_name))
			{
				pbr->setNormal(s);
				if (!m.normal_texname.empty())
					pbr->setNormalScale(m.normal_texopt.scale[0]);
				else if (!m.bump_texname.empty())
					pbr->setNormalScale(m.bump_texopt.scale[0]);
			}
			
			if (SDL_Surface* s = loadTex(m.ambient_texname))
			{
				pbr->setOcclusion(s);
				pbr->setOcclusionScale(m.ambient_texopt.scale[0]);
			}
			
			mat = pbr;
		}
		//surcharge fichier de base gagnant
		applyScales(mat);
		
		converted_mats.push_back(mat);
		app.materials["mesh_mat_" + std::to_string(converted_mats.size() - 1)] = mat;
	}

	std::shared_ptr<Material>	default_mat = std::make_shared<Lambertian>();
	
	default_mat->setColor(Vec3f(0.7f, 0.7f, 0.7f));
	applyScales(default_mat);
	app.materials["default_mesh_mat"] = default_mat;

	Material*	override_mat_ptr = nullptr;
	
	if (!override_mat.empty() && app.materials.find(override_mat) != app.materials.end())
		override_mat_ptr = app.materials[override_mat].get();

	auto	mesh = std::make_shared<Mesh>();

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

				float	vx = attrib.vertices[3 * idx.vertex_index + 0] * scale;
				float	vy = attrib.vertices[3 * idx.vertex_index + 1] * scale;
				float	vz = attrib.vertices[3 * idx.vertex_index + 2] * scale;

				float	y1 = vy * cx - vz * sx;
				float	z1 = vy * sx + vz * cx;
				vy = y1; vz = z1;

				float	x2 = vx * cy + vz * sy;
				float	z2 = -vx * sy + vz * cy;
				vx = x2; vz = z2;

				float	x3 = vx * cz - vy * sz;
				float	y3 = vx * sz + vy * cz;
				vx = x3; vy = y3;

				verts[v] = Vec3f(vx + pos._x, vy + pos._y, vz + pos._z);

				if (idx.texcoord_index >= 0)
				{
					uvs[v][0] = attrib.texcoords[2 * idx.texcoord_index + 0];
					uvs[v][1] = attrib.texcoords[2 * idx.texcoord_index + 1];
				}
				else
				{
					has_uv = false;
				}
			}

			int			mat_id = shape.mesh.material_ids[f];
			Material*	mat_ptr = default_mat.get();
			
			if (override_mat_ptr)
				mat_ptr = override_mat_ptr;
			else if (mat_id >= 0 && (size_t)mat_id < converted_mats.size())
				mat_ptr = converted_mats[mat_id].get();

			std::shared_ptr<Triangle>	new_tr;
			
			if (has_uv)
				new_tr = std::make_shared<Triangle>(verts[0], verts[1], verts[2], mat_ptr, uvs[0], uvs[1], uvs[2]);
			else
				new_tr = std::make_shared<Triangle>(verts[0], verts[1], verts[2], mat_ptr);

			if (dynamic_cast<DiffuseLight*>(mat_ptr))
				app.scene.add_light(new_tr);
			mesh->addTriangle(new_tr);

			index_offset += fv;
		}
	}

	if (mesh->triCount() > 0)
	{
		mesh->build();
		app.scene.add(mesh);
		app.meshes.push_back(mesh);
	}
	std::cout << "Mesh charge: " << path << " (" << mesh->triCount() << " triangles)\n";
}

void	SceneLoader::buildBlackHole(std::unordered_map<std::string, std::string>& params, AppContext& app)
{
	Vec3f	pos = params.count("position") ? parseVec3(params["position"]) : Vec3f(0.0f, 0.0f, 0.0f);
	float	rs = params.count("radius") ? std::stof(params["radius"]) : 1.0f;
	
	app.black_hole_enabled = true;
	app.black_hole = BlackHole(pos, rs);
}

void	SceneLoader::buildEnvironment(std::unordered_map<std::string, std::string>& params, AppContext& app)
{
	if (params.count("path"))
	{
		if (!app.env_map.load(params["path"]))
			std::cerr << "Erreur chargement HDRI: " << params["path"] << std::endl;
	}
	if (params.count("intensity"))
		app.env_map.setIntensity(std::stof(params["intensity"]));
	if (params.count("rotation"))
		app.env_map.setRotation(std::stof(params["rotation"]) * (float)M_PI / 180.0f);
}
