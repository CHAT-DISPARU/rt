/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 16:17:25 by gajanvie          #+#    #+#             */
/*   Updated: 2026/08/31 11:10:40 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.hpp"
#include "vk_textures.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "use: " << av[0] << " <scene_file.rt>\n";
		return (1);
	}

	AppContext  app;

	std::cout << "Loading scene file: " << av[1] << "..." << std::endl;
	if (!SceneLoader::loadRT(av[1], app))
	{
		std::cerr << "Failed to load scene." << std::endl;
		return (1);
	}
	std::cout << "Scene loaded\n";
	std::cout << "Resolution: " << app.width << "x" << app.height << "\n";
	std::cout << "Samples: " << app.samples << "\n";
	std::cout << "\n[BVH] Building..." << std::endl;
	auto	startbvh = std::chrono::high_resolution_clock::now();
	app.scene.build_bvhs();
	auto	endbvh = std::chrono::high_resolution_clock::now();
	auto	durationbvh = std::chrono::duration_cast<std::chrono::milliseconds>(endbvh - startbvh);
	std::cout << "[BVH] Construction time: " << durationbvh.count() << " ms" << std::endl;

	std::cout << "\n[PACK]japplatis la scene pour le gpu" << std::endl;
	auto	startpack = std::chrono::high_resolution_clock::now();
	
	ScenePacker	packer;
	packer.pack_scene_to_gpu(app);
	
	auto	endpack = std::chrono::high_resolution_clock::now();
	auto	durationpack = std::chrono::duration_cast<std::chrono::milliseconds>(endpack - startpack);
	
	std::cout << "[PACK] time applatissement: " << durationpack.count() << " ms\n";
	std::cout << "  -> Materials: " << packer.gpu_materials.size() << " (" << packer.gpu_materials.size() * sizeof(GPUMaterial) << " bites)\n";
	std::cout << "  -> Triangles: " << packer.gpu_triangles.size() << " (" << packer.gpu_triangles.size() * sizeof(GPUTriangle) << " bites)\n";
	std::cout << "  -> Spheres: " << packer.gpu_spheres.size()   << " (" << packer.gpu_spheres.size() * sizeof(GPUSphere) << " bites)\n";
	std::cout << "  -> Quads: " << packer.gpu_quads.size()     << " (" << packer.gpu_quads.size() * sizeof(GPUQuad) << " bites)\n";
	std::cout << "  -> Planes: " << packer.gpu_planes.size()    << " (" << packer.gpu_planes.size() * sizeof(GPUPlane) << " bites)\n";
	std::cout << "  -> Textures: " << packer.gpu_texture_sources.size() << "\n";
	srand(static_cast<unsigned int>(time(NULL)));

	VulkanContext vCtx;

	if (!init_vulkan(vCtx))
		return (1);
	std::cout << "\n[VULKAN] scene envoie to VRAM..." << std::endl;
	VulkanBuffer	gpu_mat_buffer = createAndUploadSSBO(vCtx.device, vCtx.physicalDevice, vCtx.commandPool, vCtx.computeQueue, packer.gpu_materials);
	VulkanBuffer	gpu_tri_buffer = createAndUploadSSBO(vCtx.device, vCtx.physicalDevice, vCtx.commandPool, vCtx.computeQueue, packer.gpu_triangles);
	VulkanBuffer	gpu_sph_buffer = createAndUploadSSBO(vCtx.device, vCtx.physicalDevice, vCtx.commandPool, vCtx.computeQueue, packer.gpu_spheres);
	VulkanBuffer	gpu_qd_buffer = createAndUploadSSBO(vCtx.device, vCtx.physicalDevice, vCtx.commandPool, vCtx.computeQueue, packer.gpu_quads);
	VulkanBuffer	gpu_bvh_tri_buffer = createAndUploadSSBO(vCtx.device, vCtx.physicalDevice, vCtx.commandPool, vCtx.computeQueue, packer.gpu_bvh_triangles);
	VulkanBuffer	gpu_bvh_sph_buffer = createAndUploadSSBO(vCtx.device, vCtx.physicalDevice, vCtx.commandPool, vCtx.computeQueue, packer.gpu_bvh_spheres);
	VulkanBuffer	gpu_bvh_qd_buffer = createAndUploadSSBO(vCtx.device, vCtx.physicalDevice, vCtx.commandPool, vCtx.computeQueue, packer.gpu_bvh_quads);
	VulkanBuffer	gpu_light_buffer = createAndUploadSSBO(vCtx.device, vCtx.physicalDevice, vCtx.commandPool, vCtx.computeQueue, packer.gpu_lights);
	VulkanBuffer	gpu_pl_buffer = createAndUploadSSBO(vCtx.device, vCtx.physicalDevice, vCtx.commandPool, vCtx.computeQueue, packer.gpu_planes);

std::cout << "\n[VULKAN] upload des textures bindless..." << std::endl;
	std::vector<GPUTextureImage>	gpu_textures;
	gpu_textures.reserve(packer.gpu_texture_sources.size());
	for (SDL_Surface* surf : packer.gpu_texture_sources)
	{
SDL_Surface*	rgba = surf;
		bool			need_free = false;

		if (surf->format != SDL_PIXELFORMAT_RGBA32)
		{
			rgba = SDL_ConvertSurface(surf, SDL_PIXELFORMAT_RGBA32);
			if (!rgba)
			{
				std::cerr << "SDL_ConvertSurface a echoue: " << SDL_GetError() << "\n";
				continue;
			}
			need_free = true;
		}

		const uint8_t*			upload_ptr = (const uint8_t*)rgba->pixels;
		std::vector<uint8_t>	tight_buffer;

		if (rgba->pitch != rgba->w * 4)
		{
			tight_buffer.resize((size_t)rgba->w * rgba->h * 4);
			for (int y = 0; y < rgba->h; y++)
				memcpy(tight_buffer.data() + (size_t)y * rgba->w * 4,
					   (const uint8_t*)rgba->pixels + (size_t)y * rgba->pitch,
					   (size_t)rgba->w * 4);
			upload_ptr = tight_buffer.data();
		}

		gpu_textures.push_back(createTextureImage(vCtx.device, vCtx.physicalDevice,
			vCtx.commandPool, vCtx.computeQueue,
			upload_ptr, (uint32_t)rgba->w, (uint32_t)rgba->h,
			/*isColorData=*/true));

		if (need_free)
			SDL_DestroySurface(rgba);
	}
	VkSampler	bindlessSampler = createGlobalBindlessSampler(vCtx.device);

	if (!init_descriptors(vCtx, app.width, app.height,
						  gpu_mat_buffer, gpu_tri_buffer, gpu_sph_buffer, gpu_qd_buffer, gpu_pl_buffer,
						  gpu_bvh_tri_buffer, gpu_bvh_sph_buffer, gpu_bvh_qd_buffer, gpu_light_buffer,
						  gpu_textures, bindlessSampler))
	{
		std::cerr << "Failed to init Vulkan descriptors" << std::endl;
		return (1);
	}
	if (!init_pipeline(vCtx))
    {
        std::cerr << "Failed to init Vulkan pipeline" << std::endl;
        return (1);
    }
	SDLContext		sdl;
	BvhDebugConfig	bvh_debug; 

	Render			render_total(app.camera, app.scene, bvh_debug, app.env_map);
	ThreadPool		threads(THREAD_MAX);

	setup_base_render(app, render_total);
	
	if (!sdl_init(sdl, app.width, app.height))
	{
		cleanup_render(render_total);
		clean_gui();
		sdl_cleanup(sdl);
		return (1);
	}
	main_loop(sdl, app, render_total, vCtx, threads);
	cleanup_render(render_total);
	clean_gui();
	sdl_cleanup(sdl);

	// nettoyage des textures bindless AVANT cleanup_vulkan (qui detruit le device)
	for (auto& tex : gpu_textures)
		destroyTextureImage(vCtx.device, tex);
	if (bindlessSampler)
		vkDestroySampler(vCtx.device, bindlessSampler, nullptr);

	cleanup_vulkan(vCtx);
	return (0);
}
