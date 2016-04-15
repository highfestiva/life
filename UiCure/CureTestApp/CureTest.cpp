
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Cure/Include/HiscoreAgent.h"
#include "../../Cure/Include/NetworkClient.h"
#include "../../Cure/Include/NetworkServer.h"
#include "../../Cure/Include/Packet.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TerrainManager.h"
#include "../../Lepra/Include/Logger.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Packer.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/SystemManager.h"



using namespace Lepra;

class CureTest{};
static LogDecorator gCLog(LogType::GetLogger(LogType::SUB_TEST), typeid(CureTest));
void ReportTestResult(const LogDecorator& pLog, const str& pTestName, const str& pContext, bool pbResult);



bool TestPacker(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	uint8 lRawData[1024];
	if (lTestOk)
	{
		lContext = "pack unicode";
		lTestOk = (PackerUnicodeString::Pack(lRawData, L"ABC") == 8);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lTestOk = (lRawData[0] == 4 && lRawData[1] == 0 && lRawData[2] == 'A' &&
				lRawData[3] == 'B' && lRawData[4] == 'C' && lRawData[5] == 0);
			deb_assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = "unpack unicode";
		wstr lUnpacked;
		lTestOk = (PackerUnicodeString::Unpack(lUnpacked, lRawData, 8) == 8);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lTestOk = (lUnpacked == L"ABC");
			deb_assert(lTestOk);
		}
	}

	ReportTestResult(pAccount, "Packer", lContext, lTestOk);
	return (lTestOk);
}

class TerrainTest
{
public:
	bool Test();

private:
	logclass();
};

bool TerrainTest::Test()
{
	str lContext;
	bool lTestOk = true;

	Cure::ResourceManager* lResourceManager = new Cure::ResourceManager(1);
	lResourceManager->InitDefault();
	Cure::TerrainManager lTerrainManager(lResourceManager);	// Creates the TerrainFunctionManager and throws it at the resource manager.

	// Test loading terrain by simply placing a camera.
	if (lTestOk)
	{
		lContext = "load terrain";
		lTerrainManager.AddCamera(vec3(0, 0, 0), 10000);
		Thread::Sleep(0.2);
		lResourceManager->Tick();
		int lOkCount = 0;
		int lErrorCount = 0;
		lTerrainManager.GetLoadCount(lOkCount, lErrorCount);
		lTestOk = (lOkCount == 1 && lErrorCount == 0);
		deb_assert(lTestOk);
	}

	lTerrainManager.Clear();
	delete (lResourceManager);

	SystemManager::AddQuitRequest(-1);

	ReportTestResult(mLog, "TerrainManager", lContext, lTestOk);
	return (lTestOk);
}



class NetworkClientServerTest
{
public:
	bool Test();

private:
	bool TestSpecific(const str& pPrefix, bool pSafe);

	logclass();
};

bool NetworkClientServerTest::Test()
{
	bool lTestOk = true; // TODO: TestSpecific("TCP", true);
	if (lTestOk)
	{
		lTestOk = TestSpecific("UDP", false);
	}
	return (lTestOk);
}

