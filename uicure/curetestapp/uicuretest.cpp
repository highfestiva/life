
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#ifndef CURE_TEST_WITHOUT_UI
#include "../../lepra/include/lepraassert.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/gameticker.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/random.h"
#include "../include/uicppcontextobject.h"
#include "../include/uigameuimanager.h"
#include "../include/uiresourcemanager.h"
#include "../include/uicure.h"



using namespace lepra;
static int g_resource_load_count = 0;
static int g_resource_load_error_count = 0;
bool TestCure();
void ReportTestResult(const lepra::LogDecorator& log, const str& test_name, const str& context, bool result);



class ResourceTest {
public:
	ResourceTest();
	virtual ~ResourceTest();

	bool TestAtom();
	bool TestClass();
	bool TestStress();
	bool TestReloadContextObject();

private:
	class TickRM {
	public:
		inline TickRM(cure::ResourceManager* rm, double sleep_time = 0.001) {
			const int tick_count = 2;
			for (int x = 0; x < tick_count; ++x) {
				rm->Tick();
			}
			lepra::Thread::Sleep(sleep_time);
			for (int x = 0; x < tick_count; ++x) {
				rm->Tick();
			}
		}
	};

	void RendererImageLoadCallback(UiCure::UserRendererImageResource* resource) {
		LoadCallback(resource);
	}
	void PainterImageLoadCallback(UiCure::UserPainterImageResource* resource) {
		LoadCallback(resource);
	}
	/*void TextureLoadCallback(UiCure::UserTextureResource* resource) {
		LoadCallback(resource);
	}*/
	void Sound2dLoadCallback(UiCure::UserSound2dResource* resource) {
		LoadCallback(resource);
	}
	void ClassLoadCallback(UiCure::UserClassResource* resource) {
		LoadCallback(resource);

		uitbc::ChunkyClass* clazz = resource->GetData();
		physics_resource_->LoadUnique(resource->GetConstResource()->GetManager(),
			clazz->GetPhysicsBaseName()+".phys",
			cure::UserPhysicsResource::TypeLoadCallback(this,
				&ResourceTest::PhysicsLoadCallback));
		const size_t mesh_count = clazz->GetMeshCount();
		for (size_t x = 0; x < mesh_count; ++x) {
			int phys_index = -1;
			str name;
			lepra::xform transform;
			float mesh_scale;
			clazz->GetMesh(x, phys_index, name, transform, mesh_scale);
			mesh_resource_array_.push_back(new UiCure::UserGeometryReferenceResource(
				ui_manager_, UiCure::GeometryOffset(phys_index, transform, mesh_scale)));
			strutil::strvec v = strutil::Split(name, ";");
			if (v.size() < 2) { v.push_back("0"); }
			mesh_resource_array_[x]->Load(resource->GetConstResource()->GetManager(),
				v[0] + ".mesh;0" + v[1],
				UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
					&ResourceTest::MeshRefLoadCallback));
		}
	}
	void PhysicsLoadCallback(cure::UserPhysicsResource* resource) {
		LoadCallback(resource);
	}
	void MeshRefLoadCallback(UiCure::UserGeometryReferenceResource* resource) {
		LoadCallback(resource);
	}
	void MeshLoadCallback(UiCure::UserUiTypeResource<UiCure::GeometryResource>* resource) {
		LoadCallback(resource);
	}
	void DumbClassLoadCallback(UiCure::UserClassResource*) {
	}
	void LoadCallback(cure::UserResource* resource) {
		if (resource->GetConstResource()->GetLoadState() == cure::kResourceLoadComplete) {
			++g_resource_load_count;
		} else {
			++g_resource_load_error_count;
		}
	}
	bool InternalLoadTransformation(cure::ContextManager& context_manager);

	cure::ResourceManager* resource_manager_;
	UiCure::GameUiManager* ui_manager_;
	cure::UserPhysicsResource* physics_resource_;
	typedef std::vector<UiCure::UserGeometryReferenceResource*> MeshArray;
	MeshArray mesh_resource_array_;

	logclass();
};



