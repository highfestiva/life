
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#ifndef CURE_TEST_WITHOUT_UI

#include <assert.h>
#include <list>
#include "../../Cure/Include/NetworkClient.h"
#include "../../Cure/Include/NetworkServer.h"
#include "../../Cure/Include/Packet.h"
#include "../../Cure/Include/PositionalData.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../../TBC/Include/PhysicsManagerFactory.h"
#include "../../UiLepra/Include/UiCore.h"
#include "../../UiLepra/Include/UiDisplayManager.h"
#include "../../UiLepra/Include/UiInput.h"
#include "../../UiTbc/Include/UiBasicMeshCreator.h"
#include "../../UiTbc/Include/UiRendererFactory.h"
#include "../../UiTbc/Include/UiTriangleBasedGeometry.h"



const int CLIENT_COUNT = 2;
const float PHYSICS_FPS = 60.0f;
const float PHYSICS_SECONDS_PER_FRAME = 1/PHYSICS_FPS;
const int NETWORK_LATENCY_CALCULATION_ARRAY_SIZE = (int)PHYSICS_FPS;	// Just pick something small.
const int NETWORK_POSITIONAL_PACKET_BUFFER_SIZE = (int)PHYSICS_FPS/2;
class NetworkPhysicsTest
{
};
Lepra::LogDecorator gNptLog(Lepra::LogType::GetLog(Lepra::LogType::SUB_TEST), typeid(NetworkPhysicsTest));



void ReportTestResult(const Lepra::LogDecorator& pLog, const Lepra::String& pTestName, const Lepra::String& pContext, bool pResult);
void StoreFrameDiff(int pAgentIndex, int pFrameDiff);



struct AgentData: public UiLepra::KeyCodeInputObserver
{
	struct BodyInfo
	{
		Lepra::String mType;
		TBC::PhysicsManager::BodyID mPhysicsId;
		TBC::GeometryBase* mGraphicsGeometry;
		UiTbc::Renderer::GeometryID mGraphicsId;
		int mLastSetFrameIndex;	// TODO: this is a hack, read up on what algo to use for clients that want to set other clients positions.
		Lepra::Vector3DF mGraphicsPosition;
		BodyInfo():
			mType(_T("?")),
			mPhysicsId(0),
			mGraphicsGeometry(0),
			mGraphicsId((UiTbc::Renderer::GeometryID)-1),
			mLastSetFrameIndex(0)
		{
		}
		BodyInfo(Lepra::String pType, TBC::PhysicsManager::BodyID pBodyId, TBC::GeometryBase* pGeometry, UiTbc::Renderer::GeometryID pGeometryId):
			mType(pType),
			mPhysicsId(pBodyId),
			mGraphicsGeometry(pGeometry),
			mGraphicsId(pGeometryId),
			mLastSetFrameIndex(0)
		{
		}
	};
	Cure::NetworkAgent* mNetworkAgent;
	TBC::PhysicsManager* mPhysics;
	std::vector<BodyInfo> mBodyArray;
	UiLepra::DisplayManager* mDisplay;
	Lepra::Canvas* mScreen;
	UiTbc::Renderer* mRenderer;
	UiLepra::InputManager* mInput;
	float mTickTimeModulo;
	bool mActive;
	int mCurrentPhysicsFrameIndex;
	double mEmulatedNetworkSendLatencyFrameCount;
	double mEmulatedNetworkSendJitterFrameCount;
	double mServerMeasuredNetworkLatencyFrameCount;
	double mServerMeasuredNetworkJitterFrameCount;
	Lepra::uint32 mClientId;
	int mNetworkFrameLatencyArrayIndex;
	std::vector<int> mNetworkFrameLatencyArray;
	int mPhysicsTickAdjustmentFrameCount;
	float mPhysicsTickAdjustmentTime;
	int mStriveSendErrorTimeCounter;
	int mIngoreStriveErrorTimeCounter;
	int mPacketUsageLateDropCount;
	int mPacketUsageEarlyDropCount;
	int mPacketUsageUseCount;
	int mMoveKeys;
	std::vector<Cure::ObjectPositionalData*> mPositionalBuffer;
	Cure::ObjectPositionalData mClientLastSendPositionalData;
	bool mNetworkForceSend;
	double mBandwidthStartMeasureTime;
	Lepra::uint64 mBandwidthLastSent;
	Lepra::uint64 mBandwidthLastReceived;

	AgentData():
		mNetworkAgent(0),
		mPhysics(0),
		mDisplay(0),
		mScreen(0),
		mRenderer(0),
		mInput(0),
		mTickTimeModulo(0),
		mActive(false),
		mCurrentPhysicsFrameIndex(0),
		mEmulatedNetworkSendLatencyFrameCount(0),
		mEmulatedNetworkSendJitterFrameCount(0),
		mServerMeasuredNetworkLatencyFrameCount(0),
		mServerMeasuredNetworkJitterFrameCount(0),
		mClientId(0),
		mNetworkFrameLatencyArrayIndex(0),
		mNetworkFrameLatencyArray(NETWORK_LATENCY_CALCULATION_ARRAY_SIZE),
		mPhysicsTickAdjustmentFrameCount(0),
		mPhysicsTickAdjustmentTime(0),
		mStriveSendErrorTimeCounter(100),
		mIngoreStriveErrorTimeCounter(0),
		mPacketUsageLateDropCount(0),
		mPacketUsageEarlyDropCount(0),
		mPacketUsageUseCount(0),
		mMoveKeys(0),
		mPositionalBuffer(NETWORK_POSITIONAL_PACKET_BUFFER_SIZE),
		mNetworkForceSend(false),
		mBandwidthStartMeasureTime(0),
		mBandwidthLastSent(0),
		mBandwidthLastReceived(0)
	{
	}
	~AgentData()
	{
		for (int x = 0; x < NETWORK_POSITIONAL_PACKET_BUFFER_SIZE; ++x)
		{
			StorePositionalData(x, 0);
		}
		delete (mInput);
		mInput = 0;
		delete (mRenderer);
		mRenderer = 0;
		delete (mScreen);
		mScreen = 0;
		delete (mDisplay);
		mDisplay = 0;
		delete (mPhysics);
		mPhysics = 0;
		delete (mNetworkAgent);
		mNetworkAgent = 0;
	}

	void AddBody(Lepra::String pType, TBC::PhysicsManager::BodyID pBodyId, TBC::GeometryBase* pGeometry, UiTbc::Renderer::GeometryID pGeometryId)
	{
		mBodyArray.push_back(BodyInfo(pType, pBodyId, pGeometry, pGeometryId));
	}

	void StorePositionalData(int x, Cure::ObjectPositionalData* pData)
	{
		delete (mPositionalBuffer[x]);
		mPositionalBuffer[x] = pData;
	}

	bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
	{
		switch (pKeyCode)
		{
			case UiLepra::InputManager::IN_KBD_UP:	mMoveKeys |= 1;	break;
			case UiLepra::InputManager::IN_KBD_DOWN:	mMoveKeys |= 2;	break;
			case UiLepra::InputManager::IN_KBD_LEFT:	mMoveKeys |= 4;	break;
			case UiLepra::InputManager::IN_KBD_RIGHT:	mMoveKeys |= 8;	break;
		}
		return (false);
	}
	bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
	{
		switch (pKeyCode)
		{
			case UiLepra::InputManager::IN_KBD_UP:	mMoveKeys &= ~1;	break;
			case UiLepra::InputManager::IN_KBD_DOWN:	mMoveKeys &= ~2;	break;
			case UiLepra::InputManager::IN_KBD_LEFT:	mMoveKeys &= ~4;	break;
			case UiLepra::InputManager::IN_KBD_RIGHT:	mMoveKeys &= ~8;	break;
		}
		mNetworkForceSend = true;
		return (false);
	}
};

struct DelayedNetworkSend
{
	DelayedNetworkSend(double pAbsoluteSendTime, int pClientIndex, int pFrameIndex, const Cure::ObjectPositionalData& pMovement,
		AgentData* pFrom, AgentData* pTo):
		mAbsoluteSendTime(pAbsoluteSendTime),
		mFrom(pFrom),
		mTo(pTo)
	{
		mPacket = mTo->mNetworkAgent->GetPacketFactory()->Allocate();
		Cure::MessageObjectPosition* lMessage = (Cure::MessageObjectPosition*)mTo->mNetworkAgent->GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_OBJECT_POSITION);
		mPacket->AddMessage(lMessage);
		lMessage->Store(mPacket, pClientIndex, pFrameIndex, pMovement);
		mPacket->StoreHeader();
	}
	double mAbsoluteSendTime;
	AgentData* mFrom;
	AgentData* mTo;
	Cure::Packet* mPacket;
};
class ServerDummyLoginListener: public Cure::NetworkServer::LoginListener
{
public:
	ServerDummyLoginListener(Cure::UserAccountManager* pUserAccountManager):
		mUserAccountManager(pUserAccountManager)
	{
	}
	Cure::UserAccount::Availability QueryLogin(const Cure::LoginId& pLoginId, Cure::UserAccount::AccountId& pAccountId)
	{
		return (mUserAccountManager->GetUserAccountStatus(pLoginId, pAccountId));
	};
	void OnLogin(Cure::UserConnection*) {};
	void OnLogout(Cure::UserConnection*) {};

	Cure::UserAccountManager* mUserAccountManager;
};



AgentData gServer;
ServerDummyLoginListener gLoginListener(new Cure::MemoryUserAccountManager());
AgentData gClient[CLIENT_COUNT];
double gAbsoluteTime = 0;
double gFPS = 0;
std::list<DelayedNetworkSend> gDelayedNetworkSendArray;

void ReportTestResult(const Lepra::LogDecorator& pLog, const Lepra::String& pTestName, const Lepra::String& pContext, bool pResult);



bool NetworkLoginClients()
{
	Lepra::String lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = _T("network start");
		lTestOk = Lepra::Network::Start();
	}

	Cure::NetworkServer* lServer = 0;
	if (lTestOk)
	{
		lContext = _T("server start");
		lServer = new Cure::NetworkServer(Cure::GetSettings(), &gLoginListener);
		gServer.mNetworkAgent = lServer;
		//lServer->GetPacketFactory()->SetMessageFactory(new PositionalMessageFactory());
		lTestOk = lServer->Start(_T("localhost:12345"));
	}

	// Create user accounts.
	for (int x = 0; lTestOk && x < CLIENT_COUNT; ++x)
	{
		lContext = _T("client create");
		Lepra::UnicodeString lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(Lepra::UnicodeStringUtility::Format(L"user%i", x), lPassword);
		lTestOk = gLoginListener.mUserAccountManager->AddUserAccount(lUser);
	}

	class ServerPoller: public Lepra::Thread
	{
	public:
		ServerPoller(Cure::NetworkServer* pServer):
			Thread(_T("Server poller")),
			mServer(pServer)
		{
		}
		void Run()
		{
			Cure::Packet* lPacket = mServer->GetPacketFactory()->Allocate();
			lPacket->Release();
			while (!GetStopRequest())
			{
				Lepra::uint32 lClientId;
				mServer->ReceiveFirstPacket(lPacket, lClientId);
				mServer->SendAll();
				Thread::Sleep(0.05);
			}
			mServer->GetPacketFactory()->Release(lPacket);
		};
		Cure::NetworkServer* mServer;
	};
	ServerPoller lThread(lServer);
	lThread.Start();

	// Login clients.
	for (int x = 0; lTestOk && x < CLIENT_COUNT; ++x)
	{
		Cure::NetworkClient* lClient = new Cure::NetworkClient(Cure::GetSettings());
		gClient[x].mNetworkAgent = lClient;
		lContext = _T("client connect+login");
		Lepra::UnicodeString lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(Lepra::UnicodeStringUtility::Format(L"user%i", x), lPassword);
		lClient->StartConnectLogin(_T("localhost:12345"), 2.0, lUser);
		Cure::RemoteStatus lStatus = lClient->WaitLogin();
		gClient[x].mClientId = lClient->GetLoginAccountId();
		bool lTestOk = (lStatus == Cure::REMOTE_OK);
		if (lTestOk)
		{
			gServer.mEmulatedNetworkSendLatencyFrameCount = 0.400f*PHYSICS_FPS;
			gServer.mEmulatedNetworkSendJitterFrameCount = 0.080f*PHYSICS_FPS;	// Interval around latency between -jitter to +jitter.
			gClient[x].mEmulatedNetworkSendLatencyFrameCount = (x+1)*0.400f*PHYSICS_FPS;
			gClient[x].mEmulatedNetworkSendJitterFrameCount = 0.080f*PHYSICS_FPS;	// Interval around latency between -jitter to +jitter.
			for (int y = 0; y < NETWORK_LATENCY_CALCULATION_ARRAY_SIZE; ++y)
			{
				StoreFrameDiff(x, (int)(gClient[x].mEmulatedNetworkSendLatencyFrameCount+gClient[x].mEmulatedNetworkSendJitterFrameCount*(y&1)*((y&2)-1)));
			}
		}
	}

	ReportTestResult(gNptLog, _T("NetworkLoginClients"), lContext, lTestOk);
	return (lTestOk);
}

void NetworkShutdown()
{
	assert(gServer.mNetworkAgent->GetConnectionCount() == CLIENT_COUNT);
	for (int x = 0; x < CLIENT_COUNT; ++x)
	{
		assert(gClient[x].mNetworkAgent->GetConnectionCount() == 1);
		delete (gClient[x].mNetworkAgent);
		gClient[x].mNetworkAgent = 0;
	}
	delete (gServer.mNetworkAgent);
	gServer.mNetworkAgent = 0;
	Lepra::Network::Stop();
}

