#ifndef CURE_TEST_WITHOUT_UI

#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../Lepra/Include/Timer.h"
#include "../../Lepra/Include/Thread.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../../TBC/Include/PhysicsManagerFactory.h"
#include "../../UiLepra/Include/UiCore.h"
#include "../../UiLepra/Include/UiDisplayManager.h"
#include "../../UiLepra/Include/UiInput.h"
#include "../../UiLepra/Include/UiLepra.h"
#include "../../UiTBC/Include/UiBasicMeshCreator.h"
#include "../../UiTBC/Include/UiGeometryBatch.h"
#include "../../UiTBC/Include/UiOpenGLRenderer.h"
//#include "../../UiTBC/Include/UiTbc.h"
#include "../../UiTBC/Include/UiTriangleBasedGeometry.h"

typedef Lepra::Vector3DF vec3;
typedef Lepra::Random rnd;

class Object
{
public:
	enum Type
	{
		BOX = 0,
		SPHERE,

		TYPE_COUNT
	};

	Object(UiTbc::Renderer* pRenderer, 
	       TBC::PhysicsManager* pPhysics) :
		mType(BOX),
		mBodyType(TBC::PhysicsManager::STATIC),
		mGfxGeom(0),
		mGeomID(UiTbc::Renderer::INVALID_GEOMETRY),
		mBodyID(TBC::INVALID_BODY),
		mRenderer(pRenderer),
		mPhysics(pPhysics)
	{
	}

	Object(UiTbc::Renderer* pRenderer, 
	       TBC::PhysicsManager* pPhysics, 
	       TBC::PhysicsManager::BodyType pBodyType) :
		mType(BOX),
		mBodyType(pBodyType),
		mGfxGeom(0),
		mGeomID(UiTbc::Renderer::INVALID_GEOMETRY),
		mBodyID(TBC::INVALID_BODY),
		mRenderer(pRenderer),
		mPhysics(pPhysics)
	{
	}

	~Object()
	{
		mRenderer->RemoveGeometry(mGeomID);
		mPhysics->DeleteBody(mBodyID);
		delete mGfxGeom;
	}

	void MakeBox(const Lepra::Vector3DF& pSize)
	{
		Lepra::Vector3DF lSize(pSize);
		mType = BOX;
		if(lSize.x <= 0)
			lSize.x = (float)Lepra::Random::Uniform(0, -lSize.x);
		if(lSize.y <= 0)
			lSize.y = (float)Lepra::Random::Uniform(0, -lSize.y);
		if(lSize.z <= 0)
			lSize.z = (float)Lepra::Random::Uniform(0, -lSize.z);

		float lVolume = lSize.x * lSize.y * lSize.z;
		// Need to be careful about what is "width", "height" and "depth".
		mGfxGeom = UiTbc::BasicMeshCreator::CreateFlatBox(lSize.x, lSize.z, lSize.y);
		mBodyID = mPhysics->CreateBox(true, mTransform, lVolume, lSize, mBodyType);
		mGeomID = mRenderer->AddGeometry(mGfxGeom, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID_PXS, UiTbc::Renderer::CAST_SHADOWS);

		TBC::GeometryBase::BasicMaterialSettings lMat(
			vec3((float)rnd::Uniform(0.0, 0.3), (float)rnd::Uniform(0.0, 0.3), (float)rnd::Uniform(0.0, 0.3)),
			vec3((float)rnd::Uniform(0.5, 1.0), (float)rnd::Uniform(0.5, 1.0), (float)rnd::Uniform(0.5, 1.0)),
			vec3((float)rnd::Uniform(0.0, 1.0), (float)rnd::Uniform(0.0, 1.0), (float)rnd::Uniform(0.0, 1.0)),
			(float)rnd::Uniform(0.0, 0.5), 1.0f, true);
		mGfxGeom->SetBasicMaterialSettings(lMat);
		UpdateTransformation();
		mGfxGeom->SetAlwaysVisible(true);
	}

