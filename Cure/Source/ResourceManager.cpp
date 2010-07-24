
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/ResourceManager.h"
#include <assert.h>
#include "../../Lepra/Include/HiResTimer.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../Include/ContextObject.h"
#include "../Include/Cure.h"
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

const str& UserResource::GetName() const
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



Resource::Resource(ResourceManager* pManager, const str& pName):
	mManager(pManager),
	mName(pName),
	mReferenceCount(0),
	mState(RESOURCE_LOAD_ERROR),
	mIsUnique(false)
{
}

Resource::~Resource()
{
	mState = RESOURCE_LOAD_ERROR;
	log_volatile(mLog.Debugf((_T("Deleting resource ")+mName).c_str(), Log::LEVEL_TRACE));
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

const str& Resource::GetName() const
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
	assert(mReferenceCount >= 0);
	return (mReferenceCount);
}

int Resource::GetReferenceCount() const
{
	return (mReferenceCount);
}

void Resource::Resume()
{
}

void Resource::Suspend()
{
}

bool Resource::IsUnique() const
{
	return (mIsUnique);
}

void Resource::SetIsUnique(bool pIsUnique)
{
	mIsUnique = pIsUnique;
}

bool Resource::IsReferenceType() const
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
	ScopeLock lMutex(&mMutex);
	mLoadCallbackList.push_back(UserResourceCallbackInfo(pUserResource, pCallback));
}

void Resource::RemoveCaller(UserResource* pUserResource)
{
	ScopeLock lMutex(&mMutex);

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
	CallbackList lCallbackList;
	{
		ScopeLock lMutex(&mMutex);
		lCallbackList = mLoadCallbackList;
	}

	CallbackList::iterator x = lCallbackList.begin();
	for (; x != lCallbackList.end(); ++x)
	{
		UserResourceCallbackInfo& lCaller = *x;
		lCaller.mUserResource->PostProcess();
	}
}

void Resource::Callback()
{
	Reference();	// Is here to stop a callback from dereferencing+deleting this resource.

	CallbackList lCallbackList;
	{
		ScopeLock lMutex(&mMutex);
		lCallbackList.splice(lCallbackList.begin(), mLoadCallbackList);
	}
	size_t lCallbackCount = lCallbackList.size();
	for (size_t x = 0; x < lCallbackCount; ++x)
	{
		UserResourceCallbackInfo lCaller = *lCallbackList.begin();
		lCallbackList.pop_front();
		if (lCaller.mUserResource->DecreaseCallbackBlockCount() > 0)
		{
			// Move to back of list = don't call yet.
			lCallbackList.push_back(lCaller);
		}
		else
		{
			//const int lPreviousCount = (int)lCallbackList.size();
			ResourceLoadState lLoadState = lCaller.mUserResource->GetResource()->GetLoadState();
			lCaller.mCallback(lCaller.mUserResource);
			lCaller.mUserResource->CallbackParent(lLoadState);
			/*// Skip the once deleted during the parent's callback.
			const int lCurrentCount = (int)lCallbackList.size();
			if (lCurrentCount < lPreviousCount)
			{
				lCallbackCount -= lPreviousCount-lCurrentCount;
			}*/
		}
	}
	if (lCallbackList.size() > 0)
	{
		ScopeLock lMutex(&mMutex);
		mLoadCallbackList.splice(mLoadCallbackList.begin(), lCallbackList);
	}

	mManager->Release(this);
}

void Resource::FreeDiversified(UserResource*)
{
}

UserResource* Resource::GetFirstUserResource() const
{
	assert(mLoadCallbackList.size() == 1);
	if (!mLoadCallbackList.empty())
	{
		return (mLoadCallbackList.front().mUserResource);
	}
	return (0);
}

void Resource::operator=(const Resource&)
{
	assert(false);
}

Lock Resource::mMutex;

LOG_CLASS_DEFINE(GENERAL_RESOURCES, Resource);



PhysicsResource::PhysicsResource(Cure::ResourceManager* pManager, const str& pName):
	Parent(pManager, pName)
{
}

PhysicsResource::~PhysicsResource()
{
	SetRamData(0);
}

const str PhysicsResource::GetType() const
{
	return (_T("Physics"));
}

PhysicsResource::UserData PhysicsResource::GetUserData(const Cure::UserResource*) const
{
	return (GetRamData());
}

