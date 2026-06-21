/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 16:17:25 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/20 13:06:37 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.hpp"

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "use: " << av[0] << " <scene_file.rt>\n";
		return (1);
	}

	AppContext	app;

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
	app.scene.build();
	auto	endbvh = std::chrono::high_resolution_clock::now();
	auto	durationbvh = std::chrono::duration_cast<std::chrono::milliseconds>(endbvh - startbvh);
	std::cout << "[BVH] Construction time: " << durationbvh.count() << " ms" << std::endl;
	srand(static_cast<unsigned int>(time(NULL)));



	SDLContext		sdl;
	BvhDebugConfig  bvh_debug; 
	Render			render_total(app.camera, app.scene, bvh_debug, app.env_map);
	ThreadPool		threads(THREAD_MAX);

	bvh_debug.tree_depth = app.scene.getMaxDepth();
	std::cout << "[BVH] Max depth: " << bvh_debug.tree_depth << "\n";
	setup_base_render(app, render_total);
	if (!sdl_init(sdl, app.width, app.height))
	{
		cleanup_render(render_total);
		clean_gui();
		sdl_cleanup(sdl);
		return (1);
	}
	//boucle exec
	main_loop(sdl, app, render_total, threads);
	cleanup_render(render_total);
	clean_gui();
	sdl_cleanup(sdl);
	return (0);
}