	void MakeSphere(float pRadius)
	{
		mType = SPHERE;
		if(pRadius <= 0)
			pRadius = (float)Lepra::Random::Uniform(0, -pRadius);

		float lVolume = pRadius * pRadius * pRadius * Lepra::PIF * 4.0f / 3.0f;
		mGfxGeom = UiTbc::BasicMeshCreator::CreateEllipsoid(pRadius, pRadius, pRadius, 16, 16);
		mBodyID = mPhysics->CreateSphere(true, mTransform, lVolume, pRadius, mBodyType);
		mGeomID = mRenderer->AddGeometry(mGfxGeom, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID_PXS, UiTbc::Renderer::CAST_SHADOWS);

		TBC::GeometryBase::BasicMaterialSettings lMat(
			vec3((float)rnd::Uniform(0.0, 0.3), (float)rnd::Uniform(0.0, 0.3), (float)rnd::Uniform(0.0, 0.3)),
			vec3((float)rnd::Uniform(0.5, 1.0), (float)rnd::Uniform(0.5, 1.0), (float)rnd::Uniform(0.5, 1.0)),
			vec3((float)rnd::Uniform(0.0, 1.0), (float)rnd::Uniform(0.0, 1.0), (float)rnd::Uniform(0.0, 1.0)),
			(float)rnd::Uniform(0.0, 0.5), 1.0f, true);
		mGfxGeom->SetBasicMaterialSettings(lMat);
		UpdateTransformation();
		mGfxGeom->SetAlwaysVisible(true);
	}

	void SetRandomPos(const Lepra::Vector3DF& pMin, const Lepra::Vector3DF& pMax)
	{
		Lepra::Vector3DF lPos((float)rnd::Uniform(pMin.x, pMax.x),
		                      (float)rnd::Uniform(pMin.y, pMax.y),
				      (float)rnd::Uniform(pMin.z, pMax.z));
		mTransform.SetPosition(lPos);
		mPhysics->SetBodyTransform(mBodyID, mTransform);
		UpdateTransformation();
	}

	void SetRandomRot()
	{
		Lepra::Vector3DF lRotAxis((float)rnd::Uniform(-1.0f, 1.0f),
		                          (float)rnd::Uniform(-1.0f, 1.0f),
					  (float)rnd::Uniform(-1.0f, 1.0f));
		lRotAxis.Normalize();
		float lAngle = (float)rnd::Uniform(0, 2.0f * Lepra::PIF);
		mTransform.SetOrientation(Lepra::QuaternionF(lAngle, lRotAxis));
		mPhysics->SetBodyTransform(mBodyID, mTransform);
		UpdateTransformation();
	}

	void SetPos(const Lepra::Vector3DF& pPos)
	{
		mTransform.SetPosition(pPos);
		mPhysics->SetBodyTransform(mBodyID, mTransform);
		UpdateTransformation();
	}

	void SetRot(const Lepra::QuaternionF& pRot)
	{
		mTransform.SetOrientation(pRot);
		mPhysics->SetBodyTransform(mBodyID, mTransform);
		UpdateTransformation();
	}

	void UpdateTransformation()
	{
		mPhysics->GetBodyTransform(mBodyID, mTransform);
		mGfxGeom->SetTransformation(mTransform);
	}

	Type mType;
	TBC::PhysicsManager::BodyType mBodyType;
	TBC::GeometryBase* mGfxGeom;
	UiTbc::Renderer::GeometryID mGeomID;
	TBC::PhysicsManager::BodyID mBodyID;

	UiTbc::Renderer* mRenderer;
	TBC::PhysicsManager* mPhysics;

	Lepra::TransformationF mTransform;

	float mTTL; // Time to live.
};