bool NetworkClientServerTest::TestSpecific(const str& pPrefix, bool pSafe)
{
	str lContext;
	bool lTestOk = true;

	//Logger::SetMainLevelThreashold(LEVEL_ERROR);

	v_set(Cure::GetSettings(), RTVAR_NETWORK_LOGIN_TIMEOUT, 2.0);

	if (lTestOk)
	{
		lContext = pPrefix+" network startup error";
		lTestOk = Network::Start();
		deb_assert(lTestOk);
	}

	// Create client.
	Cure::NetworkClient* lClient = new Cure::NetworkClient(Cure::GetSettings());
	// Make sure server connect fails (server not up yet).
	if (lTestOk)
	{
		lContext = pPrefix+" client1 invalid force connect";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		lClient->Open(":11332");
		lTestOk = !lClient->Connect(":25344", 0.5);
		deb_assert(lTestOk);
	}

	// Create and start server.
	class TestLoginListener: public Cure::NetworkServer::LoginListener
	{
	public:
		TestLoginListener(Cure::UserAccountManager* pUserAccountManager):
			mUserAccountManager(pUserAccountManager)
		{
		}
		Cure::UserAccount::Availability QueryLogin(const Cure::LoginId& pLoginId, Cure::UserAccount::AccountId& pAccountId)
		{
			return (mUserAccountManager->GetUserAccountStatus(pLoginId, pAccountId));
		};
		void OnLogin(Cure::UserConnection*) {};
		void OnLogout(Cure::UserConnection*) {};
	private:
		Cure::UserAccountManager* mUserAccountManager;
	};
	Cure::UserAccountManager* lUserAccountManager = new Cure::MemoryUserAccountManager();
	TestLoginListener lLoginListener(lUserAccountManager);
	Cure::NetworkServer* lServer = new Cure::NetworkServer(Cure::GetSettings(), &lLoginListener);
	if (lTestOk)
	{
		lContext = pPrefix+" start server";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		lTestOk = lServer->Start(":25344");	// Server picks up ID.
		deb_assert(lTestOk);
	}
	// Make sure that client1 can't connect to wrong port.
	if (lTestOk)
	{
		lContext = pPrefix+" invalid client1 port connect";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		lTestOk = lClient->Open(":11332");
		deb_assert(lTestOk);
		lTestOk = !lClient->Connect(":25343", 0.5);
		deb_assert(lTestOk);
	}

	class ServerPoller: public Thread
	{
	public:
		ServerPoller(Cure::NetworkServer* pServer):
			Thread("Server poller"),
			mServer(pServer)
		{
		}
		void Run()
		{
			Cure::Packet* lPacket = mServer->GetPacketFactory()->Allocate();
			lPacket->Release();
			while (!GetStopRequest())
			{
				uint32 lClientId;
				mServer->ReceiveFirstPacket(lPacket, lClientId);
				mServer->SendAll();
				Thread::Sleep(0.001);
			}
			mServer->GetPacketFactory()->Release(lPacket);
		};
		Cure::NetworkServer* mServer;
	};
	ServerPoller lServerThread(lServer);
	lServerThread.Start();

	// Make sure that login without connect fails.
	if (lTestOk)
	{
		lContext = pPrefix+" invalid client1 unconnected login";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		wstr lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(wstr(L"client1"), lPassword);
		lClient->StartConnectLogin(":25344", 0, lUser);	// No connect time = skip connect, just try to login.
		Cure::RemoteStatus lStatus = lClient->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_NO_CONNECTION);
		deb_assert(lTestOk);
	}
	// Make sure that client1 can connect correctly.
	if (lTestOk)
	{
		lContext = pPrefix+" connect client1";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		lTestOk = lClient->Open(":11332");
		deb_assert(lTestOk);
		lTestOk = lClient->Connect(":25344", 0.5);
		deb_assert(lTestOk);
	}
	// Make sure that bad username login is denied.
	if (lTestOk)
	{
		lContext = pPrefix+" invalid client1 username";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		wstr lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(wstr(L"client2"), lPassword);
		lClient->StartConnectLogin("", 0, lUser);	// No connect time = skip connect, just try to login.
		Cure::RemoteStatus lStatus = lClient->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_LOGIN_ERRONOUS_DATA);
		deb_assert(lTestOk);
	}
	// Make sure that client1 connect #2 works.
	if (lTestOk)
	{
		lContext = pPrefix+" connect client1 second time";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		lTestOk = lClient->Open(":11332");
		deb_assert(lTestOk);
		lTestOk = lClient->Connect(":25344", 0.5);
		deb_assert(lTestOk);
	}
	// Make sure that bad password login is denied.
	if (lTestOk)
	{
		lContext = pPrefix+" invalid client1 password";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		wstr lBadPassword(L"feddn");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(wstr(L"client1"), lPassword);
		lClient->StartConnectLogin("", 0, lUser);	// No connect time = skip connect, just try to login.
		Cure::RemoteStatus lStatus = lClient->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_LOGIN_ERRONOUS_DATA);
		deb_assert(lTestOk);
	}
	// Create user on server side.
	if (lTestOk)
	{
		lContext = pPrefix+" create user";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		wstr lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(wstr(L"client1"), lPassword);
		lTestOk = lUserAccountManager->AddUserAccount(lUser);
		deb_assert(lTestOk);
	}
	// Connect+login client.
	if (lTestOk)
	{
		lContext = pPrefix+" connect+login client";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		wstr lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(wstr(L"client1"), lPassword);
		lTestOk = lClient->Open(":11332");
		deb_assert(lTestOk);
		lClient->StartConnectLogin(":25344", 2.0, lUser);
		Cure::RemoteStatus lStatus = lClient->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_OK);
		deb_assert(lTestOk);
	}
	// Second login checks.
	Cure::NetworkClient* lClient2 = new Cure::NetworkClient(Cure::GetSettings());
	// Make sure that client2 can connect correctly.
	if (lTestOk)
	{
		lContext = pPrefix+" connect client2";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		lTestOk = lClient2->Open(":11333");
		deb_assert(lTestOk);
		lTestOk = lClient2->Connect(":25344", 0.5);
		deb_assert(lTestOk);
	}
	// Make sure double login fails.
	if (lTestOk)
	{
		lContext = pPrefix+" invalid double login client";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		wstr lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(wstr(L"client1"), lPassword);
		lClient2->StartConnectLogin("", 0, lUser);
		Cure::RemoteStatus lStatus = lClient2->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_LOGIN_ALREADY);
		deb_assert(lTestOk);
	}
	// Create user #2 on server side.
	if (lTestOk)
	{
		lContext = pPrefix+" create user #2";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		wstr lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(wstr(L"client2"), lPassword);
		lTestOk = lUserAccountManager->AddUserAccount(lUser);
		deb_assert(lTestOk);
	}
	// Make sure second connect+login works.
	if (lTestOk)
	{
		lContext = pPrefix+" connect+login 2, client2";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		wstr lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(wstr(L"client2"), lPassword);
		lTestOk = lClient2->Open(":11333");
		deb_assert(lTestOk);
		lClient2->StartConnectLogin(":25344", 2.0, lUser);
		Cure::RemoteStatus lStatus = lClient2->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_OK);
		deb_assert(lTestOk);
	}

	uint32 lClient2Id = 0;
	if (lTestOk)
	{
		lContext = pPrefix+" second client's ID";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		lClient2Id = lClient2->GetLoginAccountId();
		lTestOk = (lClient2Id >= 1000 && lClient2Id < 0x7FFFFFFF);
		deb_assert(lTestOk);
	}
	uint32 lClientId = 0;
	if (lTestOk)
	{
		lContext = pPrefix+" first client's ID";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		lClientId = lClient->GetLoginAccountId();
		lTestOk = (lClientId >= 1000 && lClientId < 0x7FFFFFFF);
		deb_assert(lTestOk);
	}
	// Done with server thread. We take over from here.
	lServerThread.Join();

	// Try transmission.
	const int lPacketCount = 4;
	if (lTestOk)
	{
		lContext = pPrefix+" client2 send";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		Cure::Packet* lPacket = lClient2->GetPacketFactory()->Allocate();
		lPacket->Release();
		Cure::MessageStatus* lStatus = (Cure::MessageStatus*)lClient2->GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_STATUS);
		lPacket->AddMessage(lStatus);
		lStatus->Store(lPacket, Cure::REMOTE_OK, Cure::MessageStatus::INFO_CHAT, 0, L"Client2ToServer");
		lPacket->StoreHeader();
		for (int x = 0; lTestOk && x < lPacketCount; ++x)
		{
			lTestOk = lClient2->PlaceInSendBuffer(pSafe, lClient2->GetSocket(), lPacket);
			deb_assert(lTestOk);
			if (lTestOk)
			{
				lContext = pPrefix+" client2 flush";
				lTestOk = lClient2->SendAll();
				deb_assert(lTestOk);
			}
		}
		lClient2->GetPacketFactory()->Release(lPacket);
	}
	if (lTestOk)
	{
		lContext = pPrefix+" server receive from client2";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		Thread::Sleep(0.2);
		Cure::Packet* lPacket = lServer->GetPacketFactory()->Allocate();
		uint32 lId = 0xFFFFFFFF;
		lTestOk = (lServer->ReceiveFirstPacket(lPacket, lId) == Cure::NetworkAgent::RECEIVE_OK && lId == lClient2Id);
		deb_assert(lTestOk);
		for (int x = 0; lTestOk && x < lPacketCount; ++x)
		{
			lContext = pPrefix+" server parse client2 packet";
			lTestOk = (lPacket->GetMessageCount() == 1 && lPacket->GetMessageAt(0)->GetType() == Cure::MESSAGE_TYPE_STATUS);
			deb_assert(lTestOk);
			if (lTestOk)
			{
				Cure::MessageStatus* lMessage = (Cure::MessageStatus*)(lPacket->GetMessageAt(0));
				wstr lText;
				lMessage->GetMessageString(lText);
				lTestOk = (lText == L"Client2ToServer");
				deb_assert(lTestOk);
			}
			if (lTestOk)
			{
				if (pSafe)
				{
					Cure::Packet::ParseResult lResult = lPacket->ParseMore();
					if (x == lPacketCount-1)
					{
						lContext = pPrefix+" server parse non-present packet";
						lTestOk = (lResult == Cure::Packet::PARSE_NO_DATA);
					}
					else
					{
						lContext = pPrefix+" server parse next packet";
						lTestOk = (lResult == Cure::Packet::PARSE_OK);
					}
				}
				else
				{
					lPacket->Clear();
					lId = 0xFFFFFFFF;
					if (x == lPacketCount-1)
					{
						lContext = pPrefix+" server receive non-present datagram";
						lTestOk = (lServer->ReceiveFirstPacket(lPacket, lId) == Cure::NetworkAgent::RECEIVE_NO_DATA);
					}
					else
					{
						lContext = pPrefix+" server receive next datagram";
						lTestOk = (lServer->ReceiveFirstPacket(lPacket, lId) == Cure::NetworkAgent::RECEIVE_OK && lId == lClient2Id);
					}
				}
				deb_assert(lTestOk);
			}
		}
		lServer->GetPacketFactory()->Release(lPacket);
	}
	if (lTestOk)
	{
		lContext = pPrefix+" server send to client2";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		Cure::Packet* lPacket = lServer->GetPacketFactory()->Allocate();
		lPacket->Release();
		Cure::MessageStatus* lStatus = (Cure::MessageStatus*)lServer->GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_STATUS);
		lPacket->AddMessage(lStatus);
		lStatus->Store(lPacket, Cure::REMOTE_OK, Cure::MessageStatus::INFO_CHAT, 0, L"BajsOxe");
		lPacket->StoreHeader();
		lTestOk = lServer->PlaceInSendBuffer(pSafe, lPacket, lClient2Id);
		lServer->GetPacketFactory()->Release(lPacket);
		lServer->SendAll();
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = pPrefix+" client2 receive";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		Thread::Sleep(0.2);
		Cure::Packet* lPacket = lClient2->GetPacketFactory()->Allocate();
		lTestOk = (lClient2->ReceiveTimeout(lPacket, 0.3)  == Cure::NetworkAgent::RECEIVE_OK);
		if (lTestOk)
		{
			lTestOk = (lPacket->GetMessageCount() == 1 && lPacket->GetMessageAt(0)->GetType() == Cure::MESSAGE_TYPE_STATUS);
			if (lTestOk)
			{
				Cure::MessageStatus* lMessage = (Cure::MessageStatus*)(lPacket->GetMessageAt(0));
				wstr lText;
				lMessage->GetMessageString(lText);
				lTestOk = (lText == L"BajsOxe");
			}
		}
		lClient2->GetPacketFactory()->Release(lPacket);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = pPrefix+" client1 send";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		Cure::Packet* lPacket = lClient->GetPacketFactory()->Allocate();
		lPacket->Release();
		Cure::MessageStatus* lStatus = (Cure::MessageStatus*)lClient->GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_STATUS);
		lPacket->AddMessage(lStatus);
		lStatus->Store(lPacket, Cure::REMOTE_OK, Cure::MessageStatus::INFO_CHAT, 0, L"?");
		lPacket->StoreHeader();
		lTestOk = lClient->PlaceInSendBuffer(pSafe, lClient->GetSocket(), lPacket);
		if (lTestOk)
		{
			lTestOk = lClient->SendAll();
		}
		lClient->GetPacketFactory()->Release(lPacket);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = pPrefix+" server receive from client1";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		Thread::Sleep(0.2);
		Cure::Packet* lPacket = lServer->GetPacketFactory()->Allocate();
		uint32 lId;
		lTestOk = (lServer->ReceiveFirstPacket(lPacket, lId) == Cure::NetworkAgent::RECEIVE_OK && lId == lClientId);
		/*if (lTestOk)
		{
			lTestOk = lMessage.IsEqual((const uint8*)"123456", 6);
		}*/
		lServer->GetPacketFactory()->Release(lPacket);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = pPrefix+" server send to client1";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		Cure::Packet* lPacket = lServer->GetPacketFactory()->Allocate();
		lPacket->Release();
		Cure::MessageStatus* lStatus = (Cure::MessageStatus*)lServer->GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_STATUS);
		lPacket->AddMessage(lStatus);
		lStatus->Store(lPacket, Cure::REMOTE_OK, Cure::MessageStatus::INFO_CHAT, 0, L"?");
		lPacket->StoreHeader();
		lTestOk = lServer->PlaceInSendBuffer(pSafe, lPacket, lClientId);
		lServer->GetPacketFactory()->Release(lPacket);
		lServer->SendAll();
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = pPrefix+" client1 receive";
		log_volatile(mLog.Debug("---> Testing: "+lContext));
		Thread::Sleep(0.2);
		Cure::Packet* lPacket = lClient->GetPacketFactory()->Allocate();
		lTestOk = (lClient->ReceiveTimeout(lPacket, 0.3) == Cure::NetworkAgent::RECEIVE_OK);
		/*if (lTestOk)
		{
			lTestOk = lMessage.IsEqual((const uint8*)"BajsOxe", 7);
		}*/
		lClient->GetPacketFactory()->Release(lPacket);
		deb_assert(lTestOk);
	}

	delete (lClient2);
	delete (lClient);
	delete (lServer);
	delete (lUserAccountManager);

	if (lTestOk)
	{
		lContext = pPrefix+" network shutdown error";
		lTestOk = Network::Stop();
		deb_assert(lTestOk);
	}

	ReportTestResult(mLog, pPrefix+"ClientServer", lContext, lTestOk);
	return (lTestOk);
}