class TestGameTicker: public cure::GameTicker {
public:
	typedef cure::GameTicker Parent;

	TestGameTicker():
		Parent(1, 1, 1) {
	}

	virtual bool Initialize() {return true;}
	virtual bool Tick() {return true;}
	virtual void PollRoundTrip() {}
	virtual float GetTickTimeReduction() const {return 0;}
	virtual float GetPowerSaveAmount() const {return 0;}
	virtual void WillMicroTick(float time_delta) {(void)time_delta;}
	virtual void DidPhysicsTick() {}
	virtual void OnTrigger(tbc::PhysicsManager::BodyID, int, int, tbc::PhysicsManager::BodyID, const vec3&, const vec3&) {}
	virtual void OnForceApplied(int, int,
		tbc::PhysicsManager::BodyID, tbc::PhysicsManager::BodyID,
		const vec3&, const vec3&,
		const vec3&, const vec3&) {}
};



class TestGameManager: public cure::GameManager {
public:
	typedef cure::GameManager Parent;

	TestGameManager(const cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager):
		Parent(time, variable_scope, resource_manager) {
	}

	virtual cure::ContextObject* CreateContextObject(const str& class_id) const {(void)class_id; return 0;}
	virtual void OnLoadCompleted(cure::ContextObject* object, bool ok) {(void)object; (void)ok;}
	virtual void OnCollision(const vec3&, const vec3&, const vec3&,
		cure::ContextObject*, cure::ContextObject*,
		tbc::PhysicsManager::BodyID, tbc::PhysicsManager::BodyID) {}
	virtual bool OnPhysicsSend(cure::ContextObject* object) {(void)object; return true;}
	virtual bool OnAttributeSend(cure::ContextObject* object) {(void)object; return true;}
	virtual bool IsServer() {return false;}
	virtual void SendAttach(cure::ContextObject*, unsigned, cure::ContextObject*, unsigned) {}
	virtual void SendDetach(cure::ContextObject*, cure::ContextObject*) {}
	virtual void TickInput() {}
};



ResourceTest::ResourceTest() {
	ui_manager_ = new UiCure::GameUiManager(UiCure::GetSettings(), 0);
	ui_manager_->Open();
	resource_manager_ = new cure::ResourceManager(1);
	resource_manager_->InitDefault();
	resource_manager_->SetLoadIntermission(0);
	resource_manager_->SetInjectTimeLimit(0.3);

	physics_resource_ = new cure::UserPhysicsResource();
}

ResourceTest::~ResourceTest() {
	delete (resource_manager_);	// Must be first to allow resources suicide.
	delete (ui_manager_);
}