bool CreateWorld(AgentData& pAgentData)
{
	bool lTestOk = true;
	pAgentData.mTickTimeModulo = 0;
	pAgentData.mPhysics = TBC::PhysicsManagerFactory::Create(TBC::PhysicsManagerFactory::ENGINE_ODE);
	pAgentData.mPhysics->SetGravity(Lepra::Vector3DF(0, 0, -10));
	// Create floor on server.
	const float lFloorSize = 100;
	const float lClientSize = 5;
	Lepra::TransformationF lFloorPlacement;
	Lepra::Vector3DF lFloorPosition(0, 0, -lFloorSize/2);
	lFloorPlacement.SetPosition(lFloorPosition);
	TBC::PhysicsManager::BodyID lPhysicsId;
	UiTbc::TriangleBasedGeometry* lGeometry;
	UiTbc::Renderer::GeometryID lGraphicsId;
	lPhysicsId = pAgentData.mPhysics->CreateBox(true, lFloorPlacement, 3, Lepra::Vector3DF(lFloorSize, lFloorSize, lFloorSize), TBC::PhysicsManager::STATIC, 1, 1.0f);
	lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lFloorSize, lFloorSize, lFloorSize);
	lGeometry->SetAlwaysVisible(true);
	lGraphicsId = pAgentData.mRenderer->AddGeometry(lGeometry, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID, UiTbc::Renderer::NO_SHADOWS);
	pAgentData.AddBody(_T("StaticBox"), lPhysicsId, lGeometry, lGraphicsId);
	// Create client spheres on server.
	for (int y = 0; y < CLIENT_COUNT; ++y)
	{
		Lepra::TransformationF lClientPlacement;
		Lepra::Vector3DF lClientPosition(-lFloorSize/2+y*lClientSize*4, 0, lClientSize*10);
		lClientPlacement.SetPosition(lClientPosition);
		lPhysicsId = pAgentData.mPhysics->CreateSphere(true, lClientPlacement, lClientSize, lClientSize, TBC::PhysicsManager::DYNAMIC, 1, 1.0f);
		pAgentData.mPhysics->ActivateGravity(lPhysicsId);
		lGeometry = UiTbc::BasicMeshCreator::CreateEllipsoid(lClientSize, lClientSize, lClientSize, 10, 10);
		lGeometry->SetAlwaysVisible(true);
		TBC::GeometryBase::BasicMaterialSettings lMaterial(
			Lepra::Vector3DF(0, 0, 0),
			Lepra::Vector3DF((float)(y%2), (float)(y/2%2), (float)(y/3%2)),
			Lepra::Vector3DF(0.1f, 0.1f, 0.1f),
			0.8f, 1.0f, true);
		lGeometry->SetBasicMaterialSettings(lMaterial);
		lGraphicsId = pAgentData.mRenderer->AddGeometry(lGeometry, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID, UiTbc::Renderer::NO_SHADOWS);
		pAgentData.AddBody(_T("DynamicSphere"), lPhysicsId, lGeometry, lGraphicsId);
	}
	Lepra::TransformationF lCameraPlacement;
	Lepra::Vector3DF lCameraPosition(0, -lFloorSize, lClientSize*5);
	lCameraPlacement.SetPosition(lCameraPosition);
	pAgentData.mRenderer->SetCameraTransformation(lCameraPlacement);
	return (lTestOk);
}

bool CreateWorld()
{
	bool lTestOk = true;
	if (lTestOk)
	{
		lTestOk = CreateWorld(gServer);
	}
	for (int x = 0; lTestOk && x < CLIENT_COUNT; ++x)
	{
		lTestOk = CreateWorld(gClient[x]);
	}
	return (lTestOk);
}

void DeleteWorld(AgentData& pAgentData)
{
	for (unsigned x = 0; x < pAgentData.mBodyArray.size(); ++x)
	{
		AgentData::BodyInfo& lBody = pAgentData.mBodyArray[x];
		pAgentData.mRenderer->RemoveGeometry(lBody.mGraphicsId);
		delete (lBody.mGraphicsGeometry);
		lBody.mGraphicsGeometry = 0;
		pAgentData.mPhysics->DeleteBody(lBody.mPhysicsId);
	}
	delete (pAgentData.mPhysics);
	pAgentData.mPhysics = 0;
}

void DeleteWorld()
{
	for (int x = 0; x < CLIENT_COUNT; ++x)
	{
		DeleteWorld(gClient[x]);
	}
	DeleteWorld(gServer);
}

void PostDelayedMovement(const Cure::ObjectPositionalData& pData, int pClientIndex, int pFrameIndex, AgentData* pFrom, AgentData* pTo)
{
	const double lCurrentTime = gAbsoluteTime;
	const double lAbsoluteSendTime = lCurrentTime + pFrom->mEmulatedNetworkSendLatencyFrameCount*PHYSICS_SECONDS_PER_FRAME + Lepra::Random::Uniform(-pFrom->mEmulatedNetworkSendJitterFrameCount, pFrom->mEmulatedNetworkSendJitterFrameCount)*PHYSICS_SECONDS_PER_FRAME;
	/*if (pFrom == &gClient[CLIENT_COUNT-1])
	{
		logdebug(_T("Client post"), Lepra::StringUtility::Format(_T("Frame %i when server on %i, waiting for time %f, current time %f."), gClient[CLIENT_COUNT-1].mCurrentPhysicsFrameIndex, gServer.mCurrentPhysicsFrameIndex, lAbsoluteSendTime, gAbsoluteTime));
	}*/
	for (std::list<DelayedNetworkSend>::iterator x = gDelayedNetworkSendArray.begin(); x != gDelayedNetworkSendArray.end(); ++x)
	{
		const DelayedNetworkSend& lDelayedSend = *x;
		if (lDelayedSend.mAbsoluteSendTime > lAbsoluteSendTime)
		{
			gDelayedNetworkSendArray.insert(x, DelayedNetworkSend(lAbsoluteSendTime, pClientIndex, pFrameIndex, pData, pFrom, pTo));
			return;	// TRICKY.
		}
	}
	gDelayedNetworkSendArray.push_back(DelayedNetworkSend(lAbsoluteSendTime, pClientIndex, pFrameIndex, pData, pFrom, pTo));
	/*if (pFrom == &gClient[CLIENT_COUNT-1])
	{
		Cure::MessageObjectPosition* lMessage = (Cure::MessageObjectPosition*)gDelayedNetworkSendArray.back().mPacket->GetMessageAt(0);
		int lFrameIndex = 0;
		Cure::ObjectPositionalData lPosition;
		lMessage->GetData(lFrameIndex, lPosition);
		logdebug(_T("Client post"), Lepra::StringUtility::Format(_T("Frame %i when server on %i, waiting for time %f, current time %f."), lFrameIndex, gServer.mCurrentPhysicsFrameIndex, gDelayedNetworkSendArray.back().mAbsoluteSendTime, gAbsoluteTime));
	}*/
}

