/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Thread_calls.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:55:29 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/10 16:18:08 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Render.hpp"
#include <thread>

void	thread_calls(Camera &cam, Render &render_total)
{
	if (cam.hasMoved())
	{
		render_total.frame_count = 1;
		cam.resetMovedFlag();
	}
	if (render_total.frame_count > (size_t)render_total.samples)
		return ;

	const int	COLS = 4;
	const int	ROWS = THREAD_MAX / COLS;

	size_t	tile_w = render_total.width / COLS;
	size_t	tile_h = render_total.height / ROWS;

	std::vector<unsigned int> seeds(THREAD_MAX);
	//knutth au ca ou si rand se repete
	for (int i = 0; i < THREAD_MAX; i++)
		seeds[i] = (unsigned int)rand() ^ (unsigned int)(i * 2654435761u);

	std::vector<Render>			jobs;
	std::vector<std::thread>	threads;

	jobs.reserve(THREAD_MAX);
	threads.reserve(THREAD_MAX);
	for (int i = 0; i < THREAD_MAX; i++)
	{
		int	col = i % COLS;
		int	row = i / COLS;

		Render	job = render_total;

		job.start_x = col * tile_w;
		job.end_x = (col == COLS - 1) ? render_total.width  : (col + 1) * tile_w;
		job.start_y = row * tile_h;
		job.end_y = (row == ROWS - 1) ? render_total.height : (row + 1) * tile_h;
		job.seed = &seeds[i];

		jobs.push_back(job);
	}
	for (int i = 0; i < THREAD_MAX; i++)
		threads.push_back(std::thread(render, std::ref(jobs[i])));
	for (auto& t : threads)
		t.join();
	render_total.frame_count++;
}