bool ResourceTest::TestAtom() {
	str _context;
	bool test_ok = true;

	// Test loading a 2D image.
	if (test_ok) {
		_context = "load 2D image";
		UiCure::UserRendererImageResource image(ui_manager_, UiCure::ImageProcessSettings(Canvas::kResizeFast, true));
		image.Load(resource_manager_, "blue_eye.png", UiCure::UserRendererImageResource::TypeLoadCallback(this, &ResourceTest::RendererImageLoadCallback));
		lepra::Thread::Sleep(0.4);
		resource_manager_->Tick();
		resource_manager_->SafeRelease(&image);
		test_ok = (g_resource_load_count == 1 && g_resource_load_error_count == 0);
		deb_assert(test_ok);
	}

	// Test loading a 2D image into a texture.
	if (test_ok) {
		_context = "load 2D image into texture";
		// Free previous resource; it's a requirement since the name must be
		// unique throughout all resource types.
		resource_manager_->ForceFreeCache();
		UiCure::UserPainterImageResource image(ui_manager_);
		image.Load(resource_manager_, "blue_eye.png", UiCure::UserPainterImageResource::TypeLoadCallback(this, &ResourceTest::PainterImageLoadCallback));
		lepra::Thread::Sleep(0.4);
		resource_manager_->Tick();
		resource_manager_->SafeRelease(&image);
		test_ok = (g_resource_load_count == 2 && g_resource_load_error_count == 0);
		deb_assert(test_ok);
	}

	// Test loading a 3D texture (own, mipmapped format).
	// We don't have a texture file, so this test should fail.
	if (test_ok) {
		_context = "load 3D texture";
		UiCure::UserRendererImageResource texture(ui_manager_, UiCure::ImageProcessSettings(Canvas::kResizeFast, false));
		texture.Load(resource_manager_, "NoSuchFile.tga", UiCure::UserRendererImageResource::TypeLoadCallback(this, &ResourceTest::RendererImageLoadCallback));
		lepra::Thread::Sleep(0.4);
		resource_manager_->Tick();
		resource_manager_->SafeRelease(&texture);
		test_ok = (g_resource_load_count == 2 && g_resource_load_error_count == 1);
		deb_assert(test_ok);
	}

	/*// Test loading a 3D texture that we actually do have (at least if we've ever run the uitbc tests).
	if (test_ok) {
		_context = "load 3D texture";
		UiCure::UserRendererImageResource texture(ui_manager_);
		texture.Load(resource_manager_, "normalmap.tex", UiCure::UserRendererImageResource::TypeLoadCallback(this, &ResourceTest::TextureLoadCallback));
		lepra::Thread::Sleep(0.4);
		resource_manager_->Tick();
		resource_manager_->SafeRelease(&texture);
		test_ok = (g_resource_load_count == 3 && g_resource_load_error_count == 1);
		if (!test_ok) {
			log_.Error("texture not available, try running UiTbc test first!");
		}
		deb_assert(test_ok);
	}*/

	// Test loading a sound.
	if (test_ok) {
		_context = "load sound";
		UiCure::UserSound2dResource sound(ui_manager_, uilepra::SoundManager::kLoopNone);
		sound.Load(resource_manager_, "logo_trumpet.wav", UiCure::UserSound2dResource::TypeLoadCallback(this, &ResourceTest::Sound2dLoadCallback));
		lepra::Thread::Sleep(0.4);
		resource_manager_->Tick();
		test_ok = (g_resource_load_count == 3 && g_resource_load_error_count == 1);
		deb_assert(test_ok);
		if (test_ok) {
			_context = "play sound";
			test_ok = ui_manager_->GetSoundManager()->Play(sound.GetData(), 1.0, 1.0);
			deb_assert(test_ok);
			lepra::Thread::Sleep(0.5);
		}
		resource_manager_->SafeRelease(&sound);
		deb_assert(test_ok);
	}

	ReportTestResult(log_, "ResourceAtom", _context, test_ok);
	return (test_ok);
}

bool ResourceTest::TestClass() {
	str _context;
	bool test_ok = true;

	if (test_ok) {
		_context = "load class";
		g_resource_load_count = 0;
		g_resource_load_error_count = 0;
		UiCure::UserClassResource clazz(ui_manager_);
		clazz.Load(resource_manager_, "UI:hover_tank_01.class", UiCure::UserClassResource::TypeLoadCallback(this, &ResourceTest::ClassLoadCallback));
		for (int x = 0; g_resource_load_count != 7 && x < 100; ++x) {
			lepra::Thread::Sleep(0.01);
			resource_manager_->Tick();	// Continue loading of physics and meshes.
		}
		// Make sure nothing more makes it through.
		lepra::Thread::Sleep(0.1);
		resource_manager_->Tick();
		test_ok = (g_resource_load_count == 7 && g_resource_load_error_count == 0);
		resource_manager_->SafeRelease(&clazz);
		deb_assert(test_ok);
	}

	delete (physics_resource_);
	physics_resource_ = 0;
	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		delete (mesh_resource_array_[x]);
	}
	mesh_resource_array_.clear();
	resource_manager_->StopClear();
	resource_manager_->InitDefault();

	ReportTestResult(log_, "ResourceClass", _context, test_ok);
	return (test_ok);
}

