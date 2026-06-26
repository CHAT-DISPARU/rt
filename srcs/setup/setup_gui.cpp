/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_gui.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:23:01 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/25 20:57:02 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.hpp"

void	show_settings_window(SDLContext &sdl, Render &render_total, AppContext &app,
	const char **res_labels, int *res_w, int *res_h, int res_count,
	int &res_current, float fps, float ms)
{
	static float	fps_history[100] = {};
	static int		fps_offset = 0;

	fps_history[fps_offset] = fps;
	fps_offset = (fps_offset + 1) % 90;
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(320, io.DisplaySize.y), ImGuiCond_Always);
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	if (!ImGui::Begin("Settings  [H]", nullptr, flags))
	{
		ImGui::End();
		return;
	}

	//stat
	ImGui::Text("%.2f ms/frame  |  %.1f FPS", ms, fps);
	char	overlay[32];
	snprintf(overlay, sizeof(overlay), "%zu samples", render_total.frame_count);
	ImGui::PlotLines("##fps", fps_history, 100, fps_offset, overlay, 0.0f, 240.0f, ImVec2(-1, 50));
	
	// res
	if (ImGui::BeginCombo("Resolution", res_labels[res_current]))
	{
		for (int i = 0; i < res_count; i++)
		{
			bool	selected = (i == res_current);
			if (ImGui::Selectable(res_labels[i], selected))
			{
				res_current = i;
				resize_sdl(sdl, res_w[i], res_h[i], render_total);
			}
			if (selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::Spacing();


	//rendu
	if (ImGui::CollapsingHeader("Renderer Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox("HDRI Env", &render_total.hdri))
			render_total.frame_count = 1;
		
		ImGui::SameLine();
		if (ImGui::Checkbox("Roulette Russe", &render_total.ru_enabled))
			render_total.frame_count = 1;
		if (ImGui::Checkbox("Shadow Rays", &render_total.shadow_ray))
			render_total.frame_count = 1;

		if (ImGui::Checkbox("Black Hole", &render_total.black_hole_enabled))
			render_total.frame_count = 1;

		ImGui::SliderInt("Max Depth", &render_total.depth_max, 1, 100);

		// Samples Combo
		const char* sample_labels[] = {"1", "2", "4", "8", "16", "32", "64", "128", "inf"};
		const int	sample_values[] = {1, 2, 4, 8, 16, 32, 64, 128, -1};
		int sample_idx = 0;
		for (int i = 0; i < 9; i++) {
			if (sample_values[i] == render_total.samples) { sample_idx = i; break; }
		}

		if (render_total.bvh_debug.mode != BvhDebugMode::OFF) ImGui::BeginDisabled();
		if (ImGui::BeginCombo("Max Samples", sample_labels[sample_idx]))
		{
			for (int i = 0; i < 9; i++) {
				bool selected = (i == sample_idx);
				if (ImGui::Selectable(sample_labels[i], selected))
					render_total.samples = sample_values[i];
				if (selected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (render_total.bvh_debug.mode != BvhDebugMode::OFF) ImGui::EndDisabled();
	}


	//cam
	if (ImGui::CollapsingHeader("Camera & Focus"))
	{
		Vec3f	pos = app.camera.get_pos();
		Vec3f	dir = app.camera.get_forward();
		ImGui::TextDisabled("Pos: %.1f, %.1f, %.1f", pos._x, pos._y, pos._z);
		ImGui::TextDisabled("Dir: %.2f, %.2f, %.2f", dir._x, dir._y, dir._z);
		
		ImGui::SliderFloat("Move Speed", &render_total.cam_speed, 0.1f, 1.0f);
		ImGui::SliderFloat("Look Speed", &render_total.cam_rotate, 0.01f, 0.5f);
		
		float	fov = app.camera.get_fov();
		if (ImGui::SliderFloat("FOV", &fov, 1.0f, 180.0f))
			app.camera.set_fov(fov);

		ImGui::Spacing();
		ImGui::Text("Depth of Field");
		float	lens_radius = app.camera.get_lens_radius();
		float	focus_dist = app.camera.get_focus_dist();
		bool	dof_changed = false;

		dof_changed |= ImGui::SliderFloat("Lens Radius", &lens_radius, 0.0f, 1.0f, "%.3f");
		dof_changed |= ImGui::SliderFloat("Focus Dist", &focus_dist, 0.1f, 50.0f, "%.1f m");
		if (dof_changed)
		{
			app.camera.set_dof(lens_radius, focus_dist);
			render_total.frame_count = 1;
		}
	}


	// bloom post prcess
	if (ImGui::CollapsingHeader("Post-Processing", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enable Bloom", &render_total.bloom_enabled);
		if (render_total.bloom_enabled)
		{
			ImGui::SliderFloat("Threshold", &render_total.bloom_threshold, 0.5f, 5.0f, "%.2f");
			ImGui::SliderFloat("Intensity", &render_total.bloom_intensity, 0.0f, 2.0f, "%.2f");
			ImGui::SliderInt("Blur Radius", &render_total.blur_radius, 1, 50);
		}
		ImGui::TextDisabled("Tone Mapping: Reinhard");
	}


	//debug bvh
	if (ImGui::CollapsingHeader("BVH Debug"))
	{
		const char* bvh_modes[] = {"Off", "Heatmap", "Depth Slice"};
		int bvh_mode_idx = (int)render_total.bvh_debug.mode;
		
		if (ImGui::Combo("Mode", &bvh_mode_idx, bvh_modes, 3))
		{
			BvhDebugMode new_mode = (BvhDebugMode)bvh_mode_idx;
			if (new_mode != BvhDebugMode::OFF && render_total.bvh_debug.mode == BvhDebugMode::OFF)
			{
				//render_total.bvh_debug.saved_samples = render_total.samples;
				render_total.samples = 1;
			}
			// else if (new_mode == BvhDebugMode::OFF && render_total.bvh_debug.mode != BvhDebugMode::OFF)
			// {
			// 	//if (render_total.bvh_debug.saved_samples != -1)
			// 		render_total.samples = render_total.bvh_debug.saved_samples;
			// 	//render_total.bvh_debug.saved_samples = -1;
			// }
			render_total.bvh_debug.mode = new_mode;
			render_total.frame_count = 1;
		}

		if (render_total.bvh_debug.mode == BvhDebugMode::HEATMAP)
		{
			if (ImGui::SliderInt("Max Tests", &render_total.bvh_debug.max_tests, 10, 1000))
				render_total.frame_count = 1;
			ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "Blue = Fast");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Red = Slow");
		}
		else if (render_total.bvh_debug.mode == BvhDebugMode::DEPTH_SLICE)
		{
			bool	changed = false;
			changed |= ImGui::SliderInt("Depth Min", &render_total.bvh_debug.min_depth, 0, render_total.bvh_debug.tree_depth);
			changed |= ImGui::SliderInt("Depth Max", &render_total.bvh_debug.max_depth, render_total.bvh_debug.min_depth, render_total.bvh_debug.tree_depth);
			if (changed) render_total.frame_count = 1;
		}
	}

	ImGui::End();
}

void	clean_gui()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}
