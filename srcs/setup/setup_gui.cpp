/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_gui.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:23:01 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/20 13:57:34 by CHAT-DISPAR      ###   ########.fr       */
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
	ImGui::SetNextWindowSize(ImVec2(280, io.DisplaySize.y), ImGuiCond_Always);
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
	ImGui::SeparatorText("Perf");
	ImGui::Text("%.2f ms / frame", ms);
	ImGui::SameLine();
	ImGui::TextDisabled("  |  %.1f FPS", fps);
	char	overlay[32];
	snprintf(overlay, sizeof(overlay), "%zu sample", render_total.frame_count);
	ImGui::PlotLines("##fps", fps_history, 100, fps_offset, overlay, 0.0f, 240.0f, ImVec2(-1, 60));
	ImGui::SeparatorText("Res");
	if (ImGui::BeginCombo("##res", res_labels[res_current]))
	{
		for (int i = 0; i < res_count; i++)
		{
			bool	selected = (i == res_current);

			if (ImGui::Selectable(res_labels[i], selected))
			{
				res_current = i;
				resize_sdl(sdl, res_w[i], res_h[i], render_total);
			}
			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::Text("Current: %d x %d", sdl.width, sdl.height);
	
	ImGui::SeparatorText("Cam");
	Vec3f	pos(app.camera.get_pos());
	ImGui::Text("Pos  %.1f  %.1f  %.1f", pos._x, pos._y, pos._z);
	ImGui::SliderFloat("speed cam", &render_total.cam_speed, 0.1f, 1.0f);
	ImGui::SliderFloat("speed rot", &render_total.cam_rotate, 0.01f, 0.5f);

	float	fov = app.camera.get_fov();
	if (ImGui::SliderFloat("fov", &fov, 1.0f, 180.0f))
		app.camera.set_fov(fov);

	ImGui::SeparatorText("Param");
	if (ImGui::Checkbox("add sun", &render_total.sun_light.enabled))
		render_total.frame_count = 1;
	if (ImGui::Checkbox("add black hole", &render_total.black_hole_enabled))
    	render_total.frame_count = 1;
	if (ImGui::Checkbox("add shadow rays", &render_total.shadow_ray))
		render_total.frame_count = 1;
	ImGui::SliderInt("depth max", &render_total.depth_max, 1, 100);
	const char*	sample_labels[] = {"1", "2", "4", "8", "16", "32", "64", "128", "inf"};
	const int	sample_values[] = {1, 2, 4, 8, 16, 32, 64, 128, -1};
	const int	sample_count = 9;
	int sample_idx = 0;

	for (int i = 0; i < sample_count; i++)
	{
		if (sample_values[i] == render_total.samples)
		{
			sample_idx = i;
			break;
		}
	}
	if (render_total.bvh_debug.mode != BvhDebugMode::OFF)
		ImGui::BeginDisabled();

	if (ImGui::BeginCombo("samples", sample_labels[sample_idx]))
	{
		for (int i = 0; i < sample_count; i++)
		{
			bool selected = (i == sample_idx);
			if (ImGui::Selectable(sample_labels[i], selected))
				render_total.samples = sample_values[i];
			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	if (render_total.bvh_debug.mode != BvhDebugMode::OFF)
		ImGui::EndDisabled();
	ImGui::SeparatorText("BVH Debug");

	const char*	bvh_modes[] = {"Off", "Heatmap", "Depth Slice"};
	int	bvh_mode_idx = (int)render_total.bvh_debug.mode;
	if (ImGui::Combo("mode##bvh", &bvh_mode_idx, bvh_modes, 3))
	{
		BvhDebugMode new_mode = (BvhDebugMode)bvh_mode_idx;

		//activation
		if (new_mode != BvhDebugMode::OFF &&
			render_total.bvh_debug.mode == BvhDebugMode::OFF)
		{
			render_total.bvh_debug.saved_samples = render_total.samples;
			render_total.samples = 1;
		}
		// desactivation
		else if (new_mode == BvhDebugMode::OFF &&
				render_total.bvh_debug.mode != BvhDebugMode::OFF)
		{
			if (render_total.bvh_debug.saved_samples != -1)
				render_total.samples = render_total.bvh_debug.saved_samples;
			render_total.bvh_debug.saved_samples = -1;
		}
		render_total.bvh_debug.mode = new_mode;
		render_total.frame_count = 1;
	}
	if (render_total.bvh_debug.mode == BvhDebugMode::HEATMAP)
	{
		if (ImGui::SliderInt("max tests", &render_total.bvh_debug.max_tests, 10, 1000))
			render_total.frame_count = 1;
		ImGui::TextDisabled("bleu = ok  rouge = skibidi");
	}
	if (render_total.bvh_debug.mode == BvhDebugMode::DEPTH_SLICE)
	{
		bool	changed = false;
		changed |= ImGui::SliderInt("bvh depth min", &render_total.bvh_debug.min_depth,
								0, render_total.bvh_debug.tree_depth);
		changed |= ImGui::SliderInt("bvh depth max", &render_total.bvh_debug.max_depth,
									render_total.bvh_debug.min_depth,
									render_total.bvh_debug.tree_depth);
		if (changed)
			render_total.frame_count = 1;
		ImGui::TextDisabled("rouge=0 orange=1 jaune=2 vert=3");
		ImGui::TextDisabled("cyan=4 bleu=5 violet=6 rose=7");
	}
	ImGui::End();
}

void	clean_gui()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}