bool PhysicsResource::Load()
{
	assert(IsUnique());
	assert(GetRamData() == 0);
	SetRamData(new TBC::ChunkyPhysics(TBC::ChunkyPhysics::TRANSFORM_LOCAL2WORLD));
	DiskFile lFile;
	bool lOk = lFile.Open(GetName(), DiskFile::MODE_READ);
	if (lOk)
	{
		TBC::ChunkyPhysicsLoader lLoader(&lFile, false);
		lOk = lLoader.Load(GetRamData());
	}
	return (lOk);
}



ClassResource::ClassResource(Cure::ResourceManager* pManager, const str& pName):
	Parent(pManager, pName)
{
}

ClassResource::~ClassResource()
{
}



/*ContextObjectResource::ContextObjectResource(ResourceManager* pManager, const str& pName):
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

const str ContextObjectResource::GetType() const
{
	return (_T("Context"));
}

ContextObjectResource::UserData ContextObjectResource::GetUserData(const UserResource*) const
{
	return (GetRamData());
}

bool ContextObjectResource::Load()
{
	log_atrace("Loading context object (self).");

	assert(!IsUnique());
	assert(GetRamData() == 0);
	ContextObject* lObject = GetManager()->GetContextObjectFactory()->Create(GetName());
	SetRamData(lObject);
	return (lObject != 0);
}

ResourceLoadState ContextObjectResource::PostProcess()
{
	log_atrace("Loading context object (group).");
	ResourceLoadState lLoadState = GetManager()->GetContextObjectFactory()->CreatePhysics(GetRamData()) ?
		RESOURCE_LOAD_COMPLETE : RESOURCE_LOAD_ERROR;
	if (lLoadState == RESOURCE_LOAD_COMPLETE)
	{
		Parent::PostProcess();
	}
	return (lLoadState);
}

LOG_CLASS_DEFINE(GAME_CONTEXT, ContextObjectResource);*/



PhysicalTerrainResource::PhysicalTerrainResource(ResourceManager* pManager, const str& pName):
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

const str PhysicalTerrainResource::GetType() const
{
	return (_T("PhysTerrain"));
}

PhysicalTerrainResource::UserData PhysicalTerrainResource::GetUserData(const UserResource*) const
{
	return (GetRamData());
}

