
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/ResourceManager.h"
#include <algorithm>
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../Lepra/Include/MemFile.h"
#include "../../Lepra/Include/Path.h"
#include "../../Lepra/Include/ResourceTracker.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../Lepra/Include/ZipArchive.h"
#include "../../Tbc/Include/ChunkyPhysics.h"
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
	/*else
	{
		deb_assert(false);
	}*/
	SetParentResource(0);
}

void UserResource::SafeRelease(ResourceManager* pManager)
{
	if (pManager)
	{
		pManager->SafeRelease(this);
	}
	mResource = 0;
}

void UserResource::SetParentResource(UserResource* pParentResource)
{
	if (pParentResource)
	{
		deb_assert(!mParentResource);
		deb_assert(pParentResource != this);
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

void UserResource::FinalizeLoad()
{
}



// ----------------------------------------------------------------------------



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
	log_volatile(mLog.Tracef(("Deleting resource "+mName).c_str(), LEVEL_TRACE));
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
	BusLock::Add(&mReferenceCount, 1);
	return (mReferenceCount);
}

int Resource::Dereference()
{
	BusLock::Add(&mReferenceCount, -1);
	deb_assert(mReferenceCount >= 0);
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
			lCaller.mUserResource->FinalizeLoad();
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
	deb_assert(mLoadCallbackList.size() == 1);
	if (!mLoadCallbackList.empty())
	{
		return (mLoadCallbackList.front().mUserResource);
	}
	return (0);
}

void Resource::PatchInfo(ResourceInfo&) const
{
}

void Resource::operator=(const Resource&)
{
	deb_assert(false);
}

Lock Resource::mMutex;

loginstance(GENERAL_RESOURCES, Resource);



// ----------------------------------------------------------------------------



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
	return ("Physics");
}

PhysicsResource::UserData PhysicsResource::GetUserData(const Cure::UserResource*) const
{
	return (GetRamData());
}

bool PhysicsResource::Load()
{
	return LoadName(GetName());
}

bool PhysicsResource::LoadName(const str& pName)
{
	deb_assert(GetRamData() == 0);
	SetRamData(new Tbc::ChunkyPhysics(Tbc::ChunkyPhysics::TRANSFORM_LOCAL2WORLD));
	File* lFile = GetManager()->QueryFile(pName);
	bool lOk = (lFile != 0);
	if (lOk)
	{
		Tbc::ChunkyPhysicsLoader lLoader(lFile, false);
		lOk = lLoader.Load(GetRamData());
	}
	delete lFile;
	return (lOk);
}



ClassResource::ClassResource(Cure::ResourceManager* pManager, const str& pName):
	Parent(pManager, pName)
{
}

ClassResource::~ClassResource()
{
}



// ----------------------------------------------------------------------------



PhysicalTerrainResource::PhysicalTerrainResource(ResourceManager* pManager, const str& pName):
	Parent(pManager, pName)
{
}

PhysicalTerrainResource::~PhysicalTerrainResource()
{
	// TRICKY: delete of data must lie in this class, and not in parent class RamResource.
	// This is so since types (UiLepra::SoundManager::SoundID) cannot be deleted!
	SetRamData(0);

	log_trace("Deleting terrain patch resource.");
}

const str PhysicalTerrainResource::GetType() const
{
	return ("PhysTerrain");
}

PhysicalTerrainResource::UserData PhysicalTerrainResource::GetUserData(const UserResource*) const
{
	return (GetRamData());
}

bool PhysicalTerrainResource::Load()
{
	log_trace("Loading actual Tbc::TerrainPatch.");

	deb_assert(!IsUnique());
	// TODO: parse constants out of resource name string.
	const TerrainPatchLoader::PatchArea lArea(0, 0, 0, 0);
	const float lLod = 0;

	TerrainPatchLoader lLoader(GetManager());
	Tbc::TerrainPatch* lPatch = 0;
	for (int x = 0; x < 3 && !lPatch; ++x)	// Retry file loading, file might be held by anti-virus/Windoze/similar shit.
	{
		lPatch = lLoader.LoadPatch(lArea, lLod);
	}
	if (lPatch)
	{
		SetRamData(lPatch);
	}
	return (lPatch != 0);
}

loginstance(PHYSICS, PhysicalTerrainResource);



// ----------------------------------------------------------------------------



RamImageResource::RamImageResource(Cure::ResourceManager* pManager, const str& pName):
	Parent(pManager, pName)
{
}

