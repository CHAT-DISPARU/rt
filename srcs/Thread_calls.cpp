/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Thread_calls.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:55:29 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/09 15:28:12 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Render.hpp"
#include <thread>

void	thread_calls(Camera &cam, Scene &scene, Render &render_total)
{
	if (cam.hasMoved())
	{
		render_total.frame_count = 1;
		render_total.cam.resetMovedFlag();
	}
	std::vector <std::thread> threads;

	for (int i = 0; i < THREAD_MAX; i++)
	{
		int col = i % THREAD_MAX;
		int row = i % THREAD_MAX;
		render_total.start_x = col * render_total.width;
		render_total.end_x = col == THREAD_MAX - 1 ? render_total.width : (col + 1) * render_total.width;
		render_total.start_y = row * render_total.height;
		render_total.end_y = row == THREAD_MAX - 1 ? render_total.height : (row + 1) * render_total.height;
		std::thread t(render, render_total);
		threads.push_back(t);
	}
	for (auto it = threads.begin(); it != threads.end(); ++it)
		it->join();
	render_total.frame_count++;
}
