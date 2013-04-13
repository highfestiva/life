
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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
		mLog.Errorf(_T("Unable to open particle image file '%s'."), pTextureName.c_str());
		assert(false);
		return;
	}
	Canvas lImage;
	ImageLoader lLoader;
	bool lLoadedOk = lLoader.Load(ImageLoader::GetFileTypeFromName(pTextureName), *lFile, lImage);
	delete lFile;
	lFile = 0;
	if (!lLoadedOk)
	{
		mLog.Errorf(_T("Unable to load particle image file '%s'."), pTextureName.c_str());
		assert(false);
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
		-0.4f,0,-0.0f,
		+0.4f,0,+0.0f,
		+0.0f,0,-4.5f,
	};
	lBillboardSpark->SetVertexData(lSparkTriStrip);
	pRenderer->AddGeometry(lBillboardSpark, UiTbc::Renderer::MAT_NULL, UiTbc::Renderer::FORCE_NO_SHADOWS);
	UiTbc::Texture* lTexture = new UiTbc::Texture(lImage, Canvas::RESIZE_FAST, -1);
	UiTbc::Renderer::TextureID lTextureId = pRenderer->AddTexture(lTexture);
	pRenderer->TryAddGeometryTexture(lBillboardGasId, lTextureId);
	pRenderer->TryAddGeometryTexture(lBillboardGlowId, lTextureId);

	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)pRenderer->GetDynamicRenderer(_T("particle"));
	lParticleRenderer->SetData(pSubFeatureTextureCount, pSubTotalTextureCount, lBillboardGas, lBillboardShrapnel, lBillboardSpark, lBillboardGlow);
}



LOG_CLASS_DEFINE(UI_GFX_3D, ParticleLoader);



}
