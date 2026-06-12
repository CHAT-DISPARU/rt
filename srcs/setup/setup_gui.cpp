/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_gui.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:23:01 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/11 22:49:56 by CHAT-DISPAR      ###   ########.fr       */
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
	ImGui::SliderInt("depth max", &render_total.depth_max, 1.0f, 100.0f);
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
	ImGui::End();
}

void	clean_gui()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}
