#include "pch.h"
#ifndef CURE_TEST_WITHOUT_UI
#include "../../lepra/include/math.h"
#include "../../lepra/include/random.h"
#include "../../lepra/include/systemmanager.h"
#include "../../lepra/include/timer.h"
#include "../../lepra/include/thread.h"
#include "../../lepra/include/transformation.h"
#include "../../tbc/include/physicsmanager.h"
#include "../../tbc/include/physicsmanagerfactory.h"
#include "../../uilepra/include/uicore.h"
#include "../../uilepra/include/uidisplaymanager.h"
#include "../../uilepra/include/uiinput.h"
#include "../../uilepra/include/uilepra.h"
#include "../../uitbc/include/uibasicmeshcreator.h"
#include "../../uitbc/include/uigeometrybatch.h"
#include "../../uitbc/include/uiopenglrenderer.h"
//#include "../../uitbc/include/uitbc.h"
#include "../../uitbc/include/uitrianglebasedgeometry.h"

typedef lepra::vec3 vec3;
typedef lepra::Random rnd;

class Object {
public:
	enum Type {
		kBox = 0,
		kSphere,

		kTypeCount
	};

	Object(uitbc::Renderer* renderer,
	       tbc::PhysicsManager* physics) :
		type_(kBox),
		body_type_(tbc::PhysicsManager::kStatic),
		gfx_geom_(0),
		geom_id_(uitbc::Renderer::INVALID_GEOMETRY),
		body_id_(tbc::INVALID_BODY),
		renderer_(renderer),
		physics_(physics) {
	}

	Object(uitbc::Renderer* renderer,
	       tbc::PhysicsManager* physics,
	       tbc::PhysicsManager::BodyType body_type) :
		type_(kBox),
		body_type_(body_type),
		gfx_geom_(0),
		geom_id_(uitbc::Renderer::INVALID_GEOMETRY),
		body_id_(tbc::INVALID_BODY),
		renderer_(renderer),
		physics_(physics) {
	}

	~Object() {
		renderer_->RemoveGeometry(geom_id_);
		physics_->DeleteBody(body_id_);
		delete gfx_geom_;
	}

	void MakeBox(const lepra::vec3& size) {
		lepra::vec3 _size(size);
		type_ = kBox;
		if(_size.x <= 0)
			_size.x = lepra::Random::Uniform(0.0f, -_size.x);
		if(_size.y <= 0)
			_size.y = lepra::Random::Uniform(0.0f, -_size.y);
		if(_size.z <= 0)
			_size.z = lepra::Random::Uniform(0.0f, -_size.z);

		float volume = _size.x * _size.y * _size.z;
		// Need to be careful about what is "width", "height" and "depth".
		gfx_geom_ = uitbc::BasicMeshCreator::CreateFlatBox(_size.x, _size.z, _size.y);
		body_id_ = physics_->CreateBox(true, transform_, volume, _size, body_type_);
		geom_id_ = renderer_->AddGeometry(gfx_geom_, uitbc::Renderer::kMatSingleColorSolidPxs, uitbc::Renderer::kCastShadows);

		tbc::GeometryBase::BasicMaterialSettings mat(
			vec3((float)rnd::Uniform(0.0, 0.3), (float)rnd::Uniform(0.0, 0.3), (float)rnd::Uniform(0.0, 0.3)),
			vec3((float)rnd::Uniform(0.5, 1.0), (float)rnd::Uniform(0.5, 1.0), (float)rnd::Uniform(0.5, 1.0)),
			vec3((float)rnd::Uniform(0.0, 1.0), (float)rnd::Uniform(0.0, 1.0), (float)rnd::Uniform(0.0, 1.0)),
			(float)rnd::Uniform(0.0, 0.5), 1.0f, true);
		gfx_geom_->SetBasicMaterialSettings(mat);
		UpdateTransformation();
		gfx_geom_->SetAlwaysVisible(true);
	}

