
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uiparticleloader.h"
#include "../../cure/include/resourcemanager.h"
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/file.h"
#include "../../lepra/include/imageloader.h"
#include "../../uitbc/include/uibillboardgeometry.h"
#include "../../uitbc/include/uiparticlerenderer.h"



namespace UiCure {



ParticleLoader::ParticleLoader(cure::ResourceManager* resource_manager, uitbc::Renderer* renderer, const str& texture_name, size_t sub_feature_texture_count, size_t sub_total_texture_count) {
	File* file = resource_manager->QueryFile(texture_name);
	if (!file) {
		log_.Errorf("Unable to open particle image file '%s'.", texture_name.c_str());
		deb_assert(false);
		return;
	}
	Canvas image;
	ImageLoader loader;
	bool loaded_ok = loader.Load(ImageLoader::GetFileTypeFromName(texture_name), *file, image);
	delete file;
	file = 0;
	if (!loaded_ok) {
		log_.Errorf("Unable to load particle image file '%s'.", texture_name.c_str());
		deb_assert(false);
		return;
	}

	uitbc::BillboardGeometry* billboard_gas = new uitbc::BillboardGeometry(1, sub_total_texture_count);
	uitbc::Renderer::GeometryID billboard_gas_id = renderer->AddGeometry(billboard_gas, uitbc::Renderer::kMatNull, uitbc::Renderer::kForceNoShadows);
	uitbc::BillboardGeometry* billboard_glow = new uitbc::BillboardGeometry(3, sub_total_texture_count);
	uitbc::Renderer::GeometryID billboard_glow_id = renderer->AddGeometry(billboard_glow, uitbc::Renderer::kMatNull, uitbc::Renderer::kForceNoShadows);
	uitbc::BillboardGeometry* billboard_shrapnel = new uitbc::BillboardGeometry(1, 0);
	const float shrapnel_tri_strip[] =
	{
		-0.6f,0,+1.2f,
		+0.6f,0,+1.0f,
		-0.6f,0,-1.2f,
		+0.6f,0,-0.7f,
	};
	billboard_shrapnel->SetVertexData(shrapnel_tri_strip);
	renderer->AddGeometry(billboard_shrapnel, uitbc::Renderer::kMatNull, uitbc::Renderer::kForceNoShadows);
	uitbc::BillboardGeometry* billboard_spark = new uitbc::BillboardGeometry(1, 0);
	const float spark_tri_strip[] =
	{
		+0.0f,0,+0.3f,
		-0.2f,0,-0.0f,
		+0.2f,0,+0.0f,
		+0.0f,0,-4.0f,
	};
	billboard_spark->SetVertexData(spark_tri_strip);
	renderer->AddGeometry(billboard_spark, uitbc::Renderer::kMatNull, uitbc::Renderer::kForceNoShadows);
	uitbc::Texture* texture = new uitbc::Texture(image, Canvas::kResizeFast, -1);
	uitbc::Renderer::TextureID texture_id = renderer->AddTexture(texture);
	delete texture;
	renderer->TryAddGeometryTexture(billboard_gas_id, texture_id);
	renderer->TryAddGeometryTexture(billboard_glow_id, texture_id);

	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)renderer->GetDynamicRenderer("particle");
	particle_renderer->SetData(sub_feature_texture_count, sub_total_texture_count, billboard_gas, billboard_shrapnel, billboard_spark, billboard_glow);
}



loginstance(kUiGfx3D, ParticleLoader);



}