RamImageResource::~RamImageResource()
{
}

const str RamImageResource::GetType() const
{
	return ("RamImg");
}

RamImageResource::UserData RamImageResource::GetUserData(const Cure::UserResource*) const
{
	return (GetRamData());
}

bool RamImageResource::Load()
{
	deb_assert(!IsUnique());
	deb_assert(GetRamData() == 0);
	SetRamData(new Canvas());
	File* lFile = GetManager()->QueryFile(GetName());
	bool lOk = (lFile != 0);
	if (lOk)
	{
		ImageLoader lLoader;
		lOk = lLoader.Load(ImageLoader::GetFileTypeFromName(GetName()), *lFile, *GetRamData());
	}
	delete lFile;
	return lOk;
}



// ----------------------------------------------------------------------------



ResourceManager::ResourceManager(unsigned pLoaderThreadCount):
	mTerrainFunctionManager(0),
	mLoaderThreadCount(pLoaderThreadCount),
	mPathPrefix(SystemManager::GetDataDirectory()),
	mLoaderThread("ResourceLoader"),
	mZipFile(new ZipArchive),
	mInjectTimeLimit(0.01),
	mLoadIntermission(0)
{
	if (mZipFile->OpenArchive(mPathPrefix + "Data.pk3", ZipArchive::READ_ONLY) != IO_OK)
	{
		delete mZipFile;
		mZipFile = 0;
	}
}