	void MakeSphere(float radius) {
		type_ = kSphere;
		if(radius <= 0)
			radius = lepra::Random::Uniform(0.0f, -radius);

		float volume = radius * radius * radius * lepra::PIF * 4.0f / 3.0f;
		gfx_geom_ = uitbc::BasicMeshCreator::CreateEllipsoid(radius, radius, radius, 16, 16);
		body_id_ = physics_->CreateSphere(true, transform_, volume, radius, body_type_);
		geom_id_ = renderer_->AddGeometry(gfx_geom_, uitbc::Renderer::kMatSingleColorSolidPxs, uitbc::Renderer::kCastShadows);

		tbc::GeometryBase::BasicMaterialSettings mat(
			vec3((float)rnd::Uniform(0.0, 0.3), (float)rnd::Uniform(0.0, 0.3), (float)rnd::Uniform(0.0, 0.3)),
			vec3((float)rnd::Uniform(0.5, 1.0), (float)rnd::Uniform(0.5, 1.0), (float)rnd::Uniform(0.5, 1.0)),
			vec3((float)rnd::Uniform(0.0, 1.0), (float)rnd::Uniform(0.0, 1.0), (float)rnd::Uniform(0.0, 1.0)),
			(float)rnd::Uniform(0.0, 0.5), 1.0f, true);
		gfx_geom_->SetBasicMaterialSettings(mat);
		UpdateTransformation();
		gfx_geom_->SetAlwaysVisible(true);
	}

	void SetRandomPos(const lepra::vec3& min, const lepra::vec3& max) {
		lepra::vec3 _pos((float)rnd::Uniform(min.x, max.x),
		                      (float)rnd::Uniform(min.y, max.y),
				      (float)rnd::Uniform(min.z, max.z));
		transform_.SetPosition(_pos);
		physics_->SetBodyTransform(body_id_, transform_);
		UpdateTransformation();
	}

	void SetRandomRot() {
		lepra::vec3 rot_axis((float)rnd::Uniform(-1.0f, 1.0f),
		                          (float)rnd::Uniform(-1.0f, 1.0f),
					  (float)rnd::Uniform(-1.0f, 1.0f));
		rot_axis.Normalize();
		float angle = rnd::Uniform(0.0f, 2.0f * lepra::PIF);
		transform_.SetOrientation(lepra::quat(angle, rot_axis));
		physics_->SetBodyTransform(body_id_, transform_);
		UpdateTransformation();
	}

	void SetPos(const lepra::vec3& pos) {
		transform_.SetPosition(pos);
		physics_->SetBodyTransform(body_id_, transform_);
		UpdateTransformation();
	}

	void SetRot(const lepra::quat& rot) {
		transform_.SetOrientation(rot);
		physics_->SetBodyTransform(body_id_, transform_);
		UpdateTransformation();
	}

	void UpdateTransformation() {
		physics_->GetBodyTransform(body_id_, transform_);
		gfx_geom_->SetTransformation(transform_);
	}

	Type type_;
	tbc::PhysicsManager::BodyType body_type_;
	tbc::GeometryBase* gfx_geom_;
	uitbc::Renderer::GeometryID geom_id_;
	tbc::PhysicsManager::BodyID body_id_;

	uitbc::Renderer* renderer_;
	tbc::PhysicsManager* physics_;

	lepra::xform transform_;

	float ttl_; // Time to live.
};

