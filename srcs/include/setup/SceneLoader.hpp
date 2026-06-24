/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SceneLoader.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 16:41:16 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/23 18:52:51 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>
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
#include "PBRMaterial.hpp"
#include "Metal.hpp"
#include "Mesh.hpp"
#include "BlackHole.hpp"
#include "EnvironmentMap.hpp"

struct	AppContext
{
	AppContext(){};
	~AppContext(){};
	
	Scene	scene;
	Camera	camera;
	std::unordered_map<std::string, std::shared_ptr<Material>>	materials;
	std::vector<std::shared_ptr<Mesh>>	meshes;
	int				width = 1920;
	int				height = 1080;
	int				samples = -1;
	size_t			frame_count = 0;
	bool			black_hole_enabled = false;
	BlackHole		black_hole = BlackHole(Vec3f(0.0f, 0.0f, 0.0f), 1.0f);
	EnvironmentMap	env_map;
};

class SceneLoader
{
	public:
		static bool		loadRT(const std::string& filepath, AppContext& app);

	private:
		static void			parseBlock(const std::string& type, const std::string& name, std::unordered_map<std::string, std::string>& params, AppContext& app);
		static void			buildSettings(std::unordered_map<std::string, std::string>& params, AppContext& app);
		static void			buildCamera(std::unordered_map<std::string, std::string>& params, AppContext& app);
		static void			buildMaterial(const std::string& name, std::unordered_map<std::string, std::string>& params, AppContext& app);
		static void			buildSphere(std::unordered_map<std::string, std::string>& params, AppContext& app);
		static void			buildPlane(std::unordered_map<std::string, std::string>& params, AppContext& app);
		static void			buildTriangle(std::unordered_map<std::string, std::string>& params, AppContext& app);
		static void			buildQuad(std::unordered_map<std::string, std::string>& params, AppContext& app);
		static void			buildMesh(std::unordered_map<std::string, std::string>& params, AppContext& app);
		static void			buildBlackHole(std::unordered_map<std::string, std::string>& params, AppContext& app);
		static void			buildEnvironment(std::unordered_map<std::string, std::string>& params, AppContext& app);
		static std::string	trim(const std::string& s);
		static Vec3f		parseVec3(const std::string& val, Vec3f def = Vec3f(0.0f, 0.0f, 0.0f));
};