void StoreFrameDiff(int pAgentIndex, int pFrameDiff)
{
	int& lReferenceFrameDiffIndex = gClient[pAgentIndex].mNetworkFrameLatencyArrayIndex;
	gClient[pAgentIndex].mNetworkFrameLatencyArray[lReferenceFrameDiffIndex++] = pFrameDiff;
	if (lReferenceFrameDiffIndex >= NETWORK_LATENCY_CALCULATION_ARRAY_SIZE)
	{
		lReferenceFrameDiffIndex = 0;
	}
}

void CalculateClientNetworkParameters(int pAgentIndex, const int pFrameDiff)
{
	AgentData& lClient = gClient[pAgentIndex];
	StoreFrameDiff(pAgentIndex, pFrameDiff);
	const int lMedian = Lepra::Math::CalculateMedian<int>(lClient.mNetworkFrameLatencyArray);
	const double lDeviation = Lepra::Math::CalculateDeviation<double, int>(lClient.mNetworkFrameLatencyArray, lMedian);
	const double lSlidingMeanLerpFactor = 0.5;
	lClient.mServerMeasuredNetworkLatencyFrameCount = Lepra::Math::Lerp<double>(lMedian, lClient.mServerMeasuredNetworkLatencyFrameCount, lSlidingMeanLerpFactor);
	lClient.mServerMeasuredNetworkJitterFrameCount = Lepra::Math::Lerp<double>(lDeviation, lClient.mServerMeasuredNetworkJitterFrameCount, lSlidingMeanLerpFactor);
	//logdebug(_T("Server measures latencies"), Lepra::StringUtility::Format(_T("Client %i: latency frame count=%f, jitter frame count=%f."), pAgentIndex, lClient.mServerMeasuredNetworkLatencyFrameCount, lClient.mServerMeasuredNetworkJitterFrameCount));
}

// A positive return value n means that data from the client to the server comes n frames too late on average (client needs to speed up).
// A negative return value n means that data from the client to the server comes n frames too early on average (client needs to slow down).
int CalculateNetworkLatencyFrameDiffCount(AgentData& pClient)
{
	return ((int)(pClient.mServerMeasuredNetworkLatencyFrameCount + pClient.mServerMeasuredNetworkJitterFrameCount));
}

void UpdateAssumedNetworkLatency(AgentData& pAgentData, int pAdjustFrameCount)
{
	// Adjust some of the oldest measured frame diffs.
	for (int x = 0; x < NETWORK_LATENCY_CALCULATION_ARRAY_SIZE; ++x)
	{
		pAgentData.mNetworkFrameLatencyArray[x] -= pAdjustFrameCount;
	}
	pAgentData.mServerMeasuredNetworkLatencyFrameCount /= 2;
}

int SendClientStriveTimes(int pAgentIndex)
{
	// Don't really need to send, we just go through RAM.
	AgentData& lClient = gClient[pAgentIndex];
	lClient.mPhysicsTickAdjustmentFrameCount = (int)PHYSICS_FPS;	// Spread over some time (=frames).
	int lNetworkFrameDiffCount = CalculateNetworkLatencyFrameDiffCount(lClient);
	// Try to adjust us to be exactly one frame too early. Being too early is much better than being
	// late, since early network data can be buffered and used in upcomming frames, but late network
	// data is always thrown away.
	++lNetworkFrameDiffCount;
	if (lNetworkFrameDiffCount > 0)
	{
		// Speed up client physics by taking longer time steps.
		lClient.mPhysicsTickAdjustmentTime = (float)lNetworkFrameDiffCount/lClient.mPhysicsTickAdjustmentFrameCount;
	}
	else
	{
		// Slow down client physics by taking shorter time steps.
		while (-lNetworkFrameDiffCount > lClient.mPhysicsTickAdjustmentFrameCount)
		{
			lClient.mPhysicsTickAdjustmentFrameCount *= (-lNetworkFrameDiffCount/lClient.mPhysicsTickAdjustmentFrameCount + 1);
		}
		lClient.mPhysicsTickAdjustmentTime = (float)lNetworkFrameDiffCount/lClient.mPhysicsTickAdjustmentFrameCount;
	}
	UpdateAssumedNetworkLatency(lClient, lNetworkFrameDiffCount);
	return (lNetworkFrameDiffCount);
}