void RunRotationalAgreementTest() {
	uilepra::DisplayManager* disp = uilepra::DisplayManager::CreateDisplayManager(uilepra::DisplayManager::kOpenglContext);
	uilepra::DisplayMode mode;
	bool ok = disp->FindDisplayMode(mode, 800, 600);
	if(ok)
		ok = disp->OpenScreen(mode, uilepra::DisplayManager::kWindowed, uilepra::DisplayManager::kOrientationAllowAny);

	lepra::Canvas screen;
	disp->GetScreenCanvas(screen);
	uitbc::OpenGLRenderer _renderer(&screen);
	tbc::PhysicsManager* _physics = tbc::PhysicsManagerFactory::Create(tbc::PhysicsManagerFactory::kEngineOde, 500, 3, 3);

	_renderer.SetShadowMode(uitbc::Renderer::kCastShadows, uitbc::Renderer::kShVolumesAndMaps);
	_physics->SetGravity(lepra::vec3(0, 0, -9.82f));

	enum {
		kDim = 1,
		kStaticCount = kDim*kDim + 1,
		kDynamicCount = 10,
	};

	Object* static_obj[kStaticCount];
	Object* dynamic_obj[kDynamicCount];

	// Create the big ground box.
	static_obj[0] = new Object(&_renderer, _physics);
	static_obj[0]->MakeBox(lepra::vec3(100.0f, 100.0f, 100.0f));
	static_obj[0]->SetPos(lepra::vec3(0, 0, -50.0f));
	int x;
	int y;
	int i;
	for(y = 0; y < kDim; y++) {
		for(x = 0; x < kDim; x++) {
			i = y * kDim + x + 1;
			static_obj[i] = new Object(&_renderer, _physics, tbc::PhysicsManager::kStatic);
			//static_obj[i]->MakeBox(lepra::vec3(3.0f / kDim, 10.0f / kDim, 0.1f));
			//static_obj[i]->SetPos(lepra::vec3((x+0.5f - 0.5f * kDim) * 10.0f / kDim, (y+0.5f - 0.5f * kDim) * 10.0f / kDim, 5.0f));

			// Try rotating using axis & angle.
			//lepra::vec3 axis(1, 0, 0);
			//axis.Normalize();
			//static_obj[i]->SetRot(lepra::quat(lepra::PIF / 4.0f, axis));

			// Random axis & angle.
			//static_obj[i]->SetRandomRot();

			// Rotation using help functions.
			//lepra::quat _rot;
			//_rot.RotateAroundWorldZ(lepra::PIF / 2.0f);
			//_rot.RotateAroundOwnX(-lepra::PIF / 4.0f);
			//_rot.RotateAroundOwnY(lepra::PIF / 4.0f);
			//_rot.RotateAroundOwnZ(lepra::PIF / 4.0f);
			//static_obj[i]->SetRot(_rot);

			// Jonte's version that shows up wrong (rotated 90 degrees around own X-axis).
			// Create physics first.
			static const float road_width = 4;
			static const float road_height = 0.1f;
			static const float uphill_length = 8;
			lepra::vec3 dimensions;
			lepra::xform transformation;
			//lepra::RotationMatrixF rotation;
			lepra::quat rotation;
			dimensions.Set(road_width, uphill_length, road_height);
			transformation.SetPosition(lepra::vec3(0, 0, 5));
			rotation.RotateAroundOwnX(lepra::PIF/4);
			//transformation.SetOrientation(rotation);
			transformation.RotatePitch(lepra::PIF/8);
			static_obj[i]->body_id_ = _physics->CreateBox(true, transformation, 0, dimensions, tbc::PhysicsManager::kStatic, 0.5f, 1.0f, 0);

			// Then graphics.
			tbc::GeometryBase::BasicMaterialSettings material(
				vec3(0, 0, 0),
				vec3(0.85f, 0.85f, 0.85f),
				vec3(0.5f, 0.5f, 0.5f),
				0.1f, 1.0f, true);
			uitbc::Renderer::MaterialType material_type = uitbc::Renderer::kMatSingleColorSolid;
			uitbc::Renderer::TextureID texture_id = uitbc::Renderer::INVALID_TEXTURE;
			uitbc::Renderer::Shadows shadow = uitbc::Renderer::kCastShadows;

			// Bug number 1, CreateFlatBox() is not intuitive. Swap z and y.
			static_obj[i]->gfx_geom_ = uitbc::BasicMeshCreator::CreateFlatBox(dimensions.x, dimensions.y, dimensions.z);
			static_obj[i]->gfx_geom_->SetAlwaysVisible(true);
			static_obj[i]->gfx_geom_->SetBasicMaterialSettings(material);

			// Not needed. The bug is there anyway.
			//_physics->GetBodyTransform(static_obj[i]->body_id_, transformation);

			// Uncomment the below to display the correctly rotated geometry.
			//transformation.GetOrientation().RotateAroundOwnX(lepra::PIF/2);
			static_obj[i]->gfx_geom_->SetTransformation(transformation);
			// Not setting the below causes a crash (feature?) within Renderer to be triggered.
			//static_obj[i]->geom_id_ =
			uitbc::Renderer::GeometryID graphic_id = _renderer.AddGeometry(static_obj[i]->gfx_geom_, material_type, shadow);
			_renderer.TryAddGeometryTexture(graphic_id, texture_id);
		}
	}

	for(i = 0; i < kDynamicCount; i++) {
		dynamic_obj[i] = new Object(&_renderer, _physics, tbc::PhysicsManager::kDynamic);
		//switch((Object::Type)(rnd::GetRandomNumber() % Object::kTypeCount))
		//{
		//case Object::kBox: dynamic_obj[i]->MakeBox(lepra::vec3((float)rnd::Uniform(0.1f, 0.9f), (float)rnd::Uniform(0.1f, 0.9f), (float)rnd::Uniform(0.1f, 0.9f))); break;
		//case Object::kSphere: dynamic_obj[i]->MakeSphere((float)rnd::Uniform(0.1f, 0.9f)); break;
		//}
		dynamic_obj[i]->MakeSphere((float)rnd::Uniform(0.2f, 0.4f));

		dynamic_obj[i]->SetRandomPos(lepra::vec3(-5.0f, -5.0f, 8.0f), lepra::vec3(5.0f, 5.0f, 8.0f));
		dynamic_obj[i]->ttl_ = (float)i;
	}

	lepra::xform cam_transform;
	cam_transform.MoveRight(7.0f);
	cam_transform.MoveUp(10.0f);
	cam_transform.RotateWorldX(-lepra::PIF / 4.0f);
	cam_transform.RotateWorldZ(lepra::PIF / 2.0f);
	_renderer.SetCameraTransformation(cam_transform);

	_renderer.SetLightsEnabled(true);
	_renderer.AddDirectionalLight(uitbc::Renderer::kLightStatic, lepra::vec3(1.0f, 0.0f, -1.0f), lepra::vec3(1.0f, 1.0f, 1.0f), 40.0f);
	_renderer.SetAmbientLight(0.5f, 0.5f, 0.5f);

	_renderer.SetViewFrustum(90.0f, 0.1f, 1000.0f);

	if(ok) {
		lepra::SystemManager::AddQuitRequest(-1);
		lepra::Timer total_timer;
		lepra::Timer delta_timer;
		i = 0;
		_renderer.SetClearColor(lepra::DARK_BLUE);
		while(!lepra::SystemManager::GetQuitRequest() && total_timer.GetTimeDiff() < 60.0f) {
			for(i = 0; i < kDynamicCount; i++) {
				dynamic_obj[i]->UpdateTransformation();
				dynamic_obj[i]->ttl_ -= (float)delta_timer.GetTimeDiff();
				if(dynamic_obj[i]->ttl_ <= 0.0f) {
					dynamic_obj[i]->ttl_ = (float)kDynamicCount;
					dynamic_obj[i]->SetRandomPos(lepra::vec3(-5.0f, -5.0f, 8.0f), lepra::vec3(5.0f, 5.0f, 8.0f));
				}
			}

			delta_timer.ClearTimeDiff();

			screen.SetBuffer(0);
			_renderer.ResetClippingRect();
			_renderer.Clear();
			_renderer.RenderScene();
			_physics->StepAccurate(0.05f, true);
			disp->UpdateScreen();

			delta_timer.UpdateTimer();
			while(delta_timer.GetTimeDiff() < 0.05f) {
				lepra::Thread::YieldCpu();
				delta_timer.UpdateTimer();
			}

			total_timer.UpdateTimer();
			i++;

			uilepra::Core::ProcessMessages();
		}
	}

	for(i = 0; i < kStaticCount; i++) {
		delete static_obj[i];
	}
	for(i = 0; i < kDynamicCount; i++) {
		delete dynamic_obj[i];
	}

	delete (_physics);
}

#endif // !CURE_TEST_WITHOUT_UI
