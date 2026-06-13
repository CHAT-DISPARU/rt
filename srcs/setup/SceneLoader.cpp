/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SceneLoader.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 14:48:47 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/13 11:34:32 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
}