void MaybeSendClientStriveTimes(int pAgentIndex)
{
	AgentData& lClient = gClient[pAgentIndex];
	const int lNetworkFrameDiffCount = CalculateNetworkLatencyFrameDiffCount(lClient);
	if (lNetworkFrameDiffCount < -(int)PHYSICS_FPS/10 || lNetworkFrameDiffCount > 0)	// TODO: don't hard-code lower (early) limit? Come up with something smarter?
	{
		if (lNetworkFrameDiffCount < -(int)PHYSICS_FPS/2)	// Dropped because too early.
		{
			++lClient.mPacketUsageEarlyDropCount;
		}
		else if (lNetworkFrameDiffCount > 0)	// Dropped because too late (we've already passed that frame, we don't have time to re-simulate it).
		{
			++lClient.mPacketUsageLateDropCount;
		}
		++lClient.mStriveSendErrorTimeCounter;
		lClient.mIngoreStriveErrorTimeCounter = 0;
		// Only send once every now and then.
		if ((lNetworkFrameDiffCount > 0 && lClient.mStriveSendErrorTimeCounter > (int)PHYSICS_FPS*2)	||
			(lNetworkFrameDiffCount < 0 && lClient.mStriveSendErrorTimeCounter > (int)PHYSICS_FPS*2))
		{
			lClient.mStriveSendErrorTimeCounter = 0;
			const int lTargetNetworkFrameDiffCount = SendClientStriveTimes(pAgentIndex);
			gNptLog.Warning(_T("Server \"sending\" physics tick time adjustments") + Lepra::StringUtility::Format(_T("Client %i: %+i frames (adjusted over %i frames)."), pAgentIndex, lTargetNetworkFrameDiffCount, lClient.mPhysicsTickAdjustmentFrameCount));
		}
		/*else
		{
			Lepra::String s;
			if (lNetworkFrameDiffCount < 0)
			{
				s = Lepra::StringUtility::Format(_T("Client %i: data arrives %i frames early on average."), pAgentIndex, -lNetworkFrameDiffCount);
			}
			else
			{
				s = Lepra::StringUtility::Format(_T("Client %i: data arrives %i frames late on average."), pAgentIndex, lNetworkFrameDiffCount);
			}
			logtrace(_T("Server find client out of range"), s);
		}*/
	}
	else
	{
		++lClient.mPacketUsageUseCount;
		static int x = 0;
		if (++x > 25)
		{
			x = 0;
			double lLateDropPercent = 100.0*lClient.mPacketUsageLateDropCount/(lClient.mPacketUsageLateDropCount+lClient.mPacketUsageEarlyDropCount+lClient.mPacketUsageUseCount);
			double lEarlyDropPercent = 100.0*lClient.mPacketUsageEarlyDropCount/(lClient.mPacketUsageLateDropCount+lClient.mPacketUsageEarlyDropCount+lClient.mPacketUsageUseCount);
			Lepra::String s = Lepra::StringUtility::Format(_T("Client %i: data arrives %i frames early on average with a drop of %.1f %% (late) and %.1f %% (early)."), pAgentIndex, -lNetworkFrameDiffCount, lLateDropPercent, lEarlyDropPercent);
			gNptLog.Debug(_T("Server find client in range ")+s);
			s = Lepra::StringUtility::Format(_T("Client %i: up %.1f kB/s, down %.1f kB/s (%.2f FPS)."), pAgentIndex,
				(lClient.mNetworkAgent->GetTotalSentByteCount()-lClient.mBandwidthLastSent)/1000.0/(gAbsoluteTime-lClient.mBandwidthStartMeasureTime),
				(lClient.mNetworkAgent->GetTotalReceivedByteCount()-lClient.mBandwidthLastReceived)/1000.0/(gAbsoluteTime-lClient.mBandwidthStartMeasureTime), gFPS);
			gNptLog.Debug(_T("Client bandwidth ")+s);
			lClient.mBandwidthLastSent = lClient.mNetworkAgent->GetTotalSentByteCount();
			lClient.mBandwidthLastReceived = lClient.mNetworkAgent->GetTotalReceivedByteCount();
			lClient.mBandwidthStartMeasureTime = gAbsoluteTime;
			/*for (int x = 0; x < NETWORK_LATENCY_CALCULATION_ARRAY_SIZE; ++x)
			{
				logdebug(_T("LatencyArray"), Lepra::StringUtility::Format(_T("%i"), lClient.mNetworkFrameLatencyArray[x]));
			}*/
		}
		++lClient.mIngoreStriveErrorTimeCounter;
		if (lClient.mIngoreStriveErrorTimeCounter > (int)PHYSICS_FPS*2)	// Reset send counter if we're mostly good.
		{
			lClient.mPacketUsageLateDropCount = 0;
			lClient.mPacketUsageEarlyDropCount = 0;
			lClient.mPacketUsageUseCount = 0;
			lClient.mIngoreStriveErrorTimeCounter = 0;
			lClient.mStriveSendErrorTimeCounter = 0;
		}
	}
}

void ClientSetMovement(AgentData& pClientData, int pClientIndex, int pClientFrameIndex, const Cure::ObjectPositionalData& pData)
{
	int& lLastFrame = pClientData.mBodyArray[pClientIndex+1].mLastSetFrameIndex;
	if (lLastFrame < pClientFrameIndex)
	{
		lLastFrame = pClientFrameIndex;
		//Lepra::String s = Lepra::StringUtility::Format(_T("client %i at frame %i"), pClientIndex, pClientFrameIndex);
		//logdebug(_T("Client set pos of other client"), s);
		TBC::PhysicsManager::BodyID lPhysicsId = pClientData.mBodyArray[pClientIndex+1].mPhysicsId;
		pClientData.mPhysics->SetBodyTransform(lPhysicsId, pData.mPosition.mTransformation);
		pClientData.mPhysics->SetBodyVelocity(lPhysicsId, pData.mPosition.mVelocity);
		pClientData.mPhysics->SetBodyAcceleration(lPhysicsId, pData.mPosition.mAcceleration);
		pClientData.mPhysics->SetBodyAngularVelocity(lPhysicsId, pData.mPosition.mAngularVelocity);
		pClientData.mPhysics->SetBodyAngularAcceleration(lPhysicsId, pData.mPosition.mAngularAcceleration);
	}
}

void ClientReceive(int pAgentIndex)
{
	AgentData& lClientData = gClient[pAgentIndex];
	Cure::Packet* lPacket = lClientData.mNetworkAgent->GetPacketFactory()->Allocate();
	Cure::NetworkAgent::ReceiveStatus lReceived = ((Cure::NetworkClient*)lClientData.mNetworkAgent)->ReceiveNonBlocking(lPacket);
	if (lReceived == Cure::NetworkAgent::RECEIVE_OK)
	{
		Cure::Packet::ParseResult lParseResult;
		do
		{
			for (int i = 0; i < lPacket->GetMessageCount(); ++i)
			{
				Cure::MessageObjectPosition* lMessage = (Cure::MessageObjectPosition*)lPacket->GetMessageAt(0);
				int lClientFrameIndex = -1;
				int lClientIndex = -1;
				lClientIndex = lMessage->GetObjectId();
				lClientFrameIndex = lMessage->GetFrameIndex();
				const Cure::ObjectPositionalData& lData = lMessage->GetPositionalData();
				ClientSetMovement(lClientData, lClientIndex, lClientFrameIndex, lData);
			}
			lParseResult = lPacket->ParseMore();
		}
		while (lParseResult == Cure::Packet::PARSE_OK);
		assert(lParseResult == Cure::Packet::PARSE_NO_DATA);
	}
	else if (lReceived != Cure::NetworkAgent::RECEIVE_NO_DATA)
	{
		gNptLog.Debug(_T("Client networking error: problem receiving data!"));
	}
	gServer.mNetworkAgent->GetPacketFactory()->Release(lPacket);
}

void ServerAdjustClientSimulationSpeed(int pClientIndex, int pFrameIndex)
{
	Lepra::String s;
	int lFrameDiff = gServer.mCurrentPhysicsFrameIndex-pFrameIndex;
	/*if (lFrameDiff == 0)
	{
		s = Lepra::StringUtility::Format(_T("client %i right on time"), pClientIndex);
		//logdebug(_T("Server receive"), s);
	}
	else if (lFrameDiff < 0)
	{
		s = Lepra::StringUtility::Format(_T("client %i is %i frames ahead of server"), pClientIndex, -lFrameDiff);
		//logdebug(_T("Server receive"), s);
	}
	else
	{
		s = Lepra::StringUtility::Format(_T("client %i is %i frames after server (s=%i, c=%i) - this is no good"), pClientIndex, lFrameDiff, gServer.mCurrentPhysicsFrameIndex, gClient[pClientIndex].mCurrentPhysicsFrameIndex);
		//logdebug(_T("Server receive"), s);
	}*/
	CalculateClientNetworkParameters(pClientIndex, lFrameDiff);
	MaybeSendClientStriveTimes(pClientIndex);
}

