
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include <assert.h>
#include "../../Lepra/Include/SystemManager.h"
#include "../Include/ContextObject.h"
#include "../Include/Cure.h"
#include "../Include/ResourceManager.h"
#include "../Include/TerrainFunctionManager.h"
#include "../Include/TerrainPatchLoader.h"



namespace Cure
{



UserResource::UserResource():
	mResource(0),
	mParentResource(0),
	mCallbackBlockCount(0)
{
}

UserResource::~UserResource()
{
	if (mResource)
	{
		SafeRelease(mResource->GetManager());
	}
	SetParentResource(0);
}

void UserResource::SafeRelease(ResourceManager* pManager)
{
	pManager->SafeRelease(this);
	mResource = 0;
}

void UserResource::SetParentResource(UserResource* pParentResource)
{
	if (pParentResource)
	{
		assert(!mParentResource);
		assert(pParentResource != this);
		mParentResource = pParentResource;
		mParentResource->IncreaseCallbackBlockCount();
	}
	else
	{
		if (mParentResource)
		{
			mParentResource->DecreaseCallbackBlockCount();
		}
		mParentResource = pParentResource;
	}
}

void UserResource::CallbackParent(ResourceLoadState pChildLoadState)
{
	UserResource* lParent = mParentResource;
	mParentResource = 0;
	if (lParent)
	{
		if (pChildLoadState == RESOURCE_LOAD_ERROR)
		{
			lParent->GetResource()->SetLoadState(pChildLoadState);
		}
		lParent->GetResource()->Callback();
	}
}

void UserResource::IncreaseCallbackBlockCount()
{
	++mCallbackBlockCount;
}

int UserResource::DecreaseCallbackBlockCount()
{
	int lCallbackBlockCount = mCallbackBlockCount;
	if (mCallbackBlockCount)
	{
		--mCallbackBlockCount;
	}
	return (lCallbackBlockCount);
}

const Lepra::String& UserResource::GetName() const
{
	return (mResource->GetName());
}

ResourceLoadState UserResource::GetLoadState() const
{
	ResourceLoadState lState = RESOURCE_UNLOADED;
	if (mResource)
	{
		lState = mResource->GetLoadState();
	}
	return (lState);
}

const Resource* UserResource::GetConstResource() const
{
	return (mResource);
}

Resource* UserResource::GetResource() const
{
	return (mResource);
}

void UserResource::SetResource(Resource* pResource)
{
	mResource = pResource;
}

void UserResource::PostProcess()
{
}



Resource::Resource(ResourceManager* pManager, const Lepra::String& pName):
	mManager(pManager),
	mName(pName),
	mReferenceCount(0),
	mState(RESOURCE_LOAD_ERROR)
{
}

Resource::~Resource()
{
	mState = RESOURCE_LOAD_ERROR;
	log_volatile(mLog.Debugf((_T("Deleting resource ")+mName).c_str(), Lepra::Log::LEVEL_TRACE));
	mManager = 0;

	CallbackList::iterator x = mLoadCallbackList.begin();
	while (x != mLoadCallbackList.end())
	{
		UserResourceCallbackInfo& lCaller = *x;
		if (lCaller.mUserResource->GetConstResource() == this)
		{
			lCaller.mUserResource->SetResource(0);
		}
		else
		{
			x = mLoadCallbackList.erase(x);
		}
	}
	mLoadCallbackList.clear();
}

ResourceManager* Resource::GetManager() const
{
	return (mManager);
}

const Lepra::String& Resource::GetName() const
{
	return (mName);
}

int Resource::Reference()
{
	++mReferenceCount;
	return (mReferenceCount);
}

int Resource::Dereference()
{
	--mReferenceCount;
	return (mReferenceCount);
}

int Resource::GetReferenceCount()
{
	return (mReferenceCount);
}

bool Resource::IsUnique() const
{
	return (false);
}

ResourceLoadState Resource::GetLoadState() const
{
	return (mState);
}

void Resource::SetLoadState(ResourceLoadState pState)
{
	mState = pState;
}

void Resource::AddCaller(UserResource* pUserResource, const UserResource::LoadCallback& pCallback)
{
	mLoadCallbackList.push_back(UserResourceCallbackInfo(pUserResource, pCallback));
}

void Resource::RemoveCaller(UserResource* pUserResource)
{
	CallbackList::iterator x = mLoadCallbackList.begin();
	while (x != mLoadCallbackList.end())
	{
		UserResourceCallbackInfo& lCaller = *x;
		if (lCaller.mUserResource == pUserResource)
		{
			mLoadCallbackList.erase(x);
			break;
		}
		else
		{
			++x;
		}
	}
}

ResourceLoadState Resource::PostProcess()
{
	UserPostProcess();
	return (RESOURCE_LOAD_COMPLETE);	// No post-processing required; we're done.
}

void Resource::UserPostProcess()
{
	CallbackList::iterator x = mLoadCallbackList.begin();
	for (; x != mLoadCallbackList.end(); ++x)
	{
		UserResourceCallbackInfo& lCaller = *x;
		lCaller.mUserResource->PostProcess();
	}
}

void Resource::Callback()
{
	Reference();	// Is here to stop a callback from dereferencing+deleting this resource.

	int lCallbackCount = (int)mLoadCallbackList.size();
	for (int x = 0; x < lCallbackCount; ++x)
	{
		UserResourceCallbackInfo lCaller = *mLoadCallbackList.begin();
		mLoadCallbackList.pop_front();
		if (lCaller.mUserResource->DecreaseCallbackBlockCount() > 0)
		{
			// Move to back of list = don't call yet.
			mLoadCallbackList.push_back(lCaller);
		}
		else
		{
			const int lPreviousCount = (int)mLoadCallbackList.size();
			ResourceLoadState lLoadState = lCaller.mUserResource->GetResource()->GetLoadState();
			lCaller.mCallback(lCaller.mUserResource);
			lCaller.mUserResource->CallbackParent(lLoadState);
			// Skip the once deleted during the parent's callback.
			const int lCurrentCount = (int)mLoadCallbackList.size();
			if (lCurrentCount < lPreviousCount)
			{
				lCallbackCount -= lPreviousCount-lCurrentCount;
			}
		}
	}

	mManager->Release(this);
}

void Resource::FreeDiversified(UserResource*)
{
}

void Resource::operator=(const Resource&)
{
	assert(false);
}

LOG_CLASS_DEFINE(GENERAL_RESOURCES, Resource);



ContextObjectResource::ContextObjectResource(ResourceManager* pManager, const Lepra::String& pName):
	Parent(pManager, pName)
{
}

ContextObjectResource::~ContextObjectResource()
{
	// TRICKY: delete of data must lie in this class, and not in parent class RamResource.
	// This is so since types (UiLepra::SoundManager::SoundID) cannot be deleted!
	SetRamData(0);

	log_atrace("Deleting context object resource.");
}

const Lepra::String ContextObjectResource::GetType() const
{
	return (_T("Context"));
}

ContextObjectResource::UserData ContextObjectResource::GetUserData(const Cure::UserResource*) const
{
	return (GetRamData());
}

bool ContextObjectResource::Load()
{
	log_atrace("Loading context object (self).");

	ContextObject* lObject = GetManager()->GetContextObjectFactory()->Create(GetName());
	SetRamData(lObject);
	return (lObject != 0);
}

ResourceLoadState ContextObjectResource::PostProcess()
{
	log_atrace("Loading context object (group).");
	ResourceLoadState lLoadState = GetManager()->GetContextObjectFactory()->CreatePhysics(GetRamData(), 0) ?
		RESOURCE_LOAD_COMPLETE : RESOURCE_LOAD_ERROR;
	if (lLoadState == RESOURCE_LOAD_COMPLETE)
	{
		Parent::PostProcess();
	}
	return (lLoadState);
}

bool ContextObjectResource::IsUnique() const
{
	return (true);
}

LOG_CLASS_DEFINE(GAME_CONTEXT, ContextObjectResource);



PhysicalTerrainResource::PhysicalTerrainResource(ResourceManager* pManager, const Lepra::String& pName):
	Parent(pManager, pName)
{
}

PhysicalTerrainResource::~PhysicalTerrainResource()
{
	// TRICKY: delete of data must lie in this class, and not in parent class RamResource.
	// This is so since types (UiLepra::SoundManager::SoundID) cannot be deleted!
	SetRamData(0);

	log_atrace("Deleting terrain patch resource.");
}

const Lepra::String PhysicalTerrainResource::GetType() const
{
	return (_T("PhysTerrain"));
}

PhysicalTerrainResource::UserData PhysicalTerrainResource::GetUserData(const Cure::UserResource*) const
{
	return (GetRamData());
}

bool PhysicalTerrainResource::Load()
{
	log_atrace("Loading actual TBC::TerrainPatch.");

	// TODO: parse constants out of resource name string.
	const TerrainPatchLoader::PatchArea lArea(0, 0, 0, 0);
	const float lLod = 0;

	TerrainPatchLoader lLoader(GetManager());
	TBC::TerrainPatch* lPatch = lLoader.LoadPatch(lArea, lLod);
	if (lPatch)
	{
		SetRamData(lPatch);
	}
	return (lPatch != 0);
}

LOG_CLASS_DEFINE(PHYSICS, PhysicalTerrainResource);



ResourceManager::ResourceManager(unsigned pLoaderThreadCount):
	mContextObjectFactory(0),
	mTerrainFunctionManager(0),
	mLoaderThreadCount(pLoaderThreadCount),
	mLoaderThread(_T("ResourceLoader"))
{
}

ResourceManager::~ResourceManager()
{
	StopClear();
	SetContextObjectFactory(0);
	SetTerrainFunctionManager(0);
}

bool ResourceManager::InitDefault(ContextObjectFactory* pContextObjectFactory)
{
	// Resources below deleted in the resource manager destructor.
	SetContextObjectFactory(pContextObjectFactory);
	SetTerrainFunctionManager(new TerrainFunctionManager());
	return (mLoaderThread.Start(this, &ResourceManager::ThreadLoaderLoop));
}

void ResourceManager::StopClear()
{
	// Join thread. 1: request stop, 2: signal, 3: sleep, 4: wait for termination.
	mLoaderThread.RequestStop();
	for (int x = 0; x < 10; ++x)
	{
		mLoadSemaphore.Signal();
		if (mLoaderThread.Join(1.0))
		{
			break;
		}
	}
	mLoaderThread.Kill();

	// Free memory.
	ForceFreeCache();	// Must be before and after active resources' are deleted (caused by hierarchical resource structures).
	while (!mActiveResourceTable.IsEmpty())
	{
		ResourceTable::Iterator x = mActiveResourceTable.First();
		Resource* lResource = *x;
		assert(mRequestLoadList.Find(lResource->GetName()) == mRequestLoadList.End());
		mActiveResourceTable.Remove(x);
		// Check that no-one else has deleted our resource.
		if (mResourceSafeLookup.find(lResource) != mResourceSafeLookup.end())
		{
			DeleteResource(lResource);
		}
	}
	ForceFreeCache();	// Must be before and after active resources' are deleted (caused by hierarchical resource structures).

	// These just contain pointers, they are not data owners.
	// Therefore no delete is done.
	mRequestLoadList.RemoveAll();
	mLoadedList.RemoveAll();

	ResourceSet::iterator x = mResourceSafeLookup.begin();
	for (; x != mResourceSafeLookup.end(); ++x)
	{
		mLog.Errorf(_T("Resource '%s' not freed!"), (*x)->GetName().c_str());
	}
	assert(mResourceSafeLookup.empty());
}



ContextObjectFactory* ResourceManager::GetContextObjectFactory() const
{
	return (mContextObjectFactory);
}

void ResourceManager::SetContextObjectFactory(ContextObjectFactory* pContextObjectFactory)
{
	if (mContextObjectFactory)
	{
		delete (mContextObjectFactory);
	}
	mContextObjectFactory = pContextObjectFactory;
}

TerrainFunctionManager* ResourceManager::GetTerrainFunctionManager() const
{
	return (mTerrainFunctionManager);
}

void ResourceManager::SetTerrainFunctionManager(TerrainFunctionManager* pTerrainFunctionManager)
{
	if (mTerrainFunctionManager)
	{
		delete (mTerrainFunctionManager);
	}
	mTerrainFunctionManager = pTerrainFunctionManager;
}



void ResourceManager::Load(const Lepra::String& pName, UserResource* pUserResource, UserResource::LoadCallback pCallback)
{
	bool lMustLoad = true;
	Lepra::ScopeLock lLock(&mThreadLock);
	Resource* lResource = QueryCachedResource(pName, pUserResource, lMustLoad);
	pUserResource->SetResource(lResource);
	lResource->AddCaller(pUserResource, pCallback);
	if (lMustLoad)
	{
		// Pass on to loader thread.
		lResource->SetLoadState(RESOURCE_LOAD_IN_PROGRESS);
		mRequestLoadList.GetCount();
		log_volatile(mLog.Debugf(_T("Requesting load of '%s'."), pName.c_str()));
		assert(mRequestLoadList.Find(pName) == mRequestLoadList.End());
		mRequestLoadList.PushBack(pName, lResource);
		mLoadSemaphore.Signal();
	}
	else
	{
		if (lResource->GetLoadState() == RESOURCE_LOAD_IN_PROGRESS)
		{
			// Loader thread is going to put this object in the request load list when it's done.
			// We have already placed the caller in the list, so we don't need to do anything else.
		}
		else
		{
			// The object is already loaded by some previous call. All we need to to is to inform the
			// current caller of resource load result.
			if (lResource->GetLoadState() == RESOURCE_LOAD_COMPLETE)
			{
				lResource->UserPostProcess();
			}
			lResource->Callback();
		}
	}
}

bool ResourceManager::IsCreated(const Lepra::String& pName) const
{
	return (mActiveResourceTable.FindObject(pName) != 0);
}

void ResourceManager::SafeRelease(UserResource* pUserResource)
{
	Resource* lResource = pUserResource->GetResource();
	if (lResource)
	{
		pUserResource->SetResource(0);

		Lepra::ScopeLock lLock(&mThreadLock);
		if (mResourceSafeLookup.find(lResource) != mResourceSafeLookup.end())
		{
			lResource->RemoveCaller(pUserResource);
			lResource->FreeDiversified(pUserResource);
			Release(lResource);
		}
	}
}

void ResourceManager::Release(Resource* pResource)
{
	if (pResource->Dereference() <= 0)
	{
		mActiveResourceTable.Remove(pResource->GetName());
		if (!pResource->IsUnique())
		{
			// Resource dropped, place it "on the way out" of the system.
			log_volatile(mLog.Debug(_T("Resource ")+pResource->GetName()+_T(" dereferenced. Placed in cache.")));
			mCachedResourceTable.Insert(pResource->GetName(), pResource);
		}
		else
		{
			if (PrepareDeleteInLoadProgress(pResource))
			{
				log_volatile(mLog.Debug(_T("Resource ")+pResource->GetName()+_T(" dereferenced. Deleted immediately.")));
				assert(mRequestLoadList.Find(pResource->GetName()) == mRequestLoadList.End());
				DeleteResource(pResource);
			}
			else
			{
				log_volatile(mLog.Debug(_T("Resource ")+pResource->GetName()+_T(" dereferenced. Will be deleted immediately after loader thread is done.")));
				assert(mRequestLoadList.Find(pResource->GetName()) != mRequestLoadList.End());
			}
		}
	}
	else
	{
		log_volatile(mLog.Debug(_T("Resource ")+pResource->GetName()+_T(" dereferenced, but has other references.")));
	}
}



void ResourceManager::Tick()
{
	if (mLoaderThreadCount == 0)
	{
		SynchronousLoadLoop();
	}
	InjectResourceLoop();
	FreeCache();
}

void ResourceManager::ForceFreeCache()
{
	// TODO: optimize by keeping objects in cache for a while!
	while (!mCachedResourceTable.IsEmpty())
	{
		ResourceTable::Iterator x = mCachedResourceTable.First();
		Resource* lResource = *x;
		assert(mRequestLoadList.Find(lResource->GetName()) == mRequestLoadList.End());
		mCachedResourceTable.Remove(x);
		DeleteResource(lResource);
	}
}



ResourceManager::NameTypeList ResourceManager::QueryActiveResourceNames()
{
	NameTypeList lNames;
	Lepra::ScopeLock lLock(&mThreadLock);
	ResourceTable::Iterator x = mActiveResourceTable.First();
	for (; x != mActiveResourceTable.End(); ++x)
	{
		Resource* lResource = *x;
		StringPair lPair(lResource->GetName(), lResource->GetType());
		lNames.push_back(lPair);
	}
	return (lNames);
}



Resource* ResourceManager::QueryCachedResource(const Lepra::String& pName, UserResource* pUserResource, bool& pMustLoad)
{
	pMustLoad = false;
	Resource* lResource = mActiveResourceTable.FindObject(pName);
	if (!lResource)
	{
		lResource = mCachedResourceTable.FindObject(pName);
		if (lResource)
		{
			// Resource found on the way out, move it back into the system.
			mCachedResourceTable.Remove(lResource->GetName());
			assert(mActiveResourceTable.Find(lResource->GetName()) == mActiveResourceTable.End());
			mActiveResourceTable.Insert(lResource->GetName(), lResource);
		}
	}
	if (!lResource)
	{
		assert(mRequestLoadList.Find(pName) == mRequestLoadList.End());
		lResource = CreateResource(pUserResource, pName);
		assert(mActiveResourceTable.Find(lResource->GetName()) == mActiveResourceTable.End());
		mActiveResourceTable.Insert(lResource->GetName(), lResource);
		assert(mRequestLoadList.Find(lResource->GetName()) == mRequestLoadList.End());
		pMustLoad = true;
		log_volatile(mLog.Debug(_T("Resource ")+pName+_T(" created + starts loading.")));
	}
	else
	{
		if (lResource->GetLoadState() == RESOURCE_UNLOADED)
		{
			log_volatile(mLog.Debug(_T("Resource ")+pName+_T(" will reload.")));
			pMustLoad = true;
		}
		else if (lResource->GetLoadState() == RESOURCE_LOAD_IN_PROGRESS)
		{
			log_volatile(mLog.Debug(_T("Resource ")+pName+_T(" currently loading, will use it when done.")));
		}
		else
		{
			log_volatile(mLog.Debug(_T("Resource ")+pName+_T(" already loaded, will use it instead of reloading.")));
		}
	}
	lResource->Reference();
	return (lResource);
}


void ResourceManager::InjectResourceLoop()
{
	// Perhaps-TODO: measure time. This method should exit when time is up.
	// Such a functionality would give a more stable frame rate, without horrible
	// dips from the resource system.
	ResourceMap lInjectList;
	{
		Lepra::ScopeLock lLock(&mThreadLock);
		if (mLoadedList.GetCount() > 0)
		{
			lInjectList = mLoadedList;
			mLoadedList.RemoveAll();
		}
	}
	for (ResourceMap::Iterator x = lInjectList.First(); x != lInjectList.End();)
	{
		Resource* lResource = x.GetObject();
		if (InjectSingleResource(lResource))
		{
			ResourceMap::Iterator y = x;
			++y;
			lInjectList.Remove(x);
			x = y;
		}
		else
		{
			++x;
		}
	}
	if (lInjectList.GetCount() > 0)
	{
		Lepra::ScopeLock lLock(&mThreadLock);
		for (ResourceMap::Iterator x = lInjectList.First(); x != lInjectList.End(); ++x)
		{
			mLoadedList.PushBack(x.GetKey(), x.GetObject());
		}
	}
}

bool ResourceManager::InjectSingleResource(Resource* pResource)
{
	bool lInjectDone = true;
	if (pResource->GetLoadState() == RESOURCE_LOAD_IN_PROGRESS)
	{
		ResourceLoadState lLoadState = pResource->PostProcess();
		if (lLoadState != RESOURCE_LOAD_IN_PROGRESS)
		{
			pResource->SetLoadState(lLoadState);
			lInjectDone = true;
		}
		else
		{
			lInjectDone = false;
		}
	}
	if (lInjectDone)
	{
		pResource->Callback();
	}
	return (lInjectDone);
}



void ResourceManager::FreeCache()
{
	// TODO: optimize this algoritm by keeping track of time an object has spent
	// in cache, dropping the oldest first, and so forth.
	if (Lepra::SystemManager::GetAvailRam() < Lepra::SystemManager::GetAmountRam()/4)
	{
		ForceFreeCache();
	}
}



void ResourceManager::ThreadLoaderLoop()
{
	while (!mLoaderThread.GetStopRequest())
	{
		mLoadSemaphore.Wait();
		if (!mLoaderThread.GetStopRequest())
		{
			LoadSingleResource();
		}
	}
}

void ResourceManager::SynchronousLoadLoop()
{
	while (mRequestLoadList.GetCount() > 0)
	{
		LoadSingleResource();
	}
}

bool ResourceManager::PrepareDeleteInLoadProgress(Resource* pResource)
{
	bool lAllowDelete = true;
	Lepra::ScopeLock lLock(&mThreadLock);
	if (mRequestLoadList.GetCount() > 0)
	{
		// Only the first object in the 'request load' list may be currently loading.
		if (pResource == mRequestLoadList.First().GetObject())
		{
			lAllowDelete = false;
		}
		else
		{
			// If it's in the 'request load' list, but not first, simply remove it
			// and it's safe to delete.
			mRequestLoadList.Remove(pResource->GetName());
			assert(mRequestLoadList.GetCount() < 10000);
		}
	}
	mLoadedList.Remove(pResource->GetName());
	return (lAllowDelete);
}

void ResourceManager::LoadSingleResource()
{
	// 1. Lock, retrieve resource, unlock.
	// 2. Load resource.
	// 3. Lock, move resource to loaded list, unlock.

	Resource* lResource = 0;
	{
		Lepra::ScopeLock lLock(&mThreadLock);
		if (mRequestLoadList.GetCount() > 0)
		{
			lResource = mRequestLoadList.First().GetObject();
		}
	}

	if (lResource)
	{
		log_volatile(mLog.Debugf(_T("Loading '%s'."), lResource->GetName().c_str()));
		if (!lResource->Load())
		{
			lResource->SetLoadState(RESOURCE_LOAD_ERROR);
		}
		{
			Lepra::ScopeLock lLock(&mThreadLock);
			ResourceMap::Iterator x = mRequestLoadList.Find(lResource->GetName());
			if (x == mRequestLoadList.First())
			{
				assert(mRequestLoadList.GetCount() < 10000);
				mRequestLoadList.Remove(x);
				assert(mRequestLoadList.GetCount() < 10000);
				mLoadedList.PushBack(lResource->GetName(), lResource);
			}
			else
			{
				// Resource was released while loading, but could not be deleted since this
				// thread was loading it. Therefore we simply delete it here.
				assert(mRequestLoadList.Find(lResource->GetName()) == mRequestLoadList.End());
				log_debug(_T("Deleting just loaded resource '")+lResource->GetName()+_T("'."));
				DeleteResource(lResource);
			}
		}
	}
}



Resource* ResourceManager::CreateResource(UserResource* pUserResource, const Lepra::String& pName)
{
	Resource* pResource = pUserResource->CreateResource(this, pName);
	mResourceSafeLookup.insert(pResource);
	return (pResource);
}

void ResourceManager::DeleteResource(Resource* pResource)
{
	assert(mResourceSafeLookup.find(pResource) != mResourceSafeLookup.end());
	assert(mRequestLoadList.Find(pResource->GetName()) == mRequestLoadList.End());
	mResourceSafeLookup.erase(pResource);
	delete (pResource);
}



LOG_CLASS_DEFINE(GENERAL_RESOURCES, ResourceManager);



}