bool PhysicalTerrainResource::Load()
{
	log_atrace("Loading actual TBC::TerrainPatch.");

	assert(!IsUnique());
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
	mTerrainFunctionManager(0),
	mLoaderThreadCount(pLoaderThreadCount),
	mLoaderThread(_T("ResourceLoader"))
{
}

ResourceManager::~ResourceManager()
{
	StopClear();
	SetTerrainFunctionManager(0);
}

bool ResourceManager::InitDefault()
{
	// Resources below deleted in the resource manager destructor.
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

	ScopeLock lLock(&mThreadLock);	// Just here for mutex lock verification.

	// Iteratively free memory. First free all resources that ARE of reference type,
	// then all cure resources. Inside that top-level order, we first kill resources,
	// without any references, then those with only 1 ref, etc. This should yield
	// correct hierarchical destruction.
	bool lKillReferencesOnly = true;	// Kill all references first.
	int lRefCountThreshold = 0;
	while (!mActiveResourceTable.IsEmpty())
	{
		ForceFreeCache();	// Must be before and after active resources' are deleted (caused by hierarchical resource structures).
		bool lKilled = false;
		bool lAreReferencesLeft = false;
		ResourceTable::Iterator x = mActiveResourceTable.First();
		while (x != mActiveResourceTable.End())
		{
			Resource* lResource = *x;
			assert(mRequestLoadList.Find(lResource) == mRequestLoadList.End());
			if ((!lKillReferencesOnly || lResource->IsReferenceType()) &&
				lResource->GetReferenceCount() <= lRefCountThreshold)
			{
				mActiveResourceTable.Remove(x++);
				// Check that no-one else has deleted our resource.
				if (mResourceSafeLookup.find(lResource) != mResourceSafeLookup.end())
				{
					if (lResource->IsReferenceType())
					{
						--lRefCountThreshold;
					}
					DeleteResource(lResource);
					lKilled = true;
				}
			}
			else
			{
				lAreReferencesLeft |= lResource->IsReferenceType();
				++x;
			}
		}
		if (lKillReferencesOnly && !lAreReferencesLeft)
		{
			lKillReferencesOnly = false;
			lRefCountThreshold = 0;
		}
		else if (!lKilled)
		{
			++lRefCountThreshold;
		}
	}
	ForceFreeCache();

	while (!mResourceSafeLookup.empty())
	{
		Resource* lResource = *mResourceSafeLookup.begin();
		mLog.Errorf(_T("Resource '%s' not freed! Going for the kill!"), lResource->GetName().c_str());
		DeleteResource(lResource);
	}

	// These just contain pointers, they are not data owners. Thus no delete required.
	mRequestLoadList.RemoveAll();
	mLoadedList.RemoveAll();
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



void ResourceManager::Load(const str& pName, UserResource* pUserResource, UserResource::LoadCallback pCallback)
{
	Resource* lResource;
	{
		bool lMustLoad = true;
		ScopeLock lLock(&mThreadLock);
		lResource = GetAddCachedResource(pName, pUserResource, lMustLoad);
		pUserResource->SetResource(lResource);
		lResource->AddCaller(pUserResource, pCallback);
		if (lMustLoad)
		{
			StartLoad(lResource);
			return;	// TRICKY: RAII simplifies.
		}
	}

	// Resource is already loaded.
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

void ResourceManager::LoadUnique(const str& pName, UserResource* pUserResource, UserResource::LoadCallback pCallback)
{
	ScopeLock lLock(&mThreadLock);
	Resource* lResource = CreateResource(pUserResource, pName);
	lResource->SetIsUnique(true);
	pUserResource->SetResource(lResource);
	lResource->AddCaller(pUserResource, pCallback);
	lResource->Reference();
	StartLoad(lResource);
}

bool ResourceManager::IsCreated(const str& pName) const
{
	return (mActiveResourceTable.FindObject(pName) != 0);
}

void ResourceManager::SafeRelease(UserResource* pUserResource)
{
	Resource* lResource = pUserResource->GetResource();
	if (lResource)
	{
		pUserResource->SetResource(0);

		ScopeLock lLock(&mThreadLock);
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
		ScopeLock lMutex(&mThreadLock);

		if (!pResource->IsUnique())
		{
			mActiveResourceTable.Remove(pResource->GetName());
			if (pResource->GetLoadState() == RESOURCE_LOAD_COMPLETE)
			{
				// A completely loaded resource dropped, place it "on the way out" of the system.
				mCachedResourceTable.Insert(pResource->GetName(), pResource);
				log_volatile(mLog.Debug(_T("Loaded resource ")+pResource->GetName()+_T(" dereferenced. Placed in cache.")));
				pResource->Suspend();
			}
			else
			{
				if (PrepareRemoveInLoadProgress(pResource))
				{
					log_volatile(mLog.Debug(_T("Incomplete resource ")+pResource->GetName()+_T(" dereferenced. Not cached - deleted immediately.")));
					assert(mRequestLoadList.Find(pResource) == mRequestLoadList.End());
					DeleteResource(pResource);
				}
				else
				{
					log_volatile(mLog.Debug(_T("Currently loading resource ")+pResource->GetName()+_T(" dereferenced. Not cached - will be deleted immediately after loader thread is done.")));
					assert(mRequestLoadList.Find(pResource) == mRequestLoadList.First());
				}
			}
		}
		else
		{
			if (PrepareRemoveInLoadProgress(pResource))
			{
				log_volatile(mLog.Debug(_T("Resource ")+pResource->GetName()+_T(" (unique) dereferenced. Deleted immediately.")));
				assert(mRequestLoadList.Find(pResource) == mRequestLoadList.End());
				DeleteResource(pResource);
			}
			else
			{
				log_volatile(mLog.Debug(_T("Resource ")+pResource->GetName()+_T(" (unique) dereferenced. Will be deleted immediately after loader thread is done.")));
				assert(mRequestLoadList.Find(pResource) == mRequestLoadList.First());
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

unsigned ResourceManager::ForceFreeCache()
{
	ScopeLock lLock(&mThreadLock);
	// TODO: optimize by keeping objects in cache for a while!

	/*mLog.AHeadline("ForceFreeCache...");
	ResourceTable::Iterator x = mCachedResourceTable.First();
	for (; x != mCachedResourceTable.End(); ++x)
	{
		mLog.Headlinef(_T("  - %s @ %p."), (*x)->GetName().c_str(), *x);
	}
	mLog.AHeadline("---------------");*/
	const unsigned lDroppedResourceCount = mCachedResourceTable.GetCount();
	while (!mCachedResourceTable.IsEmpty())
	{
		ResourceTable::Iterator x = mCachedResourceTable.First();
		/*mLog.Headlinef(_T("  %p:"), *x);
		mLog.Headlinef(_T("  - %s."), (*x)->GetName().c_str());*/
		Resource* lResource = *x;
		assert(mRequestLoadList.Find(lResource) == mRequestLoadList.End());
		mCachedResourceTable.Remove(x);
		DeleteResource(lResource);
	}
	return (lDroppedResourceCount);
}



size_t ResourceManager::QueryResourceCount() const
{
	ScopeLock lLock(&mThreadLock);
	return (mResourceSafeLookup.size());
}

size_t ResourceManager::QueryCachedResourceCount() const
{
	ScopeLock lLock(&mThreadLock);
	return (mCachedResourceTable.GetCount());
}

ResourceManager::NameTypeList ResourceManager::QueryResourceNames()
{
	NameTypeList lNames;
	ScopeLock lLock(&mThreadLock);
	ResourceSet::iterator x = mResourceSafeLookup.begin();
	for (; x != mResourceSafeLookup.end(); ++x)
	{
		Resource* lResource = *x;
		StringPair lPair(lResource->GetName(), lResource->GetType());
		lNames.push_back(lPair);
	}
	return (lNames);
}



Resource* ResourceManager::GetAddCachedResource(const str& pName, UserResource* pUserResource, bool& pMustLoad)
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
			lResource->Resume();
		}
	}
	if (!lResource)
	{
		//assert(mRequestLoadList.Find(pName) == mRequestLoadList.End());
		lResource = CreateResource(pUserResource, pName);
		assert(mActiveResourceTable.Find(lResource->GetName()) == mActiveResourceTable.End());
		mActiveResourceTable.Insert(lResource->GetName(), lResource);
		assert(mRequestLoadList.Find(lResource) == mRequestLoadList.End());
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
	assert(!lResource->IsUnique());
	lResource->Reference();
	return (lResource);
}

void ResourceManager::StartLoad(Resource* pResource)
{
	AssertIsMutexOwner();

	// Pass on to loader thread.
	assert(pResource->GetReferenceCount() > 0);
	pResource->SetLoadState(RESOURCE_LOAD_IN_PROGRESS);
	assert(mRequestLoadList.GetCount() < 10000);	// Just run GetCount() to validate internal integrity.
	log_volatile(const str& lName = pResource->GetName());
	log_volatile(mLog.Debugf(_T("Requesting load of '%s' (%s)."), lName.c_str(), pResource->GetType().c_str()));
	assert(mRequestLoadList.Find(pResource) == mRequestLoadList.End());
	mRequestLoadList.PushBack(pResource, pResource);
	mLoadSemaphore.Signal();
}



void ResourceManager::InjectResourceLoop()
{
	HiResTimer lTimer;
	ResourceMapList lInjectList;
	{
		ScopeLock lLock(&mThreadLock);
		if (mLoadedList.GetCount() > 0)
		{
			lInjectList = mLoadedList;
			mLoadedList.RemoveAll();
		}
	}
	for (ResourceMapList::Iterator x = lInjectList.First(); x != lInjectList.End();)
	{
		Resource* lResource = x.GetObject();
		if (InjectSingleResource(lResource))
		{
			ResourceMapList::Iterator y = x;
			++y;
			lInjectList.Remove(x);
			x = y;
		}
		else
		{
			++x;
		}
		if (lTimer.QueryTimeDiff() > 0.01)	// Time's up, have a go later.
		{
			break;
		}
	}
	if (lInjectList.GetCount() > 0)
	{
		ScopeLock lLock(&mThreadLock);
		for (ResourceMapList::Iterator x = lInjectList.First(); x != lInjectList.End(); ++x)
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
	if (SystemManager::GetAvailRam() < SystemManager::GetAmountRam()/4)
	{
		ForceFreeCache();
	}
}



void ResourceManager::ThreadLoaderLoop()
{
	while (!mLoaderThread.GetStopRequest())
	{
		LoadSingleResource();
		mLoadSemaphore.Wait();
	}
}

void ResourceManager::SynchronousLoadLoop()
{
	while (mRequestLoadList.GetCount() > 0)
	{
		LoadSingleResource();
	}
}

bool ResourceManager::PrepareRemoveInLoadProgress(Resource* pResource)
{
	AssertIsMutexOwner();

	// We assume that only the main thread is at work. If more threads are working
	// the system, the following race condition can and will happen:
	//
	//   1.          Thread A:  starts loading resource R.
	//   2. Resource thread T:  loads resource R and places in LOADED list.
	//   3.          Thread A:  ticks the resource, picking it out of the LOADED list, RM now unlocked.
	//   4.          Thread B:  tries to destroy resource R in loading state, but cannot find it in LOADED
	//                          nor REQUEST_LOADED lists.
	bool lAllowDelete = true;
	if (pResource->GetLoadState() == RESOURCE_LOAD_IN_PROGRESS)
	{
		assert(mRequestLoadList.GetCount() > 0 || mLoadedList.GetCount() > 0);

		// Only the first object in the 'request load' list may be currently loading.
		if (mRequestLoadList.GetCount() > 0 && pResource == mRequestLoadList.First().GetObject())
		{
			lAllowDelete = false;
		}
		else
		{
			// If it's in the 'request load' list, but not first, simply remove it
			// and it's safe to delete.
			assert(mRequestLoadList.Find(pResource) != mRequestLoadList.End() ||
				mLoadedList.Find(pResource) != mLoadedList.End());
			mRequestLoadList.Remove(pResource);
		}
		assert(mRequestLoadList.GetCount() < 10000);
	}
	mLoadedList.Remove(pResource);
	return (lAllowDelete);
}

void ResourceManager::LoadSingleResource()
{
	// 1. Lock, retrieve resource, unlock.
	// 2. Load resource.
	// 3. Lock, move resource to loaded list, unlock.

	size_t lListCount = 0;
	Resource* lResource = 0;
	{
		ScopeLock lLock(&mThreadLock);
		lListCount = mRequestLoadList.GetCount();
		if (lListCount > 0)
		{
			lResource = mRequestLoadList.First().GetObject();
		}
	}

	if (lResource)
	{
		assert(lResource->GetLoadState() == RESOURCE_LOAD_IN_PROGRESS);
		log_volatile(mLog.Debugf(_T("Loading %s with %i resources in list (inclusive)."),
			lResource->GetName().c_str(), lListCount));
		const bool lIsLoaded = lResource->Load();
		assert(lResource->GetLoadState() == RESOURCE_LOAD_IN_PROGRESS);
		if (!lIsLoaded)
		{
			lResource->SetLoadState(RESOURCE_LOAD_ERROR);
		}
		{
			ScopeLock lLock(&mThreadLock);
			if (lResource == mRequestLoadList.First().GetObject())
			{
				assert(mRequestLoadList.GetCount() < 10000);
				mRequestLoadList.Remove(lResource);
				assert(mRequestLoadList.GetCount() < 10000);
				mLoadedList.PushBack(lResource, lResource);
			}
			else
			{
				// Resource was released while loading, but could not be deleted since this
				// thread was loading it. Therefore we delete it here. No use trying to cache
				// this resource, since it needs both Suspend() and PostProcess(); in addition
				// the last call must also come from the main thread.
				assert(mRequestLoadList.Find(lResource) == mRequestLoadList.End());
				log_debug(_T("Deleting just loaded resource '")+lResource->GetName()+_T("' (unique)."));
				DeleteResource(lResource);
			}
		}
	}
}



Resource* ResourceManager::CreateResource(UserResource* pUserResource, const str& pName)
{
	AssertIsMutexOwner();

	Resource* pResource = pUserResource->CreateResource(this, pName);
	mResourceSafeLookup.insert(pResource);
	return (pResource);
}

void ResourceManager::DeleteResource(Resource* pResource)
{
	AssertIsMutexOwner();

	assert(mResourceSafeLookup.find(pResource) != mResourceSafeLookup.end());
	assert(mRequestLoadList.Find(pResource) == mRequestLoadList.End());
	mResourceSafeLookup.erase(pResource);
	delete (pResource);
}



LOG_CLASS_DEFINE(GENERAL_RESOURCES, ResourceManager);



}