void ServerStoreClientMovement(int pClientIndex, int pFrameIndex, Cure::ObjectPositionalData* pData)
{
	const int lFrameOffset = pFrameIndex-gServer.mCurrentPhysicsFrameIndex;
	if (lFrameOffset >= 0 && lFrameOffset < NETWORK_POSITIONAL_PACKET_BUFFER_SIZE)
	{
		const int lFrameCycleIndex = (gServer.mCurrentPhysicsFrameIndex+lFrameOffset)%NETWORK_POSITIONAL_PACKET_BUFFER_SIZE;
		gClient[pClientIndex].StorePositionalData(lFrameCycleIndex, pData);
	}
	else
	{
		// This input data is already old or too much ahead!
		delete (pData);
	}
}

Cure::ObjectPositionalData* ServerPopClientMovement(int pClientIndex, int pServerFrameIndex)
{
	const int lFrameCycleIndex = pServerFrameIndex%NETWORK_POSITIONAL_PACKET_BUFFER_SIZE;
	Cure::ObjectPositionalData* lData = gClient[pClientIndex].mPositionalBuffer[lFrameCycleIndex];
	gClient[pClientIndex].mPositionalBuffer[lFrameCycleIndex] = 0;
	return (lData);
}

void ServerAdjustClientMovement(int pClientIndex, const Cure::ObjectPositionalData& pData)
{
	TBC::PhysicsManager::BodyID lPhysicsId = gServer.mBodyArray[pClientIndex+1].mPhysicsId;
	gServer.mPhysics->SetBodyTransform(lPhysicsId, pData.mPosition.mTransformation);
	gServer.mPhysics->SetBodyVelocity(lPhysicsId, pData.mPosition.mVelocity);
	gServer.mPhysics->SetBodyAcceleration(lPhysicsId, pData.mPosition.mAcceleration);
	gServer.mPhysics->SetBodyAngularVelocity(lPhysicsId, pData.mPosition.mAngularVelocity);
	gServer.mPhysics->SetBodyAngularAcceleration(lPhysicsId, pData.mPosition.mAngularAcceleration);
}

void ServerHandleNetworkInput(int pFrameIndex)
{
	for (int x = 0; x < CLIENT_COUNT; ++x)
	{
		Cure::ObjectPositionalData* lData = ServerPopClientMovement(x, pFrameIndex);
		if (lData)
		{
			ServerAdjustClientMovement(x, *lData);
		}
		delete (lData);
	}
}

void ServerBroadcast(int pClientIndex, int pClientFrameIndex, const Cure::ObjectPositionalData& pData)
{
	for (int x = 0; x < CLIENT_COUNT; ++x)
	{
		if (x != pClientIndex)
		{
			PostDelayedMovement(pData, pClientIndex, pClientFrameIndex, &gServer, &gClient[x]);
		}
	}
}

int GetClientIndexFromNetworkId(Lepra::uint32 pNetworkClientId)
{
	for (int x = 0; x < CLIENT_COUNT; ++x)
	{
		if (gClient[x].mClientId == pNetworkClientId)
		{
			return (x);
		}
	}
	return (-100000000);
}

void ServerReceive()
{
	Cure::Packet* lPacket = gServer.mNetworkAgent->GetPacketFactory()->Allocate();
	Cure::NetworkAgent::ReceiveStatus lReceived = Cure::NetworkAgent::RECEIVE_OK;
	while (lReceived != Cure::NetworkAgent::RECEIVE_NO_DATA)
	{
		Lepra::uint32 lId;
		lReceived = ((Cure::NetworkServer*)gServer.mNetworkAgent)->ReceiveFirstPacket(lPacket, lId);
		int x = GetClientIndexFromNetworkId(lId);
		if (lReceived == Cure::NetworkAgent::RECEIVE_OK)
		{
			//gNptLog.Debugf(_T("Server received data from client %i!"), x);
			Cure::Packet::ParseResult lParseResult;
			do
			{
				for (int i = 0; i < lPacket->GetMessageCount(); ++i)
				{
					Cure::MessageObjectPosition* lMessage = (Cure::MessageObjectPosition*)lPacket->GetMessageAt(i);
					int lClientIndex = -1;
					int lClientFrameIndex = -1;
					Cure::ObjectPositionalData* lData = new Cure::ObjectPositionalData();
					lClientIndex = lMessage->GetObjectId();
					lClientFrameIndex = lMessage->GetFrameIndex();
					lData->CopyData(&lMessage->GetPositionalData());
					ServerAdjustClientSimulationSpeed(x, lClientFrameIndex);

					// Pass on to other clients.
					ServerBroadcast(x, lClientFrameIndex, *lData);

					ServerStoreClientMovement(x, lClientFrameIndex, lData);
				}
				lParseResult = lPacket->ParseMore();
			}
			while (lParseResult == Cure::Packet::PARSE_OK);
			assert(lParseResult == Cure::Packet::PARSE_NO_DATA);
		}
		else if (lReceived != Cure::NetworkAgent::RECEIVE_NO_DATA)
		{
			gNptLog.AError("Server networking error: problem receiving data!");
		}
	}
	gServer.mNetworkAgent->GetPacketFactory()->Release(lPacket);
}

void ClientSendSelf(int pClientIndex)
{
	AgentData& lFrom = gClient[pClientIndex];
	Cure::ObjectPositionalData lData;
	lFrom.mPhysics->GetBodyTransform(lFrom.mBodyArray[pClientIndex+1].mPhysicsId, lData.mPosition.mTransformation);
	lFrom.mPhysics->GetBodyVelocity(lFrom.mBodyArray[pClientIndex+1].mPhysicsId, lData.mPosition.mVelocity);
	lFrom.mPhysics->GetBodyAcceleration(lFrom.mBodyArray[pClientIndex+1].mPhysicsId, lData.mPosition.mAcceleration);
	lFrom.mPhysics->GetBodyAngularVelocity(lFrom.mBodyArray[pClientIndex+1].mPhysicsId, lData.mPosition.mAngularVelocity);
	lFrom.mPhysics->GetBodyAngularAcceleration(lFrom.mBodyArray[pClientIndex+1].mPhysicsId, lData.mPosition.mAngularAcceleration);
	if (lFrom.mNetworkForceSend || lData.GetScaledDifference(&lFrom.mClientLastSendPositionalData) > 10.0f)
	{
		lFrom.mNetworkForceSend = false;
		lFrom.mClientLastSendPositionalData.CopyData(&lData);
		PostDelayedMovement(lData, pClientIndex, lFrom.mCurrentPhysicsFrameIndex, &lFrom, &gServer);
	}
}

