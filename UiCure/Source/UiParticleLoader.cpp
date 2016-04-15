
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiParticleLoader.h"
#include "../../Cure/Include/ResourceManager.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/File.h"
#include "../../Lepra/Include/ImageLoader.h"
#include "../../UiTbc/Include/UiBillboardGeometry.h"
#include "../../UiTbc/Include/UiParticleRenderer.h"



namespace UiCure
{



ParticleLoader::ParticleLoader(Cure::ResourceManager* pResourceManager, UiTbc::Renderer* pRenderer, const str& pTextureName, size_t pSubFeatureTextureCount, size_t pSubTotalTextureCount)
{
	File* lFile = pResourceManager->QueryFile(pTextureName);
	if (!lFile)
	{
		mLog.Errorf("Unable to open particle image file '%s'.", pTextureName.c_str());
		deb_assert(false);
		return;
	}
	Canvas lImage;
	ImageLoader lLoader;
	bool lLoadedOk = lLoader.Load(ImageLoader::GetFileTypeFromName(pTextureName), *lFile, lImage);
	delete lFile;
	lFile = 0;
	if (!lLoadedOk)
	{
		mLog.Errorf("Unable to load particle image file '%s'.", pTextureName.c_str());
		deb_assert(false);
		return;
	}

	UiTbc::BillboardGeometry* lBillboardGas = new UiTbc::BillboardGeometry(1, pSubTotalTextureCount);
	UiTbc::Renderer::GeometryID lBillboardGasId = pRenderer->AddGeometry(lBillboardGas, UiTbc::Renderer::MAT_NULL, UiTbc::Renderer::FORCE_NO_SHADOWS);
	UiTbc::BillboardGeometry* lBillboardGlow = new UiTbc::BillboardGeometry(3, pSubTotalTextureCount);
	UiTbc::Renderer::GeometryID lBillboardGlowId = pRenderer->AddGeometry(lBillboardGlow, UiTbc::Renderer::MAT_NULL, UiTbc::Renderer::FORCE_NO_SHADOWS);
	UiTbc::BillboardGeometry* lBillboardShrapnel = new UiTbc::BillboardGeometry(1, 0);
	const float lShrapnelTriStrip[] =
	{
		-0.6f,0,+1.2f,
		+0.6f,0,+1.0f,
		-0.6f,0,-1.2f,
		+0.6f,0,-0.7f,
	};
	lBillboardShrapnel->SetVertexData(lShrapnelTriStrip);
	pRenderer->AddGeometry(lBillboardShrapnel, UiTbc::Renderer::MAT_NULL, UiTbc::Renderer::FORCE_NO_SHADOWS);
	UiTbc::BillboardGeometry* lBillboardSpark = new UiTbc::BillboardGeometry(1, 0);
	const float lSparkTriStrip[] =
	{
		+0.0f,0,+0.3f,
		-0.2f,0,-0.0f,
		+0.2f,0,+0.0f,
		+0.0f,0,-4.0f,
	};
	lBillboardSpark->SetVertexData(lSparkTriStrip);
	pRenderer->AddGeometry(lBillboardSpark, UiTbc::Renderer::MAT_NULL, UiTbc::Renderer::FORCE_NO_SHADOWS);
	UiTbc::Texture* lTexture = new UiTbc::Texture(lImage, Canvas::RESIZE_FAST, -1);
	UiTbc::Renderer::TextureID lTextureId = pRenderer->AddTexture(lTexture);
	delete lTexture;
	pRenderer->TryAddGeometryTexture(lBillboardGasId, lTextureId);
	pRenderer->TryAddGeometryTexture(lBillboardGlowId, lTextureId);

	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)pRenderer->GetDynamicRenderer("particle");
	lParticleRenderer->SetData(pSubFeatureTextureCount, pSubTotalTextureCount, lBillboardGas, lBillboardShrapnel, lBillboardSpark, lBillboardGlow);
}



loginstance(UI_GFX_3D, ParticleLoader);



}