bool ResourceTest::TestStress() {
	str _context;
	bool test_ok = true;

	if (test_ok) {
		_context = "no loaded resources";
		test_ok = (resource_manager_->QueryResourceCount() == 0);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "caching 0";
		test_ok = (resource_manager_->QueryCachedResourceCount() == 0);
		deb_assert(test_ok);
	}

	if (test_ok) {
		_context = "stress load/free unique";
		for (int x = 0; x <= 0x100; ++x) {
			UiCure::UserGeometryReferenceResource* mesh0 =
				new UiCure::UserGeometryReferenceResource(ui_manager_, UiCure::GeometryOffset(0));
			UiCure::UserGeometryReferenceResource* mesh1 =
				new UiCure::UserGeometryReferenceResource(ui_manager_, UiCure::GeometryOffset(0));
			mesh0->LoadUnique(resource_manager_, "hover_tank_01_skid.mesh;0",
				UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
					&ResourceTest::MeshRefLoadCallback));
			mesh1->LoadUnique(resource_manager_, "hover_tank_01_skid.mesh;0",
				UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
					&ResourceTest::MeshRefLoadCallback));
			delete (mesh1);
			delete (mesh0);
			if ((x&7) == 0) {
				test_ok = false;
				size_t count = 0;
				for (int i = 0; i < 250 && !test_ok; ++i) {
					TickRM tick(resource_manager_);
					count = resource_manager_->QueryResourceCount();
					test_ok = (count <= 2);
				}
				deb_assert(test_ok);
			}
		}
		TickRM tick(resource_manager_);
	}

	if (test_ok) {
		_context = "caching 1";
		test_ok = (resource_manager_->QueryCachedResourceCount() == 0);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "clearing cache 1";
		test_ok = (resource_manager_->ForceFreeCache() == 0);
		deb_assert(test_ok);
		if (test_ok) {
			Thread::Sleep(0.01);
			test_ok = (resource_manager_->QueryResourceCount() == 0);
			deb_assert(test_ok);
		}
	}

	if (test_ok) {
		_context = "stress load/free mass";
		for (int x = 0; x <= 0x100; ++x) {
			typedef UiCure::UserUiTypeResource<UiCure::GeometryResource> UserMesh;
			UserMesh* mesh0 = new UserMesh(ui_manager_);
			UserMesh* mesh1 = new UserMesh(ui_manager_);
			mesh0->Load(resource_manager_, "hover_tank_01_skid.mesh",
				UserMesh::TypeLoadCallback(this, &ResourceTest::MeshLoadCallback));
			mesh1->Load(resource_manager_, "hover_tank_01_skid.mesh",
				UserMesh::TypeLoadCallback(this, &ResourceTest::MeshLoadCallback));
			delete (mesh1);
			delete (mesh0);
			if ((x&7) == 0) {
				test_ok = false;
				for (int i = 0; i < 250 && !test_ok; ++i) {
					TickRM tick(resource_manager_);
					const size_t count = resource_manager_->QueryResourceCount();
					test_ok = (count <= 1);
				}
				deb_assert(test_ok);
			}
		}
	}

	if (test_ok) {
		_context = "caching 2";
		test_ok = (resource_manager_->QueryCachedResourceCount() == 0);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "clearing cache 2";
		resource_manager_->ForceFreeCache();
		test_ok = (resource_manager_->QueryResourceCount() == 0);
		deb_assert(test_ok);
	}

	const int loop_count = 30;
	const int add_count = 10;
	const int dec_count = add_count/3;

	if (test_ok) {
		_context = "stressing unique load";
		typedef std::list<UiCure::UserClassResource*> ClassList;
		ClassList resources;
		for (int x = 0; x < loop_count; ++x) {
			for (int y = 0; y < add_count; ++y) {
				UiCure::UserClassResource* clazz = new UiCure::UserClassResource(ui_manager_);
				clazz->LoadUnique(resource_manager_, "UI:hover_tank_01.class",
					UiCure::UserClassResource::TypeLoadCallback(this, &ResourceTest::DumbClassLoadCallback));
				resources.push_back(clazz);
			}
			size_t c = resource_manager_->QueryResourceCount();
			deb_assert(c == (size_t)(x*(add_count-dec_count)+add_count));
			for (int z = 0; z < dec_count; ++z) {
				int drop_index = lepra::Random::GetRandomNumber()%resources.size();
				ClassList::reverse_iterator u = resources.rbegin();
				for (int v = 0; v < drop_index; ++u, ++v)
					;
				UiCure::UserClassResource* clazz = *u;
				resources.erase((++u).base());
				delete clazz;
			}
			test_ok = false;
			for (int i = 0; i < 250 && !test_ok; ++i) {
				TickRM tick(resource_manager_);
				c = resource_manager_->QueryResourceCount();
				test_ok = (c == (size_t)((x+1)*(add_count-dec_count)));
			}
			deb_assert(test_ok);
		}
		test_ok = (resource_manager_->QueryResourceCount() == loop_count*(add_count-add_count/3));
		deb_assert(test_ok);
		if (test_ok) {
			_context = "unique load completeness";
			test_ok = false;
			for (int z = 0; !test_ok && z < 100; ++z) {
				TickRM tick(resource_manager_, 0.1);

				test_ok = true;
				ClassList::iterator y = resources.begin();
				for (int u = 0; test_ok && y != resources.end(); ++y, ++u) {
					test_ok = ((*y)->GetLoadState() == cure::kResourceLoadComplete);
					if (!test_ok && z == 99) {
						log_.Warningf("Failed on the %ith element; load state = %i.", u, (*y)->GetLoadState());
					}
				}
			}
			deb_assert(test_ok);
		}
		if (test_ok) {
			_context = "unique load clear";
			ClassList::iterator y = resources.begin();
			for (; y != resources.end(); ++y) {
				delete (*y);
			}
			resources.clear();
			test_ok = (resource_manager_->QueryResourceCount() == 0);
			deb_assert(test_ok);
		}
	}

	if (test_ok) {
		_context = "stressing mass load";
		g_resource_load_count = 0;
		typedef std::list<UiCure::UserGeometryReferenceResource*> MeshList;
		MeshList resources;
		const int _loop_count = 100;
		const int _add_count = 10;
		const int _dec_count = _add_count/3;
		for (int x = 0; x < _loop_count; ++x) {
			for (int y = 0; y < _add_count; ++y) {
				UiCure::UserGeometryReferenceResource* mesh =
					new UiCure::UserGeometryReferenceResource(ui_manager_, UiCure::GeometryOffset(0));
				mesh->Load(resource_manager_, "hover_tank_01_tower.mesh;0",
					UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
						&ResourceTest::MeshRefLoadCallback));
				resources.push_back(mesh);
			}
			size_t c = resource_manager_->QueryResourceCount();
			deb_assert(c <= 2);
			for (int z = 0; z < _dec_count; ++z) {
				int drop_index = lepra::Random::GetRandomNumber()%resources.size();
				MeshList::reverse_iterator u = resources.rbegin();
				for (int v = 0; v < drop_index; ++u, ++v)
					;
				delete (*u);
				resources.erase(--u.base());
			}
			test_ok = false;
			for (int i = 0; i < 250 && !test_ok; ++i) {
				TickRM tick(resource_manager_);
				c = resource_manager_->QueryResourceCount();
				test_ok = (c <= 2);
			}
			deb_assert(test_ok);
		}
		if (test_ok) {
			_context = "resource load count";
			TickRM tick(resource_manager_, 0.1);
			size_t c = resource_manager_->QueryResourceCount();
			test_ok = (c == 2);
			deb_assert(test_ok);
		}
		if (test_ok) {
			_context = "mass load completeness";
			MeshList::iterator y = resources.begin();
			int u = 0;
			for (; test_ok && y != resources.end(); ++y, ++u) {
				test_ok = ((*y)->GetLoadState() == cure::kResourceLoadComplete);
				if (!test_ok) {
					log_.Warningf("Failed on the %ith element (of %u); load state = %i.",
						u, resources.size(), (*y)->GetLoadState());
				}
			}
			deb_assert(test_ok);
		}
		if (test_ok) {
			_context = "mass ref clear";
			MeshList::iterator y = resources.begin();
			for (; y != resources.end(); ++y) {
				delete (*y);
			}
			resources.clear();
			test_ok = (resource_manager_->QueryResourceCount() == 2);
			deb_assert(test_ok);
		}
		if (test_ok) {
			_context = "mass cache clear";
			resource_manager_->ForceFreeCache();
			test_ok = (resource_manager_->QueryResourceCount() == 0);
			deb_assert(test_ok);
		}
	}

	ReportTestResult(log_, "ResourceStress", _context, test_ok);
	return (test_ok);
}