bool TickEmulatedLatencyNetwork()
{
	bool lServerFlush = false;
	bool lOk = true;
	while (lOk && gDelayedNetworkSendArray.size() > 0)
	{
		double lTimeUntilSend = gDelayedNetworkSendArray.front().mAbsoluteSendTime-gAbsoluteTime;
		if (lTimeUntilSend <= 0)
		{
			const DelayedNetworkSend lTransmit = gDelayedNetworkSendArray.front();
			gDelayedNetworkSendArray.pop_front();
			if (lTransmit.mFrom->mClientId == 0)
			{
				lServerFlush = true;
				Cure::NetworkServer* lNetworkServer = (Cure::NetworkServer*)(lTransmit.mFrom->mNetworkAgent);
				lOk = lNetworkServer->PlaceInSendBuffer(false, lTransmit.mPacket, lTransmit.mTo->mClientId);
				if (!lOk)
				{
					gNptLog.AError("Could not send to remote client!");
				}
			}
			else
			{
				/*if (lTransmit.mFrom == &gClient[CLIENT_COUNT-1])
				{
					logdebug(_T("Client send"), Lepra::StringUtility::Format(_T("Frame %i when server on %i, waited for time %f, current time %f."), gClient[CLIENT_COUNT-1].mCurrentPhysicsFrameIndex, gServer.mCurrentPhysicsFrameIndex, lTransmit.mAbsoluteSendTime, gAbsoluteTime));
				}*/
				Cure::NetworkClient* lNetworkClient = (Cure::NetworkClient*)(lTransmit.mFrom->mNetworkAgent);
				lOk = lNetworkClient->PlaceInSendBuffer(false, lNetworkClient->GetSocket(), lTransmit.mPacket);
				if (!lOk)
				{
					gNptLog.AError("Could not send to remote server!");
				}
			}
		}
		else
		{
			break;
		}
	}
	if (lOk && lServerFlush)
	{
		lOk = ((Cure::NetworkServer*)gServer.mNetworkAgent)->SendAll();
	}
	for (int x = 0; lOk && x < CLIENT_COUNT; ++x)
	{
		Cure::NetworkClient* lNetworkClient = (Cure::NetworkClient*)(gClient[x].mNetworkAgent);
		lOk = lNetworkClient->SendAll();
	}
	return (lOk);
}

bool OpenWindow(AgentData& pAgentData, const Lepra::String& pCaption)
{
	Lepra::String lContext;
	bool lTestOk = true;
	if (lTestOk)
	{
		lContext = _T("create display manager");
		pAgentData.mDisplay = UiLepra::DisplayManager::CreateDisplayManager(UiLepra::DisplayManager::OPENGL_CONTEXT);
		lTestOk = (pAgentData.mDisplay != 0);
	}
	UiLepra::DisplayMode lDisplayMode;
	if (lTestOk)
	{
		lContext = _T("find display mode");
		lTestOk = pAgentData.mDisplay->FindDisplayMode(lDisplayMode, 640, 480, 32);
	}
	if (lTestOk)
	{
		lContext = _T("open screen");
		lTestOk = pAgentData.mDisplay->OpenScreen(lDisplayMode, UiLepra::DisplayManager::WINDOWED);
	}
	if (lTestOk)
	{
		pAgentData.mScreen = new Lepra::Canvas(pAgentData.mDisplay->GetWidth(), pAgentData.mDisplay->GetHeight(), Lepra::Canvas::IntToBitDepth(pAgentData.mDisplay->GetBitDepth()));
		pAgentData.mScreen->SetBuffer(pAgentData.mDisplay->GetScreenPtr());
		pAgentData.mRenderer = UiTbc::RendererFactory::Create(UiLepra::DisplayManager::OPENGL_CONTEXT, pAgentData.mScreen);
		pAgentData.mRenderer->SetClearColor(Lepra::Color(128, 128, 128));
		pAgentData.mDisplay->SetCaption(pCaption);
		lTestOk = pAgentData.mDisplay->SetVSyncEnabled(false);
	}
	if (lTestOk)
	{
		pAgentData.mInput = UiLepra::InputManager::CreateInputManager(pAgentData.mDisplay);
		pAgentData.mInput->AddKeyCodeInputObserver(&pAgentData);
	}
	if (lTestOk)
	{
		pAgentData.mActive = true;
	}
	ReportTestResult(gNptLog, _T("OpenWindow"), lContext, lTestOk);
	return (lTestOk);
}

bool OpenWindows()
{
	Lepra::String lContext;
	bool lTestOk = true;
	if (lTestOk)
	{
		lContext = _T("create server window");
		lTestOk = OpenWindow(gServer, _T("Server"));
	}
	for (int x = 0; lTestOk && x < CLIENT_COUNT; ++x)
	{
		lContext = Lepra::StringUtility::Format(_T("create client window %i"), x);
		lTestOk = OpenWindow(gClient[x], Lepra::StringUtility::Format(_T("Client %i"), x));
	}
	ReportTestResult(gNptLog, _T("OpenWindows"), lContext, lTestOk);
	return (lTestOk);
}

void CloseWindow(AgentData& pAgentData)
{
	delete (pAgentData.mInput);
	pAgentData.mInput = 0;
	delete (pAgentData.mRenderer);
	pAgentData.mRenderer = 0;
	delete (pAgentData.mScreen);
	pAgentData.mScreen = 0;
	delete (pAgentData.mDisplay);
	pAgentData.mDisplay = 0;
}

void CloseWindows()
{
	CloseWindow(gServer);
	for (int x = 0; x < CLIENT_COUNT; ++x)
	{
		CloseWindow(gClient[x]);
	}
}

bool UpdateGraphicsObjects(int pAgentIndex)
{
	AgentData* lAgentData;
	if (pAgentIndex < 0)
	{
		lAgentData = &gServer;
	}
	else
	{
		lAgentData = &gClient[pAgentIndex];
	}

	// Below is some data used when lerping (optimized by pulling out of the inner loop).
	const float lCloseAfterTime = 0.5f;	// Unit is seconds. In this many lerps, we want to be close to the 
	const float lCloseness = 5e-9f;	// Some sort of lerp thingie.
	const float lLerpTime = ::pow(lCloseness, 1/(lCloseAfterTime*PHYSICS_FPS));

	bool lTestOk = true;
	Lepra::TransformationF lTransformation;
	for (int x = 0; x < (int)lAgentData->mBodyArray.size(); ++x)
	{
		AgentData::BodyInfo& lBody = lAgentData->mBodyArray[x];
		lAgentData->mPhysics->GetBodyTransform(lBody.mPhysicsId, lTransformation);
		// Check if we "slide" to the correct position.
		if (pAgentIndex == -1 || x == 0 || x-1 == pAgentIndex)
		{
			// Server: set all without sliding. Client: set box and self without sliding.
			lBody.mGraphicsGeometry->SetTransformation(lTransformation);
		}
		else
		{
			// Client: set other with sliding (moving average).
			lBody.mGraphicsPosition = Lepra::Math::Lerp<Lepra::Vector3DF, float>(lTransformation.GetPosition(), lBody.mGraphicsPosition, lLerpTime);
			lTransformation.SetPosition(lBody.mGraphicsPosition);
			lBody.mGraphicsGeometry->SetTransformation(lTransformation);
		}
	}
	return (lTestOk);
}