ResourceManager::~ResourceManager()
{
	StopClear();
	SetTerrainFunctionManager(0);

	delete mZipFile;
	mZipFile = 0;
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

	mRequestLoadList.RemoveAll();	// We claim nothin's cookin'.

	// Iteratively free memory. First free all resources that ARE of reference type,
	// then all cure resources. Inside that top-level order, we first kill resources,
	// without any references, then those with only 1 ref, etc. This should yield
	// correct hierarchical destruction.
	bool lKillReferencesOnly = true;	// Kill all references first.
	int lRefCountThreshold = 0;
	while (!mActiveResourceTable.empty())
	{
		ForceFreeCache();	// Must be before and after active resources' are deleted (caused by hierarchical resource structures).
		bool lKilled = false;
		bool lAreReferencesLeft = false;
		ResourceTable::iterator x = mActiveResourceTable.begin();
		while (x != mActiveResourceTable.end())
		{
			Resource* lResource = x->second;
			deb_assert(mRequestLoadList.Find(lResource) == mRequestLoadList.End());
			if ((!lKillReferencesOnly || lResource->IsReferenceType()) &&
				lResource->GetReferenceCount() <= lRefCountThreshold)
			{
				x = mActiveResourceTable.erase(x);
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
	for (int x = 0; x < 10; ++x)	// Also frees base resources, which may have just had their referencing resourced freed.
	{
		ForceFreeCache();
	}

	// First kill all reference resources.
	bool lDeleteMoreReferences = true;
	while (lDeleteMoreReferences)
	{
		lDeleteMoreReferences = false;
		for (ResourceSet::iterator x = mResourceSafeLookup.begin(); x != mResourceSafeLookup.end(); ++x)
		{
			Resource* lResource = *x;
			if (lResource->IsReferenceType())
			{
				mLog.Errorf("Reference resource '%s' not freed! Going for the kill!", lResource->GetName().c_str());
				DeleteResource(lResource);
				lDeleteMoreReferences = true;
				break;
			}
		}
	}

	// Then kill the rest (non-reference resources).
	while (!mResourceSafeLookup.empty())
	{
		Resource* lResource = *mResourceSafeLookup.begin();
		mLog.Errorf("Base resource '%s' not freed! Going for the kill!", lResource->GetName().c_str());
		DeleteResource(lResource);
	}

	// These just contain pointers, they are not data owners. Thus no delete required.
	mRequestLoadList.RemoveAll();
	mLoadedList.RemoveAll();
}

void ResourceManager::SetLoadIntermission(double pLoadIntermission)
{
	mLoadIntermission = pLoadIntermission;
}

void ResourceManager::SetInjectTimeLimit(double pInjectTimeLimit)
{
	mInjectTimeLimit = pInjectTimeLimit;
}



File* ResourceManager::QueryFile(const str& pFilename)
{
	str lFilename = strutil::ReplaceAll(pFilename, '\\', '/');
	if (mZipFile)
	{
		ScopeLock lLock(&mZipLock);
		if (mZipFile->FileOpen(lFilename))
		{
			const size_t lSize = (size_t)mZipFile->FileSize();
			MemFile* lFile = new MemFile;
			if (mZipFile->FileRead(lFile->GetBuffer(lSize), lSize) != IO_OK)
			{
				delete lFile;
				lFile = 0;
			}
			mZipFile->FileClose();
			if (lFile)
			{
				lFile->SeekSet(0);
				return lFile;
			}
		}
	}

	lFilename = mPathPrefix + lFilename;
	DiskFile* lFile = new DiskFile;
	bool lOk = false;
	for (int x = 0; x < 3 && !lOk; ++x)	// Retry file open, file might be held by anti-virus/Windoze/similar shit.
	{
		lOk = lFile->Open(lFilename, DiskFile::MODE_READ);
	}
	if (lOk)
	{
		return lFile;
	}
	else
	{
		mLog.Errorf("Could not load file with name '%s'.", lFilename.c_str());
	}
	delete lFile;
	return 0;
}

bool ResourceManager::QueryFileExists(const str& pFilename)
{
	if (mZipFile)
	{
		ScopeLock lLock(&mZipLock);
		if (mZipFile->FileExist(pFilename))
		{
			return true;
		}
	}
	const str lFilename = mPathPrefix + strutil::ReplaceAll(pFilename, '\\', '/');
	return DiskFile::Exists(lFilename);
}

strutil::strvec ResourceManager::ListFiles(const str& pWildcard)
{
	strutil::strvec lFilenameArray;
	if (mZipFile)
	{
		ScopeLock lLock(&mZipLock);
		str lFilename;
		for (lFilename = mZipFile->FileFindFirst(); !lFilename.empty(); lFilename = mZipFile->FileFindNext())
		{
			if (Path::IsWildcardMatch(pWildcard, lFilename))
			{
				lFilenameArray.push_back(lFilename);
			}
		}
		return lFilenameArray;
	}

	const str lWildcard = mPathPrefix + strutil::ReplaceAll(pWildcard, '\\', '/');
	DiskFile::FindData lInfo;
	for (bool lOk = DiskFile::FindFirst(lWildcard, lInfo); lOk; lOk = DiskFile::FindNext(lInfo))
	{
		lFilenameArray.push_back(lInfo.GetName());
	}
	return lFilenameArray;
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

void ResourceManager::AddLoaded(UserResource* pUserResource)
{
	deb_assert(pUserResource->GetLoadState() != RESOURCE_LOAD_ERROR);
	ScopeLock lLock(&mThreadLock);
	Resource* lResource = pUserResource->GetResource();
	lResource->Reference();
	mResourceSafeLookup.insert(lResource);
	mActiveResourceTable.insert(ResourceTable::value_type(lResource->GetName(), lResource));
	mLoadedList.PushBack(lResource, lResource);
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
		else
		{
			deb_assert(false);
		}
	}
	else
	{
		deb_assert(false);
	}
}

void ResourceManager::Release(Resource* pResource)
{
	if (pResource->Dereference() <= 0)
	{
		deb_assert(pResource->GetReferenceCount() == 0);
		ScopeLock lMutex(&mThreadLock);

		if (!pResource->IsUnique())
		{
			mActiveResourceTable.erase(pResource->GetName());
			if (pResource->GetLoadState() == RESOURCE_LOAD_COMPLETE)
			{
				// A completely loaded resource dropped, place it "on the way out" of the system.
				mCachedResourceTable.insert(ResourceTable::value_type(pResource->GetName(), pResource));
				log_volatile(mLog.Trace("Loaded resource "+pResource->GetName()+" dereferenced. Placed in cache."));
				pResource->Suspend();
			}
			else
			{
				if (PrepareRemoveInLoadProgress(pResource))
				{
					log_volatile(mLog.Trace("Incomplete resource "+pResource->GetName()+" dereferenced. Not cached - deleted immediately."));
					deb_assert(mRequestLoadList.Find(pResource) == mRequestLoadList.End());
					DeleteResource(pResource);
				}
				else
				{
					log_volatile(mLog.Trace("Currently loading resource "+pResource->GetName()+" dereferenced. Not cached - will be deleted immediately after loader thread is done."));
					deb_assert(mRequestLoadList.Find(pResource) == mRequestLoadList.End());
				}
			}
		}
		else
		{
			mActiveResourceTable.erase(pResource->GetName());	// TRICKY: this is for shared resources that are not to be kept any more when dereferenced.
			if (PrepareRemoveInLoadProgress(pResource))
			{
				log_volatile(mLog.Trace("Resource "+pResource->GetName()+" (unique) dereferenced. Deleted immediately."));
				deb_assert(mRequestLoadList.Find(pResource) == mRequestLoadList.End());
				DeleteResource(pResource);
			}
			else
			{
				log_volatile(mLog.Trace("Resource "+pResource->GetName()+" (unique) dereferenced. Will be deleted immediately after loader thread is done."));
				deb_assert(mRequestLoadList.Find(pResource) == mRequestLoadList.End());
			}
		}
	}
	else
	{
		log_volatile(mLog.Trace("Resource "+pResource->GetName()+" dereferenced, but has other references."));
	}
}

bool ResourceManager::IsLoading()
{
	ScopeLock lLock(&mThreadLock);
	return (mRequestLoadList.GetCount() || mLoadedList.GetCount());
}

bool ResourceManager::WaitLoading()
{
	for (int x = 0; IsLoading() && SystemManager::GetQuitRequest() == 0 && x < 200; ++x)
	{
		Thread::Sleep(0.1);
	}
	return !IsLoading();
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
	return ForceFreeCache(strutil::strvec());
}

unsigned ResourceManager::ForceFreeCache(const strutil::strvec& pResourceTypeList)
{
	ScopeLock lLock(&mThreadLock);
	// TODO: optimize by keeping objects in cache for a while!

	/*mLog.Headline("ForceFreeCache...");
	ResourceTable::iterator x = mCachedResourceTable.begin();
	for (; x != mCachedResourceTable.end(); ++x)
	{
		mLog.Headlinef("  - %s @ %p.", (*x)->GetName().c_str(), *x);
	}
	mLog.Headline("---------------");*/
	int lPrevDroppedResourceCount = -1;
	unsigned lDroppedResourceCount = 0;
	while (lPrevDroppedResourceCount != lDroppedResourceCount)
	{
		lPrevDroppedResourceCount = lDroppedResourceCount;
		ResourceTable::iterator x = mCachedResourceTable.begin();
		while (x != mCachedResourceTable.end())
		{
			Resource* lResource = x->second;
			deb_assert(mRequestLoadList.Find(lResource) == mRequestLoadList.End());
			bool lDrop = pResourceTypeList.empty();
			const str& lType = lResource->GetType();
			strutil::strvec::const_iterator y = pResourceTypeList.begin();
			for (; !lDrop && y != pResourceTypeList.end(); ++y)
			{
				lDrop = (lType == *y);
			}
			if (lDrop)
			{
				x = mCachedResourceTable.erase(x);
				DeleteResource(lResource);
				++lDroppedResourceCount;
				break;
			}
			else
			{
				++x;
			}
		}
	}
	return lDroppedResourceCount;
}



size_t ResourceManager::QueryResourceCount() const
{
	ScopeLock lLock(&mThreadLock);
	return (mResourceSafeLookup.size());
}

size_t ResourceManager::QueryCachedResourceCount() const
{
	ScopeLock lLock(&mThreadLock);
	return (mCachedResourceTable.size());
}

ResourceManager::ResourceInfoList ResourceManager::QueryResourceNames()
{
	ResourceInfoList lNames;
	ScopeLock lLock(&mThreadLock);
	ResourceSet::iterator x = mResourceSafeLookup.begin();
	for (; x != mResourceSafeLookup.end(); ++x)
	{
		Resource* lResource = *x;
		ResourceInfo lInfo;
		lInfo.mName = lResource->GetName();
		lInfo.mType = lResource->GetType();
		lInfo.mReferenceCount = lResource->GetReferenceCount();
		lResource->PatchInfo(lInfo);
		lNames.push_back(lInfo);
	}
	return lNames;
}

ResourceManager::ResourceList ResourceManager::HookAllResourcesOfType(const str& pType)
{
	ResourceList lList;
	mThreadLock.Acquire();
	ResourceSet::iterator x = mResourceSafeLookup.begin();
	for (; x != mResourceSafeLookup.end(); ++x)
	{
		Resource* lResource = *x;
		if (strutil::StartsWith(lResource->GetType(), pType))
		{
			lResource->Reference();
			lList.push_back(lResource);
		}
	}
	return lList;
}

void ResourceManager::UnhookResources(ResourceList& pResourceList)
{
	ResourceList::iterator x = pResourceList.begin();
	for (; x != pResourceList.end(); ++x)
	{
		Resource* lResource = *x;
		lResource->Dereference();
	}
	pResourceList.clear();
	mThreadLock.Release();
}



Resource* ResourceManager::GetAddCachedResource(const str& pName, UserResource* pUserResource, bool& pMustLoad)
{
	pMustLoad = false;
	Resource* lResource = 0;
	ResourceTable::iterator x = mActiveResourceTable.find(pName);
	if (x != mActiveResourceTable.end())
	{
		lResource = x->second;
	}
	else
	{
		ResourceTable::iterator x = mCachedResourceTable.find(pName);
		if (x != mCachedResourceTable.end())
		{
			lResource = x->second;
			// Resource found on the way out, move it back into the system.
			mCachedResourceTable.erase(lResource->GetName());
			deb_assert(mActiveResourceTable.find(lResource->GetName()) == mActiveResourceTable.end());
			deb_assert(lResource->GetLoadState() == RESOURCE_LOAD_COMPLETE);
			mActiveResourceTable.insert(ResourceTable::value_type(lResource->GetName(), lResource));
			lResource->Resume();
		}
	}
	if (!lResource)
	{
		//deb_assert(mRequestLoadList.Find(pName) == mRequestLoadList.End());
		lResource = CreateResource(pUserResource, pName);
		deb_assert(mActiveResourceTable.find(lResource->GetName()) == mActiveResourceTable.end());
		deb_assert(lResource->GetLoadState() == RESOURCE_LOAD_ERROR);
		mActiveResourceTable.insert(ResourceTable::value_type(lResource->GetName(), lResource));
		deb_assert(mRequestLoadList.Find(lResource) == mRequestLoadList.End());
		pMustLoad = true;
		log_volatile(mLog.Trace("Resource "+pName+" created + starts loading."));
	}
	else
	{
		if (lResource->GetLoadState() == RESOURCE_UNLOADED)
		{
			log_volatile(mLog.Trace("Resource "+pName+" will reload."));
			pMustLoad = true;
		}
		else if (lResource->GetLoadState() == RESOURCE_LOAD_IN_PROGRESS)
		{
			log_volatile(mLog.Debug("Resource "+pName+" currently loading, will use it when done."));
		}
		else
		{
			log_volatile(mLog.Trace("Resource "+pName+" already loaded, will use it instead of reloading."));
		}
	}
	//deb_assert(!lResource->IsUnique());
	lResource->Reference();
	return (lResource);
}

void ResourceManager::StartLoad(Resource* pResource)
{
	AssertIsMutexOwner();

	// Pass on to loader thread.
	deb_assert(pResource->GetReferenceCount() > 0);
	pResource->SetLoadState(RESOURCE_LOAD_IN_PROGRESS);
	deb_assert(mRequestLoadList.GetCount() < 10000);	// Just run GetCount() to validate internal integrity.
	log_volatile(const str& lName = pResource->GetName());
	log_volatile(mLog.Tracef("Requesting load of '%s' (%s).", lName.c_str(), pResource->GetType().c_str()));
	deb_assert(mRequestLoadList.Find(pResource) == mRequestLoadList.End());
	mRequestLoadList.PushBack(pResource, pResource);
	mLoadSemaphore.Signal();
}



void ResourceManager::InjectResourceLoop()
{
	HiResTimer lTimer(false);

	// ---------------------------
	// NOTE: this lock must be here, as the state otherwise changes outside of the lock state, which causes all sorts
	//       of problems, especially when deleting just loaded resources which have not yet been postprocessed. Also,
	//       there is no way to stop injection once the resource has been postprocessed by the injection thread.
	ScopeLock lLock(&mThreadLock);
	// ---------------------------

	for (ResourceMapList::Iterator x = mLoadedList.First(); x != mLoadedList.End();)
	{
		Resource* lResource = x.GetObject();
		if (InjectSingleResource(lResource))
		{
			ResourceMapList::Iterator y = x;
			++y;
			mLoadedList.Remove(x);
			x = y;
		}
		else
		{
			++x;
		}
		if (lTimer.QueryTimeDiff() > mInjectTimeLimit)	// Time's up, have a go later.
		{
			break;
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
		// Smooth out loads, so all the heavy lifting won't end up in a single frame.
		size_t lCount = 0;
		{
			ScopeLock lLock(&mThreadLock);
			lCount = mRequestLoadList.GetCount();
		}
		if (lCount > 0 && lCount < 50)
		{
			Thread::Sleep(mLoadIntermission);
		}
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
		// This is a bit complex, but the resource can enter the "loaded list", which it does when it's
		// "Load" method is first called. However, it may still depend asynchronously on other resources
		// which are currently loading. So it keeps its "load in progress" state, but remains in the
		// "loaded list".
		//deb_assert(mRequestLoadList.Exists(pResource) && !mLoadedList.Exists(pResource));

		// Only the first object in the 'request load' list may be currently loading.
		if (mRequestLoadList.GetCount() > 0 && pResource == mRequestLoadList.First().GetObject())
		{
			lAllowDelete = false;
			mPostLoadDeleteArray.push_back(pResource);
			mLoadSemaphore.Signal();	// Kick it, this may be a recursive delete request.
		}
		else
		{
			// If it's in the 'request load' list, but not first, simply remove it
			// and it's safe to delete.
			deb_assert(mRequestLoadList.Find(pResource) != mRequestLoadList.End() ||
				mLoadedList.Find(pResource) != mLoadedList.End());
		}
		mRequestLoadList.Remove(pResource);
	}
	deb_assert(mRequestLoadList.GetCount() < 10000);
	mLoadedList.Remove(pResource);
	deb_assert(mLoadedList.GetCount() < 10000);
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
		deb_assert(lResource->GetLoadState() == RESOURCE_LOAD_IN_PROGRESS);
		log_volatile(mLog.Tracef("Loading %s with %i resources in list (inclusive).",
			lResource->GetName().c_str(), lListCount));
		const bool lIsLoaded = lResource->Load();
		deb_assert(lResource->GetLoadState() == RESOURCE_LOAD_IN_PROGRESS);
		if (!lIsLoaded)
		{
			lResource->SetLoadState(RESOURCE_LOAD_ERROR);
		}
		{
			ScopeLock lLock(&mThreadLock);
			if (!mRequestLoadList.IsEmpty() && lResource == mRequestLoadList.First().GetObject())
			{
				deb_assert(mRequestLoadList.GetCount() < 10000);
				mRequestLoadList.Remove(lResource);
				deb_assert(mRequestLoadList.GetCount() < 10000);
				mLoadedList.PushBack(lResource, lResource);
			}
			else
			{
				// Resource was released while loading, but could not be deleted since this
				// thread was loading it. Therefore we delete it here. No use trying to cache
				// this resource, since it needs both Suspend() and PostProcess(); in addition
				// the last call must also come from the main thread.
				deb_assert(mRequestLoadList.Find(lResource) == mRequestLoadList.End());
				deb_assert(lResource->GetReferenceCount() == 0);
				mLog.Info("Deleting just loaded resource '"+lResource->GetName()+"'.");
				DeleteResource(lResource);
			}

			// Delete any remaining (usually depending-upon) resources.
			while (!mPostLoadDeleteArray.empty())
			{
				DeleteResource(mPostLoadDeleteArray.back());
			}
		}
	}
	else
	{
		ScopeLock lLock(&mThreadLock);
		// Delete any remaining (usually depending-upon) resources.
		while (!mPostLoadDeleteArray.empty())
		{
			DeleteResource(mPostLoadDeleteArray.back());
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

	deb_assert(mResourceSafeLookup.find(pResource) != mResourceSafeLookup.end());
	deb_assert(mRequestLoadList.Find(pResource) == mRequestLoadList.End());
	mResourceSafeLookup.erase(pResource);

	// Drop resource from post load delete array.
	mPostLoadDeleteArray.erase(std::remove(mPostLoadDeleteArray.begin(), mPostLoadDeleteArray.end(), pResource), mPostLoadDeleteArray.end());

	{
		const int lLockCount = mThreadLock.GetReferenceCount();
		for (int i = 0; i < lLockCount; ++i) mThreadLock.Release();	// Required since we may need other system-wide locks in destructor.
		deb_assert(!mThreadLock.IsOwner());
		delete (pResource);
		for (int i = 0; i < lLockCount; ++i) mThreadLock.Acquire();
	}
}



loginstance(GENERAL_RESOURCES, ResourceManager);



}