loginstance(TEST, TerrainTest);
loginstance(TEST, NetworkClientServerTest);



class HiscoreTest
{
public:
	bool Test();
	logclass();
};

bool HiscoreTest::Test()
{
	const std::vector<Logger*> lLogArray = LogType::GetLoggers();
	std::vector<Logger*>::const_iterator x = lLogArray.begin();
	LogLevel lNewLogLevel = LEVEL_LOWEST_TYPE;
	for (; x != lLogArray.end(); ++x)
	{
		(*x)->SetLevelThreashold(lNewLogLevel);
	}

	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = "network startup error";
		lTestOk = Network::Start();
		deb_assert(lTestOk);
	}

	Cure::HiscoreAgent lHiscore("gamehiscore.pixeldoctrine.com", 80, "UnitTest");

	if (lTestOk)
	{
		lContext = "list parse";
		astr a = "{\"offset\":2,\"total_count\":5,\"list\":[{\"name\":\"Jonte\",\"score\":4},{\"name\":\"Bea Sune\",\"score\":34567}]}";
		lTestOk = lHiscore.ParseList(a);
		deb_assert(lHiscore.GetDownloadedList().mOffset == 2);
		deb_assert(lHiscore.GetDownloadedList().mTotalCount == 5);
		deb_assert(lHiscore.GetDownloadedList().mEntryList.size() == 2);
		deb_assert(lHiscore.GetDownloadedList().mEntryList[0].mName == "Jonte");
		deb_assert(lHiscore.GetDownloadedList().mEntryList[0].mScore == 4);
		deb_assert(lHiscore.GetDownloadedList().mEntryList[1].mName == "Bea Sune");
		deb_assert(lHiscore.GetDownloadedList().mEntryList[1].mScore == 34567);
		lHiscore.Close();
		if (lTestOk)
		{
			a = "{ \"offset\" : 13 , \"total_count\" : 5 , \"list\" : [ { \"name\" : \"Mea Culpa~*'-.,;:_\\\"\\\\@#%&\" , \"score\" : 42 } , { \"name\" : \"A\" , \"score\" : 4321 } ] }";
			lTestOk = lHiscore.ParseList(a);
			deb_assert(lHiscore.GetDownloadedList().mOffset == 13);
			deb_assert(lHiscore.GetDownloadedList().mTotalCount == 5);
			deb_assert(lHiscore.GetDownloadedList().mEntryList.size() == 2);
			deb_assert(lHiscore.GetDownloadedList().mEntryList[0].mName == "Mea Culpa~*'-.,;:_\"\\@#%&");
			deb_assert(lHiscore.GetDownloadedList().mEntryList[0].mScore == 42);
			deb_assert(lHiscore.GetDownloadedList().mEntryList[1].mName == "A");
			deb_assert(lHiscore.GetDownloadedList().mEntryList[1].mScore == 4321);
			lHiscore.Close();
		}
		if (lTestOk)
		{
			a = "{ \"total_count\": 5 , \"list\": [ { \"name\": \"Mea Culpa~*'-.,;:_\\\"\\\\@#%&\" , \"score\" :42 } , { \"name\" : \"A\" , \"score\":4321 } ], \"offset\" :13}";
			lTestOk = lHiscore.ParseList(a);
			deb_assert(lHiscore.GetDownloadedList().mOffset == 13);
			deb_assert(lHiscore.GetDownloadedList().mTotalCount == 5);
			deb_assert(lHiscore.GetDownloadedList().mEntryList.size() == 2);
			deb_assert(lHiscore.GetDownloadedList().mEntryList[0].mName == "Mea Culpa~*'-.,;:_\"\\@#%&");
			deb_assert(lHiscore.GetDownloadedList().mEntryList[0].mScore == 42);
			deb_assert(lHiscore.GetDownloadedList().mEntryList[1].mName == "A");
			deb_assert(lHiscore.GetDownloadedList().mEntryList[1].mScore == 4321);
			lHiscore.Close();
		}
		if (lTestOk)
		{
			a = "{\"list\":[ { \"score\" :42,\"name\": \"Mea Culpa~*'-.,;:_\\\"\\\\@#%&\" } , { \"score\":4321,\"name\" : \"A\"}],\"total_count\" :5,\"offset\" :13 }";
			lTestOk = lHiscore.ParseList(a);
			deb_assert(lHiscore.GetDownloadedList().mOffset == 13);
			deb_assert(lHiscore.GetDownloadedList().mTotalCount == 5);
			deb_assert(lHiscore.GetDownloadedList().mEntryList.size() == 2);
			deb_assert(lHiscore.GetDownloadedList().mEntryList[0].mName == "Mea Culpa~*'-.,;:_\"\\@#%&");
			deb_assert(lHiscore.GetDownloadedList().mEntryList[0].mScore == 42);
			deb_assert(lHiscore.GetDownloadedList().mEntryList[1].mName == "A");
			deb_assert(lHiscore.GetDownloadedList().mEntryList[1].mScore == 4321);
			lHiscore.Close();
		}
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "score parse";
		astr a = "{ \"offset\" : 15 }";
		lTestOk = lHiscore.ParseScore(a);
		deb_assert(lHiscore.GetUploadedPlace() == 15);
		lHiscore.Close();
		if (lTestOk)
		{
			a = "{\"offset\":3429}";
			lTestOk = lHiscore.ParseScore(a);
			deb_assert(lHiscore.GetUploadedPlace() == 3429);
			lHiscore.Close();
		}
		deb_assert(lTestOk);
	}

	/*// Upload some scores.
	const tchar* lNames[] = { "high_festiva!", "Rune", "Ojsan Mobrink", "Gregolf Gugerthsson", "Sveodilf", "Matorgh", "Elow", "Mastiffen", };
	for (int y = 0; y < LEPRA_ARRAY_COUNT(lNames); ++y)
	{
		lTestOk = lHiscore.StartUploadingScore("Computer", "Level", "Avatar", lNames[y], Lepra::Random::GetRandomNumber()%1000 + 1000);
		for (int x = 0; x < 50; ++x)
		{
			Thread::Sleep(0.1f);
			Cure::ResourceLoadState lLoadState = lHiscore.Poll();
			lTestOk = (lLoadState == Cure::RESOURCE_LOAD_COMPLETE);
			if (lLoadState != Cure::RESOURCE_LOAD_IN_PROGRESS)
			{
				break;
			}
		}
		deb_assert(lTestOk);
	}*/

	if (lTestOk)
	{
		lContext = "init score download";
		lTestOk = lHiscore.StartDownloadingList("Computer", "Level", "Avatar", 0, 10);
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "downloading score";
		for (int x = 0; x < 50; ++x)
		{
			Thread::Sleep(0.1f);
			Cure::ResourceLoadState lLoadState = lHiscore.Poll();
			lTestOk = (lLoadState == Cure::RESOURCE_LOAD_COMPLETE);
			if (lLoadState != Cure::RESOURCE_LOAD_IN_PROGRESS)
			{
				break;
			}
		}
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "checking score";
		Cure::HiscoreAgent::List lHiscoreList = lHiscore.GetDownloadedList();
		lTestOk = (lHiscoreList.mOffset == 0);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lTestOk = (lHiscoreList.mEntryList.size() == 8);
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (lHiscoreList.mEntryList[0].mName.size() >= 1 && lHiscoreList.mEntryList[0].mName.size() <= 13);
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (lHiscoreList.mEntryList[0].mScore >= 1000 && lHiscoreList.mEntryList[0].mScore < 2000);
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (lHiscoreList.mEntryList[0].mScore >= lHiscoreList.mEntryList[1].mScore &&
				lHiscoreList.mEntryList[1].mScore >= lHiscoreList.mEntryList[2].mScore &&
				lHiscoreList.mEntryList[2].mScore >= lHiscoreList.mEntryList[3].mScore &&
				lHiscoreList.mEntryList[3].mScore >= lHiscoreList.mEntryList[4].mScore);
			deb_assert(lTestOk);
		}
	}

	if (lTestOk)
	{
		lContext = "init score upload";
		lTestOk = lHiscore.StartUploadingScore("Computer", "Level", "Avatar", "high_festiva!", Lepra::Random::GetRandomNumber()%1000 + 1000);
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "uploading score";
		for (int x = 0; x < 50; ++x)
		{
			Thread::Sleep(0.1f);
			Cure::ResourceLoadState lLoadState = lHiscore.Poll();
			lTestOk = (lLoadState == Cure::RESOURCE_LOAD_COMPLETE);
			if (lLoadState != Cure::RESOURCE_LOAD_IN_PROGRESS)
			{
				break;
			}
		}
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "score place";
		int lPosition = lHiscore.GetUploadedPlace();
		lTestOk = (lPosition >= 0 && lPosition <= 7);
		deb_assert(lTestOk);
	}
	Network::Stop();

	ReportTestResult(mLog, "Hiscore", lContext, lTestOk);
	return (lTestOk);
}

loginstance(TEST, HiscoreTest);



bool TestCure()
{
	bool lTestOk = true;
	/*if (lTestOk)
	{
		lTestOk = TestPython(gCLog);
	}*/
	if (lTestOk)
	{
		TestPacker(gCLog);
	}
	if (lTestOk)
	{
		//TerrainTest lTerrainTest;
		//lTestOk = lTerrainTest.Test();
	}
	if (lTestOk)
	{
		NetworkClientServerTest lClientServerTest;
		lTestOk = lClientServerTest.Test();
	}
	if (lTestOk)
	{
		HiscoreTest lHiscoreTest;
		lTestOk = lHiscoreTest.Test();
	}
	return (lTestOk);
}