void RunRotationalAgreementTest()
{
	UiLepra::DisplayManager* lDisp = UiLepra::DisplayManager::CreateDisplayManager(UiLepra::DisplayManager::OPENGL_CONTEXT);
	UiLepra::DisplayMode lMode;
	bool lOk = lDisp->FindDisplayMode(lMode, 800, 600);
	if(lOk)
		lOk = lDisp->OpenScreen(lMode, UiLepra::DisplayManager::WINDOWED);

	Lepra::Canvas lScreen;
	lDisp->GetScreenCanvas(lScreen);
	UiTbc::OpenGLRenderer lRenderer(&lScreen);
	TBC::PhysicsManager* lPhysics = TBC::PhysicsManagerFactory::Create(TBC::PhysicsManagerFactory::ENGINE_ODE);

	lRenderer.SetShadowMode(UiTbc::Renderer::CAST_SHADOWS, UiTbc::Renderer::SH_VOLUMES_AND_MAPS);
	lPhysics->SetGravity(Lepra::Vector3DF(0, 0, -9.82f));

	enum
	{
		DIM = 1,
		STATIC_COUNT = DIM*DIM + 1,
		DYNAMIC_COUNT = 10,
	};

	Object* lStaticObj[STATIC_COUNT];
	Object* lDynamicObj[DYNAMIC_COUNT];

	// Create the big ground box.
	lStaticObj[0] = new Object(&lRenderer, lPhysics);
	lStaticObj[0]->MakeBox(Lepra::Vector3DF(100.0f, 100.0f, 100.0f));
	lStaticObj[0]->SetPos(Lepra::Vector3DF(0, 0, -50.0f));
	int x;
	int y;
	int i;
	for(y = 0; y < DIM; y++)
	{
		for(x = 0; x < DIM; x++)
		{
			i = y * DIM + x + 1;
			lStaticObj[i] = new Object(&lRenderer, lPhysics, TBC::PhysicsManager::STATIC);
			//lStaticObj[i]->MakeBox(Lepra::Vector3DF(3.0f / DIM, 10.0f / DIM, 0.1f));
			//lStaticObj[i]->SetPos(Lepra::Vector3DF((x+0.5f - 0.5f * DIM) * 10.0f / DIM, (y+0.5f - 0.5f * DIM) * 10.0f / DIM, 5.0f));

			// Try rotating using axis & angle.
			//Lepra::Vector3DF lAxis(1, 0, 0);
			//lAxis.Normalize();
			//lStaticObj[i]->SetRot(Lepra::QuaternionF(Lepra::PIF / 4.0f, lAxis));

			// Random axis & angle.
			//lStaticObj[i]->SetRandomRot();

			// Rotation using help functions.
			//Lepra::QuaternionF lRot;
			//lRot.RotateAroundWorldZ(Lepra::PIF / 2.0f);
			//lRot.RotateAroundOwnX(-Lepra::PIF / 4.0f);
			//lRot.RotateAroundOwnY(Lepra::PIF / 4.0f);
			//lRot.RotateAroundOwnZ(Lepra::PIF / 4.0f);
			//lStaticObj[i]->SetRot(lRot);

			// Jonte's version that shows up wrong (rotated 90 degrees around own X-axis).
			// Create physics first.
			static const float lRoadWidth = 4;
			static const float lRoadHeight = 0.1f;
			static const float lUphillLength = 8;
			Lepra::Vector3DF lDimensions;
			Lepra::TransformationF lTransformation;
			//Lepra::RotationMatrixF lRotation;
			Lepra::QuaternionF lRotation;
			lDimensions.Set(lRoadWidth, lUphillLength, lRoadHeight);
			lTransformation.SetPosition(Lepra::Vector3DF(0, 0, 5));
			lRotation.RotateAroundOwnX(Lepra::PIF/4);
			//lTransformation.SetOrientation(lRotation);
			lTransformation.RotatePitch(Lepra::PIF/8);
			lStaticObj[i]->mBodyID = lPhysics->CreateBox(true, lTransformation, 0, lDimensions, TBC::PhysicsManager::STATIC, 0.5f, 1.0f, 0);

			// Then graphics.
			TBC::GeometryBase::BasicMaterialSettings lMaterial(
				vec3(0, 0, 0),
				vec3(0.85f, 0.85f, 0.85f),
				vec3(0.5f, 0.5f, 0.5f),
				0.1f, 1.0f, true);
			UiTbc::Renderer::MaterialType lMaterialType = UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID;
			UiTbc::Renderer::TextureID lTextureId = UiTbc::Renderer::INVALID_TEXTURE;
			UiTbc::Renderer::Shadows lShadow = UiTbc::Renderer::CAST_SHADOWS;

			// Bug number 1, CreateFlatBox() is not intuitive. Swap z and y.
			lStaticObj[i]->mGfxGeom = UiTbc::BasicMeshCreator::CreateFlatBox(lDimensions.x, lDimensions.y, lDimensions.z);
			lStaticObj[i]->mGfxGeom->SetAlwaysVisible(true);
			lStaticObj[i]->mGfxGeom->SetBasicMaterialSettings(lMaterial);
			
			// Not needed. The bug is there anyway.
			//lPhysics->GetBodyTransform(lStaticObj[i]->mBodyID, lTransformation);
			
			// Uncomment the below to display the correctly rotated geometry.
			//lTransformation.GetOrientation().RotateAroundOwnX(Lepra::PIF/2);
			lStaticObj[i]->mGfxGeom->SetTransformation(lTransformation);
			// Not setting the below causes a crash (feature?) within Renderer to be triggered.
			//lStaticObj[i]->mGeomID =
			UiTbc::Renderer::GeometryID lGraphicId = lRenderer.AddGeometry(lStaticObj[i]->mGfxGeom, lMaterialType, lShadow);
			lRenderer.TryAddGeometryTexture(lGraphicId, lTextureId);
		}
	}

	for(i = 0; i < DYNAMIC_COUNT; i++)
	{
		lDynamicObj[i] = new Object(&lRenderer, lPhysics, TBC::PhysicsManager::DYNAMIC);
		//switch((Object::Type)(rnd::GetRandomNumber() % Object::TYPE_COUNT))
		//{
		//case Object::BOX: lDynamicObj[i]->MakeBox(Lepra::Vector3DF((float)rnd::Uniform(0.1f, 0.9f), (float)rnd::Uniform(0.1f, 0.9f), (float)rnd::Uniform(0.1f, 0.9f))); break;
		//case Object::SPHERE: lDynamicObj[i]->MakeSphere((float)rnd::Uniform(0.1f, 0.9f)); break;
		//}
		lDynamicObj[i]->MakeSphere((float)rnd::Uniform(0.2f, 0.4f));

		lDynamicObj[i]->SetRandomPos(Lepra::Vector3DF(-5.0f, -5.0f, 8.0f), Lepra::Vector3DF(5.0f, 5.0f, 8.0f));
		lDynamicObj[i]->mTTL = (float)i;
	}

	Lepra::TransformationF lCamTransform;
	lCamTransform.MoveRight(7.0f);
	lCamTransform.MoveUp(10.0f);
	lCamTransform.RotateWorldX(-Lepra::PIF / 4.0f);
	lCamTransform.RotateWorldZ(Lepra::PIF / 2.0f);
	lRenderer.SetCameraTransformation(lCamTransform);

	lRenderer.SetLightsEnabled(true);
	lRenderer.AddDirectionalLight(UiTbc::Renderer::LIGHT_STATIC, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f, 1.0f, 40.0f);
	lRenderer.SetAmbientLight(0.5f, 0.5f, 0.5f);

	lRenderer.SetViewFrustum(90.0f, 0.1f, 1000.0f);

	if(lOk)
	{
		Lepra::SystemManager::AddQuitRequest(-1);
		Lepra::Timer lTotalTimer;
		Lepra::Timer lDeltaTimer;
		i = 0;
		lRenderer.SetClearColor(Lepra::DARK_BLUE);
		while(!Lepra::SystemManager::GetQuitRequest() && lTotalTimer.GetTimeDiffF() < 60.0f)
		{
			for(i = 0; i < DYNAMIC_COUNT; i++)
			{
				lDynamicObj[i]->UpdateTransformation();
				lDynamicObj[i]->mTTL -= (float)lDeltaTimer.GetTimeDiffF();
				if(lDynamicObj[i]->mTTL <= 0.0f)
				{
					lDynamicObj[i]->mTTL = (float)DYNAMIC_COUNT;
					lDynamicObj[i]->SetRandomPos(Lepra::Vector3DF(-5.0f, -5.0f, 8.0f), Lepra::Vector3DF(5.0f, 5.0f, 8.0f));
				}
			}

			lDeltaTimer.ClearTimeDiff();

			lScreen.SetBuffer(0);
			lRenderer.ResetClippingRect();
			lRenderer.Clear();
			lRenderer.RenderScene();
			lPhysics->StepAccurate(0.05f);
			lDisp->UpdateScreen();

			lDeltaTimer.UpdateTimer();
			while(lDeltaTimer.GetTimeDiffF() < 0.05f)
			{
				Lepra::Thread::YieldCpu();
				lDeltaTimer.UpdateTimer();
			}

			lTotalTimer.UpdateTimer();
			i++;

			UiLepra::Core::ProcessMessages();
		}
	}

	for(i = 0; i < STATIC_COUNT; i++)
	{
		delete lStaticObj[i];
	}
	for(i = 0; i < DYNAMIC_COUNT; i++)
	{
		delete lDynamicObj[i];
	}

	delete (lPhysics);
}

#endif // !CURE_TEST_WITHOUT_UI
