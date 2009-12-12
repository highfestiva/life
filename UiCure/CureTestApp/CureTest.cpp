
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include "../../Cure/Include/NetworkClient.h"
#include "../../Cure/Include/NetworkServer.h"
#include "../../Cure/Include/Packet.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TerrainManager.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Packer.h"
#include "../../Lepra/Include/SystemManager.h"



class CureTest{};
static Lepra::LogDecorator gCLog(Lepra::LogType::GetLog(Lepra::LogType::SUB_TEST), typeid(CureTest));
void ReportTestResult(const Lepra::LogDecorator& pLog, const Lepra::String& pTestName, const Lepra::String& pContext, bool pbResult);



bool TestPacker(const Lepra::LogDecorator& pAccount)
{
	Lepra::String lContext;
	bool lTestOk = true;

	Lepra::uint8 lRawData[1024];
	if (lTestOk)
	{
		lContext = _T("pack unicode");
		lTestOk = (Lepra::PackerUnicodeString::Pack(lRawData, L"ABC") == 10);
		assert(lTestOk);
		if (lTestOk)
		{
			lTestOk = (lRawData[0] == 4 && lRawData[1] == 0 && lRawData[2] == 'A' &&
				lRawData[3] == 0 && lRawData[4] == 'B' && lRawData[5] == 0 &&
				lRawData[6] == 'C' && lRawData[7] == 0 && lRawData[8] == 0 &&
				lRawData[9] == 0);
			assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("unpack unicode");
		Lepra::UnicodeString lUnpacked;
		lTestOk = (Lepra::PackerUnicodeString::Unpack(&lUnpacked, lRawData, 10) == 10);
		assert(lTestOk);
		if (lTestOk)
		{
			lTestOk = (lUnpacked == L"ABC");
			assert(lTestOk);
		}
	}

	ReportTestResult(pAccount, _T("Packer"), lContext, lTestOk);
	return (lTestOk);
}

class TerrainTest
{
public:
	bool Test();

private:
	LOG_CLASS_DECLARE();
};

bool TerrainTest::Test()
{
	Lepra::String lContext;
	bool lTestOk = true;

	Cure::ResourceManager* lResourceManager = new Cure::ResourceManager(1);
	lResourceManager->InitDefault();
	Cure::TerrainManager lTerrainManager(lResourceManager);	// Creates the TerrainFunctionManager and throws it at the resource manager.

	// Test loading terrain by simply placing a camera.
	if (lTestOk)
	{
		lContext = _T("load terrain");
		lTerrainManager.AddCamera(Lepra::Vector3DF(0, 0, 0), 10000);
		Lepra::Thread::Sleep(0.2);
		lResourceManager->Tick();
		int lOkCount = 0;
		int lErrorCount = 0;
		lTerrainManager.GetLoadCount(lOkCount, lErrorCount);
		lTestOk = (lOkCount == 1 && lErrorCount == 0);
		assert(lTestOk);
	}

	lTerrainManager.Clear();
	delete (lResourceManager);

	Lepra::SystemManager::SetQuitRequest(false);

	ReportTestResult(mLog, _T("TerrainManager"), lContext, lTestOk);
	return (lTestOk);
}



class NetworkClientServerTest
{
public:
	bool Test();

private:
	bool TestSpecific(const Lepra::String& pPrefix, bool pSafe);

	LOG_CLASS_DECLARE();
};

bool NetworkClientServerTest::Test()
{
	bool lTestOk = TestSpecific(_T("TCP"), true);
	if (lTestOk)
	{
		lTestOk = TestSpecific(_T("UDP"), false);
	}
	return (lTestOk);
}

bool NetworkClientServerTest::TestSpecific(const Lepra::String& pPrefix, bool pSafe)
{
	Lepra::String lContext;
	bool lTestOk = true;

	//Lepra::Log::SetMainLevelThreashold(Lepra::Log::LEVEL_ERROR);

	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_NETWORK_LOGIN_TIMEOUT, 2.0);

	if (lTestOk)
	{
		lContext = pPrefix+_T(" network startup error");
		lTestOk = Lepra::Network::Start();
		assert(lTestOk);
	}

	// Create client.
	Cure::NetworkClient* lClient = new Cure::NetworkClient(Cure::GetSettings());
	// Make sure server connect fails (server not up yet).
	if (lTestOk)
	{
		lContext = pPrefix+_T(" client1 invalid force connect");
		lTestOk = !lClient->Connect(_T("localhost:11332"), _T("localhost:25344"), 0.5);
		assert(lTestOk);
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
		lContext = pPrefix+_T(" start server");
		lTestOk = lServer->Start(_T("localhost:25344"));	// Server picks up ID.
		assert(lTestOk);
	}
	// Make sure that client1 can't connect to wrong port.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" invalid client1 port connect");
		lTestOk = !lClient->Connect(_T("localhost:11332"), _T("localhost:25343"), 0.5);
		assert(lTestOk);
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
	ServerPoller lServerThread(lServer);
	lServerThread.Start();

	// Make sure that login without connect fails.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" invalid client1 unconnected login");
		Lepra::UnicodeString lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(Lepra::UnicodeString(L"client1"), lPassword);
		lClient->StartConnectLogin(_T(""), -1, lUser);
		Cure::RemoteStatus lStatus = lClient->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_NO_CONNECTION);
		assert(lTestOk);
	}
	// Make sure that client1 can connect correctly.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" connect client1");
		lTestOk = lClient->Connect(_T("localhost:11332"), _T("localhost:25344"), 0.5);
		assert(lTestOk);
	}
	// Make sure that bad username login is denied.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" invalid client1 username");
		Lepra::UnicodeString lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(Lepra::UnicodeString(L"client2"), lPassword);
		lClient->StartConnectLogin(_T(""), -1, lUser);
		Cure::RemoteStatus lStatus = lClient->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_LOGIN_ERRONOUS_DATA);
		assert(lTestOk);
	}
	// Make sure that client1 connect #2 works.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" connect client1 second time");
		lTestOk = lClient->Connect(_T("localhost:11332"), _T("localhost:25344"), 0.5);
		assert(lTestOk);
	}
	// Make sure that bad password login is denied.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" invalid client1 password");
		Lepra::UnicodeString lBadPassword(L"feddn");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(Lepra::UnicodeString(L"client1"), lPassword);
		lClient->StartConnectLogin(_T(""), -1, lUser);
		Cure::RemoteStatus lStatus = lClient->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_LOGIN_ERRONOUS_DATA);
		assert(lTestOk);
	}
	// Create user on server side.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" create user");
		Lepra::UnicodeString lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(Lepra::UnicodeString(L"client1"), lPassword);
		lTestOk = lUserAccountManager->AddUserAccount(lUser);
		assert(lTestOk);
	}
	// Connect+login client.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" connect+login client");
		Lepra::UnicodeString lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(Lepra::UnicodeString(L"client1"), lPassword);
		lClient->StartConnectLogin(_T("localhost:25344"), 2.0, lUser);
		Cure::RemoteStatus lStatus = lClient->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_OK);
		assert(lTestOk);
	}
	// Second login checks.
	Cure::NetworkClient* lClient2 = new Cure::NetworkClient(Cure::GetSettings());
	// Make sure that client2 can connect correctly.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" connect client2");
		lTestOk = lClient2->Connect(_T("localhost:11333"), _T("localhost:25344"), 0.5);
		assert(lTestOk);
	}
	// Make sure double login fails.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" invalid double login client");
		Lepra::UnicodeString lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(Lepra::UnicodeString(L"client1"), lPassword);
		lClient2->StartConnectLogin(_T(""), -1, lUser);
		Cure::RemoteStatus lStatus = lClient2->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_LOGIN_ALREADY);
		assert(lTestOk);
	}
	// Create user #2 on server side.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" create user #2");
		Lepra::UnicodeString lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(Lepra::UnicodeString(L"client2"), lPassword);
		lTestOk = lUserAccountManager->AddUserAccount(lUser);
		assert(lTestOk);
	}
	// Make sure second connect+login works.
	if (lTestOk)
	{
		lContext = pPrefix+_T(" connect+login 2, client2");
		Lepra::UnicodeString lBadPassword(L"feddo");
		Cure::MangledPassword lPassword(lBadPassword);
		Cure::LoginId lUser(Lepra::UnicodeString(L"client2"), lPassword);
		lClient2->StartConnectLogin(_T("localhost:25344"), 2.0, lUser);
		Cure::RemoteStatus lStatus = lClient2->WaitLogin();
		lTestOk = (lStatus == Cure::REMOTE_OK);
		assert(lTestOk);
	}

	Lepra::uint32 lClient2Id = 0;
	if (lTestOk)
	{
		lClient2Id = lClient2->GetLoginAccountId();
		lTestOk = (lClient2Id >= 1000 && lClient2Id < 0x7FFFFFFF);
		assert(lTestOk);
	}
	Lepra::uint32 lClientId = 0;
	if (lTestOk)
	{
		lClientId = lClient->GetLoginAccountId();
		lTestOk = (lClientId >= 1000 && lClientId < 0x7FFFFFFF);
		assert(lTestOk);
	}
	// Done with server thread. We take over from here.
	lServerThread.Join();

	// Try transmission.
	const int lPacketCount = 4;
	if (lTestOk)
	{
		lContext = pPrefix+_T(" client2 send");
		Cure::Packet* lPacket = lClient2->GetPacketFactory()->Allocate();
		lPacket->Release();
		Cure::MessageStatus* lStatus = (Cure::MessageStatus*)lClient2->GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_STATUS);
		lPacket->AddMessage(lStatus);
		lStatus->Store(lPacket, Cure::REMOTE_OK, 0, L"Client2ToServer");
		lPacket->StoreHeader();
		for (int x = 0; lTestOk && x < lPacketCount; ++x)
		{
			lTestOk = lClient2->PlaceInSendBuffer(pSafe, lClient2->GetSocket(), lPacket);
			assert(lTestOk);
			if (lTestOk)
			{
				lContext = pPrefix+_T(" client2 flush");
				lTestOk = lClient2->SendAll();
				assert(lTestOk);
			}
		}
		lClient2->GetPacketFactory()->Release(lPacket);
	}
	if (lTestOk)
	{
		lContext = pPrefix+_T(" server receive from client2");
		Lepra::Thread::Sleep(0.2);
		Cure::Packet* lPacket = lServer->GetPacketFactory()->Allocate();
		Lepra::uint32 lId = 0xFFFFFFFF;
		lTestOk = (lServer->ReceiveFirstPacket(lPacket, lId) == Cure::NetworkAgent::RECEIVE_OK && lId == lClient2Id);
		assert(lTestOk);
		for (int x = 0; lTestOk && x < lPacketCount; ++x)
		{
			lContext = pPrefix+_T(" server parse client2 packet");
			lTestOk = (lPacket->GetMessageCount() == 1 && lPacket->GetMessageAt(0)->GetType() == Cure::MESSAGE_TYPE_STATUS);
			assert(lTestOk);
			if (lTestOk)
			{
				Cure::MessageStatus* lMessage = (Cure::MessageStatus*)(lPacket->GetMessageAt(0));
				Lepra::UnicodeString lText;
				lMessage->GetMessageString(lText);
				lTestOk = (lText == L"Client2ToServer");
				assert(lTestOk);
			}
			if (lTestOk)
			{
				if (pSafe)
				{
					Cure::Packet::ParseResult lResult = lPacket->ParseMore();
					if (x == lPacketCount-1)
					{
						lContext = pPrefix+_T(" server parse non-present packet");
						lTestOk = (lResult == Cure::Packet::PARSE_NO_DATA);
					}
					else
					{
						lContext = pPrefix+_T(" server parse next packet");
						lTestOk = (lResult == Cure::Packet::PARSE_OK);
					}
				}
				else
				{
					lPacket->Clear();
					lId = 0xFFFFFFFF;
					if (x == lPacketCount-1)
					{
						lContext = pPrefix+_T(" server receive non-present datagram");
						lTestOk = (lServer->ReceiveFirstPacket(lPacket, lId) == Cure::NetworkAgent::RECEIVE_NO_DATA);
					}
					else
					{
						lContext = pPrefix+_T(" server receive next datagram");
						lTestOk = (lServer->ReceiveFirstPacket(lPacket, lId) == Cure::NetworkAgent::RECEIVE_OK && lId == lClient2Id);
					}
				}
				assert(lTestOk);
			}
		}
		lServer->GetPacketFactory()->Release(lPacket);
	}
	if (lTestOk)
	{
		lContext = pPrefix+_T(" server send to client2");
		Cure::Packet* lPacket = lServer->GetPacketFactory()->Allocate();
		lPacket->Release();
		Cure::MessageStatus* lStatus = (Cure::MessageStatus*)lServer->GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_STATUS);
		lPacket->AddMessage(lStatus);
		lStatus->Store(lPacket, Cure::REMOTE_OK, 0, L"BajsOxe");
		lPacket->StoreHeader();
		lTestOk = lServer->PlaceInSendBuffer(pSafe, lPacket, lClient2Id);
		lServer->GetPacketFactory()->Release(lPacket);
		lServer->SendAll();
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = pPrefix+_T(" client2 receive");
		Lepra::Thread::Sleep(0.2);
		Cure::Packet* lPacket = lClient2->GetPacketFactory()->Allocate();
		lTestOk = (lClient2->ReceiveTimeout(lPacket, 0.3)  == Cure::NetworkAgent::RECEIVE_OK);
		if (lTestOk)
		{
			lTestOk = (lPacket->GetMessageCount() == 1 && lPacket->GetMessageAt(0)->GetType() == Cure::MESSAGE_TYPE_STATUS);
			if (lTestOk)
			{
				Cure::MessageStatus* lMessage = (Cure::MessageStatus*)(lPacket->GetMessageAt(0));
				Lepra::UnicodeString lText;
				lMessage->GetMessageString(lText);
				lTestOk = (lText == L"BajsOxe");
			}
		}
		lClient2->GetPacketFactory()->Release(lPacket);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = pPrefix+_T(" client1 send");
		Cure::Packet* lPacket = lClient->GetPacketFactory()->Allocate();
		lPacket->Release();
		Cure::MessageStatus* lStatus = (Cure::MessageStatus*)lClient->GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_STATUS);
		lPacket->AddMessage(lStatus);
		lStatus->Store(lPacket, Cure::REMOTE_OK, 0, L"?");
		lPacket->StoreHeader();
		lTestOk = lClient->PlaceInSendBuffer(pSafe, lClient->GetSocket(), lPacket);
		if (lTestOk)
		{
			lTestOk = lClient->SendAll();
		}
		lClient->GetPacketFactory()->Release(lPacket);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = pPrefix+_T(" server receive from client1");
		Lepra::Thread::Sleep(0.2);
		Cure::Packet* lPacket = lServer->GetPacketFactory()->Allocate();
		Lepra::uint32 lId;
		lTestOk = (lServer->ReceiveFirstPacket(lPacket, lId) == Cure::NetworkAgent::RECEIVE_OK && lId == lClientId);
		/*if (lTestOk)
		{
			lTestOk = lMessage.IsEqual((const Lepra::uint8*)"123456", 6);
		}*/
		lServer->GetPacketFactory()->Release(lPacket);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = pPrefix+_T(" server send to client1");
		Cure::Packet* lPacket = lServer->GetPacketFactory()->Allocate();
		lPacket->Release();
		Cure::MessageStatus* lStatus = (Cure::MessageStatus*)lServer->GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_STATUS);
		lPacket->AddMessage(lStatus);
		lStatus->Store(lPacket, Cure::REMOTE_OK, 0, L"?");
		lPacket->StoreHeader();
		lTestOk = lServer->PlaceInSendBuffer(pSafe, lPacket, lClientId);
		lServer->GetPacketFactory()->Release(lPacket);
		lServer->SendAll();
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = pPrefix+_T(" client1 receive");
		Lepra::Thread::Sleep(0.2);
		Cure::Packet* lPacket = lClient->GetPacketFactory()->Allocate();
		lTestOk = (lClient->ReceiveTimeout(lPacket, 0.3) == Cure::NetworkAgent::RECEIVE_OK);
		/*if (lTestOk)
		{
			lTestOk = lMessage.IsEqual((const Lepra::uint8*)"BajsOxe", 7);
		}*/
		lClient->GetPacketFactory()->Release(lPacket);
		assert(lTestOk);
	}

	delete (lClient2);
	delete (lClient);
	delete (lServer);
	delete (lUserAccountManager);

	if (lTestOk)
	{
		lContext = pPrefix+_T(" network shutdown error");
		lTestOk = Lepra::Network::Stop();
		assert(lTestOk);
	}

	ReportTestResult(mLog, pPrefix+_T("ClientServer"), lContext, lTestOk);
	return (lTestOk);
}



LOG_CLASS_DEFINE(TEST, TerrainTest);
LOG_CLASS_DEFINE(TEST, NetworkClientServerTest);



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
		TerrainTest lTerrainTest;
		lTestOk = lTerrainTest.Test();
	}
	if (lTestOk)
	{
		NetworkClientServerTest lClientServerTest;
		lTestOk = lClientServerTest.Test();
	}
	return (lTestOk);
}
