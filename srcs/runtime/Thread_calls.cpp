/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Thread_calls.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:55:29 by gajanvie          #+#    #+#             */
/*   Updated: 2026/07/16 12:33:11 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Render.hpp"
#include "BVHdebug.hpp"
#include "ThreadPool.hpp"
#include <thread>
#include <future>
#include <complex>
#include <cstddef>
#include "external/pocketfft_hdronly.h"

void	apply_post_processing(Render &render_total)
{
	int	w = render_total.width;
	int	h = render_total.height;
	int	total_pixels = w * h;
	std::vector<Vec3f>	bright_pixels(total_pixels, Vec3f(0.0f));

	if (render_total.bloom_enabled)
	{
		// threesold
		for (int i = 0; i < total_pixels; i++)
		{
			Vec3f base_color = render_total.accum_buffer[i] / (float)render_total.frame_count;
			float lum = base_color._x * 0.2126f + base_color._y * 0.7152f + base_color._z * 0.0722f;
			if (lum > render_total.bloom_threshold)
				bright_pixels[i] = base_color;
		}
		// allocation des buffer pour l image frequentiel
		std::vector<std::complex<float>> c_R(total_pixels, 0.0f);
		std::vector<std::complex<float>> c_G(total_pixels, 0.0f);
		std::vector<std::complex<float>> c_B(total_pixels, 0.0f);
		std::vector<std::complex<float>> c_Kernel(total_pixels, 0.0f);
		// Separation de l'image (domaine spatial) en trois canaux distincts
		for (int i = 0; i < total_pixels; i++)
		{
			c_R[i] = bright_pixels[i]._x;
			c_G[i] = bright_pixels[i]._y;
			c_B[i] = bright_pixels[i]._z;
		}

		int		radius = render_total.blur_radius;
		float	weight_sum = 0.0f;
		//disque parfait simulant ouverture de la camera
		// la FFT utilise le wrapping (modulo) pour couper le disque en 4 et le repartir dans les 4 coin du tableau
		for (int y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++)
			{
				if (x * x + y * y <= radius * radius)
				{
					int	wrap_x = (x >= 0) ? x : (w + x);
					int	wrap_y = (y >= 0) ? y : (h + y);
					c_Kernel[wrap_y * w + wrap_x] = 1.0f;
					weight_sum += 1.0f;
				}
			}
		}
		// normalise pour conserver energie totale de l'image
		for (int i = 0; i < total_pixels; i++)
			c_Kernel[i] /= weight_sum;

		// Configuration des dimensions pour PocketFF
		pocketfft::shape_t shape{ (size_t)h, (size_t)w };
		pocketfft::stride_t stride{ (ptrdiff_t)(sizeof(std::complex<float>) * w), (ptrdiff_t)(sizeof(std::complex<float>)) };
		pocketfft::shape_t axes{ 0, 1 };// FFT 2D : Axe Y (0) et Axe X (1)
		// transformees Directes (Forward) : Passage dans le domaine frequentiel
		pocketfft::c2c(shape, stride, stride, axes, pocketfft::FORWARD, c_R.data(), c_R.data(), 1.0f);
		pocketfft::c2c(shape, stride, stride, axes, pocketfft::FORWARD, c_G.data(), c_G.data(), 1.0f);
		pocketfft::c2c(shape, stride, stride, axes, pocketfft::FORWARD, c_B.data(), c_B.data(), 1.0f);
		pocketfft::c2c(shape, stride, stride, axes, pocketfft::FORWARD, c_Kernel.data(), c_Kernel.data(), 1.0f);
		// Multiplication Element par Element (Le Theoreme de Convolution)
		//En multipliant les frequences de l'image par les frequences du kernel,
		// on applique la fonction d'etalement du point (PSF) de la lentille.
		for (int i = 0; i < total_pixels; i++)
		{
			c_R[i] *= c_Kernel[i];
			c_G[i] *= c_Kernel[i];
			c_B[i] *= c_Kernel[i];
		}
		// Transformees Inverses (Backward) : Retour dans le domaine spatial
		// PocketFFT necessite une division par le nombre total de pixels lors du retour.
		float	norm = 1.0f / (float)total_pixels; 
		pocketfft::c2c(shape, stride, stride, axes, pocketfft::BACKWARD, c_R.data(), c_R.data(), norm);
		pocketfft::c2c(shape, stride, stride, axes, pocketfft::BACKWARD, c_G.data(), c_G.data(), norm);
		pocketfft::c2c(shape, stride, stride, axes, pocketfft::BACKWARD, c_B.data(), c_B.data(), norm);
		//Extraction de la partie reelle
		// On ecarte la partie imaginaire (qui devrait etre proche de zero) pour 
		// recuperer les vraies couleurs convoluees. Utilisation de fmax pour 
		//eviter les valeurs negatives liees a l'imprecision flottante.
		for (int i = 0; i < total_pixels; i++)
		{
			bright_pixels[i] = Vec3f(
				std::fmax(0.0f, c_R[i].real()), 
				std::fmax(0.0f, c_G[i].real()), 
				std::fmax(0.0f, c_B[i].real())
			);
		}
	}
	//reinhard et gamma
	for (int i = 0; i < w * h; i++)
	{
		Vec3f base = render_total.accum_buffer[i];
		
		if (render_total.bvh_debug.mode == BvhDebugMode::OFF)
			base = base / (float)render_total.frame_count;

		Vec3f fc = base;
		if (render_total.bloom_enabled)
			fc += bright_pixels[i] * render_total.bloom_intensity;
			
		fc._x = std::fmax(0.0f, fc._x);
		fc._y = std::fmax(0.0f, fc._y);
		fc._z = std::fmax(0.0f, fc._z);
		fc._x = std::pow(fc._x / (fc._x + 1.0f), 1.0f / 2.2f);
		fc._y = std::pow(fc._y / (fc._y + 1.0f), 1.0f / 2.2f);
		fc._z = std::pow(fc._z / (fc._z + 1.0f), 1.0f / 2.2f);
		int ir = (int)(255.999f * std::fmin(1.0f, fc._x));
		int ig = (int)(255.999f * std::fmin(1.0f, fc._y));
		int ib = (int)(255.999f * std::fmin(1.0f, fc._z));

		render_total.definitive[i] = (ir << 24) | (ig << 16) | (ib << 8) | 0xFF;
	}
}

void	thread_calls(Camera &cam, Render &render_total, ThreadPool &threads)
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

	jobs.reserve(THREAD_MAX);
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
	std::atomic<int>		tasks_left(THREAD_MAX);
	std::mutex				wait_mutex;
	std::condition_variable	wait_cv;

	for (int i = 0; i < THREAD_MAX; i++)
	{
		threads.enqueue([&ljob = jobs[i], &tasks_left, &wait_mutex, &wait_cv]()
		{
			render(ljob); 
			if (--tasks_left == 0) 
			{
				std::unique_lock<std::mutex> lock(wait_mutex);
				wait_cv.notify_one();
			}
		});
	}
	std::unique_lock<std::mutex> lock(wait_mutex);
	wait_cv.wait(lock, [&tasks_left](){ return tasks_left == 0; });
	apply_post_processing(render_total);
	render_total.frame_count++;
}