/// This is a pretty complete application test, with game tickers, managers, loading and so forth. Wow!
bool ResourceTest::TestReloadContextObject() {
	str _context;
	bool test_ok = true;

	delete resource_manager_;
	resource_manager_ = new cure::ResourceManager(1);
	resource_manager_->InitDefault();
	TestGameTicker ticker;
	cure::TimeManager time_manager;
	TestGameManager game_manager(&time_manager, new cure::RuntimeVariableScope(UiCure::GetSettings()), resource_manager_);
	game_manager.SetTicker(&ticker);
	cure::ContextManager _context_manager(&game_manager);
	if (test_ok) {
		_context = "init transformation suxx";
		test_ok = InternalLoadTransformation(_context_manager);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "reload transformation suxx";
		test_ok = InternalLoadTransformation(_context_manager);
		deb_assert(test_ok);
	}

	ReportTestResult(log_, "ReloadTransform", _context, test_ok);
	return (test_ok);
}

bool ResourceTest::InternalLoadTransformation(cure::ContextManager& context_manager) {
	// Generate orientation to set and to test against.
	quat q;
	q.RotateAroundOwnX(PIF/5);
	q.RotateAroundOwnY(PIF/5);
	q.RotateAroundOwnZ(PIF/5);
	quat flip;
	flip.RotateAroundOwnZ(PIF);
	flip.RotateAroundOwnX(PIF/2);
	quat Q = -(q * flip);

	UiCure::CppContextObject _object(resource_manager_, "hover_tank_01", ui_manager_);
	context_manager.AddLocalObject(&_object);
	xform transform(q, vec3(1, 2, 3));
	_object.SetInitialTransform(transform);
	_object.StartLoading();
	for (int x = 0; x < 10 && !_object.IsLoaded(); ++x) {
		TickRM tick(resource_manager_, 0.1);
	}
	bool test_ok = _object.IsLoaded();
	deb_assert(test_ok);
	if (test_ok) {
		vec3 p = _object.GetPosition();
		test_ok = (p.x == 1 && Math::IsInRange(p.y, 1.0f, 3.0f) && Math::IsInRange(p.z, 2.0f, 4.0f));
		deb_assert(test_ok);
	}
	if (test_ok) {
		quat r = _object.GetOrientation();
		test_ok = (Math::IsEpsEqual(r.a, Q.a, 0.1f) &&
			Math::IsEpsEqual(r.b, Q.b, 0.1f) &&
			Math::IsEpsEqual(r.c, Q.c, 0.1f) &&
			Math::IsEpsEqual(r.d, Q.d, 0.1f));
		deb_assert(test_ok);
	}
	return test_ok;
}


loginstance(kTest, ResourceTest);



bool TestUiCure() {
	bool test_ok = true;
	if (test_ok) {
		test_ok = TestCure();
	}
	ResourceTest resource_test;
	if (test_ok) {
		test_ok = resource_test.TestAtom();
	}
	if (test_ok) {
		test_ok = resource_test.TestClass();
	}
	if (test_ok) {
		test_ok = resource_test.TestStress();
	}
	if (test_ok) {
		test_ok = resource_test.TestReloadContextObject();
	}
	return (test_ok);
}



#endif // !CURE_TEST_WITHOUT_UI