void ClientHandleUserInput(int pClientIndex)
{
	//UiLepra::InputManager* lInputManager = UiLepra::InputManager::GetInputManager();
	//lInputManager->PollEvents();
	AgentData& lAgentData = gClient[pClientIndex];
	const float lForce = 800;
	Lepra::Vector3DF lForceVector[] = { Lepra::Vector3DF(0, lForce, 0), Lepra::Vector3DF(0, -lForce, 0), Lepra::Vector3DF(-lForce, 0, 0), Lepra::Vector3DF(lForce, 0, 0), };
	TBC::PhysicsManager::BodyID lPhysicsId = lAgentData.mBodyArray[pClientIndex+1].mPhysicsId;
	for (int lKey = 0; lKey < 4; ++lKey)
	{
		if (lAgentData.mMoveKeys&(1<<lKey))
		{
			lAgentData.mPhysics->AddForce(lPhysicsId, lForceVector[lKey]);
			lAgentData.mNetworkForceSend = true;
		}
	}
}

void StepPhysics(int pAgentIndex, int pStepCount)
{
	AgentData* lAgentData;
	if (pAgentIndex < 0)
	{
		lAgentData = &gServer;
	}
	else
	{
		lAgentData = &gClient[pAgentIndex];
	}
	const float lThisStepTime = PHYSICS_SECONDS_PER_FRAME*pStepCount;
	lAgentData->mPhysics->StepFast(lThisStepTime);
	lAgentData->mCurrentPhysicsFrameIndex += pStepCount;
	lAgentData->mTickTimeModulo -= lThisStepTime;

	// Adjust some if needed (server tells client to adjust time so that it will receive network data just in time).
	lAgentData->mTickTimeModulo += lAgentData->mPhysicsTickAdjustmentTime*PHYSICS_SECONDS_PER_FRAME;
	if (lAgentData->mPhysicsTickAdjustmentFrameCount > 0)
	{
		lAgentData->mPhysicsTickAdjustmentFrameCount -= pStepCount;
	}
	else
	{
		lAgentData->mPhysicsTickAdjustmentTime = 0;
	}

	// Simulate basic physics on the network output ghost(s). The N.O.G. contains the last sent information.
	if (pAgentIndex < 0)
	{
		// TODO
	}
	else
	{
		Cure::ObjectPositionalData& lData = lAgentData->mClientLastSendPositionalData;
		// TODO: improve. For now we just apply velocity to position.
		lData.mPosition.mTransformation.GetPosition().Add(lData.mPosition.mVelocity*lThisStepTime);
	}
}

bool Tick(float pTickTime, int pAgentIndex)
{
	AgentData* lAgentData;
	if (pAgentIndex < 0)
	{
		lAgentData = &gServer;
	}
	else
	{
		lAgentData = &gClient[pAgentIndex];
	}

	bool lTestOk = true;
	lAgentData->mTickTimeModulo += pTickTime;
	const int lStepCount = (int)(lAgentData->mTickTimeModulo*PHYSICS_FPS);
	if (lStepCount >= 1 && lAgentData->mActive)
	{
		if (lTestOk)
		{
			bool lWindowOpen = lAgentData->mDisplay->Activate();
			if (!lWindowOpen)
			{
				lAgentData->mActive = false;
			}
		}
		if (lTestOk)
		{
			lAgentData->mRenderer->Clear();
			if (pAgentIndex < 0)
			{
				ServerReceive();
				for (int x = 0; x < lStepCount; ++x)
				{
					ServerHandleNetworkInput(gServer.mCurrentPhysicsFrameIndex+x);
				}
			}
			else
			{
				ClientReceive(pAgentIndex);
				for (int x = 0; x < lStepCount; ++x)
				{
					ClientHandleUserInput(pAgentIndex);
				}
				ClientSendSelf(pAgentIndex);
			}
			StepPhysics(pAgentIndex, lStepCount);
			lTestOk = UpdateGraphicsObjects(pAgentIndex);
		}
		if (lTestOk)
		{
			lAgentData->mRenderer->RenderScene();
			bool lWindowOpen = lAgentData->mDisplay->UpdateScreen();
			if (!lWindowOpen)
			{
				lAgentData->mActive = false;
			}
		}
		if (!lAgentData->mActive)
		{
			lAgentData->mDisplay->CloseScreen();
		}
	}
	return (lTestOk);
}

bool Tick(float pTickTime, bool& pQuit)
{
	UiLepra::Core::ProcessMessages();
	pQuit = (Lepra::SystemManager::GetQuitRequest() != 0);
	if (!pQuit)
	{
		pQuit = !gServer.mActive;
	}
	bool lTestOk = true;
	for (int x = 0; !pQuit && lTestOk && x < CLIENT_COUNT; ++x)
	{
		lTestOk = Tick(pTickTime, x);
	}
	if (!pQuit && lTestOk)
	{
		lTestOk = TickEmulatedLatencyNetwork();
	}
	if (!pQuit && lTestOk)
	{
		lTestOk = Tick(pTickTime, -1);
	}
	gAbsoluteTime += pTickTime;
	return (lTestOk);
}

bool TestPrototypeNetworkPhysics()
{
	bool lTestOk = true;

	Lepra::SystemManager::AddQuitRequest(-1);

	if (lTestOk)
	{
		lTestOk = NetworkLoginClients();
	}
	if (lTestOk)
	{
		lTestOk = OpenWindows();
	}
	if (lTestOk)
	{
		lTestOk = CreateWorld();
	}

	bool lQuit = false;
	Lepra::HiResTimer lIrlTimer;
	Lepra::HiResTimer lTimer;
	while (!lQuit && lTestOk && lIrlTimer.GetTimeDiff() < 20.0)
	{
		float lStepTime = (float)lTimer.GetTimeDiff();
		if (lStepTime > 0)
		{
			gFPS = 1/lStepTime;
		}
		if (lStepTime > 0.5f)
		{
			lStepTime = 0.5f;
		}
		lTestOk = Tick(lStepTime, lQuit);
		//lTestOk = Tick(pLogAccount, (float)PHYSICS_SECONDS_PER_FRAME, lQuit);
		//lTestOk = Tick(pLogAccount, (float)1.0, lQuit);
		Lepra::Thread::Sleep(0.001);
		//Lepra::Thread::YieldCpu();
		lIrlTimer.UpdateTimer();
		lTimer.ClearTimeDiff();
		lTimer.UpdateTimer();
	}

	DeleteWorld();
	CloseWindows();
	NetworkShutdown();
	UiLepra::Core::ProcessMessages();
	Lepra::Thread::Sleep(0.05);
	UiLepra::Core::ProcessMessages();

	if (lQuit)
	{
		gNptLog.AWarning("user requested termination");
	}
	else if (!lTestOk)
	{
		ReportTestResult(gNptLog, _T("ExitStatus"), _T("terminating due to error"), lTestOk);
	}

	return (lTestOk);
}

#endif // !CURE_TEST_WITHOUT_UI
