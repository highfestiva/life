
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/lepraassert.h"
#include "../../cure/include/hiscoreagent.h"
#include "../../cure/include/networkclient.h"
#include "../../cure/include/networkserver.h"
#include "../../cure/include/packet.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/terrainmanager.h"
#include "../../lepra/include/logger.h"
#include "../../lepra/include/network.h"
#include "../../lepra/include/packer.h"
#include "../../lepra/include/random.h"
#include "../../lepra/include/systemmanager.h"



using namespace lepra;

class CureTest{};
static LogDecorator gCLog(LogType::GetLogger(LogType::kTest), typeid(CureTest));
void ReportTestResult(const LogDecorator& log, const str& test_name, const str& context, bool result);



bool TestPacker(const LogDecorator& account) {
	str _context;
	bool test_ok = true;

	uint8 raw_data[1024];
	if (test_ok) {
		_context = "pack unicode";
		test_ok = (PackerUnicodeString::Pack(raw_data, "ABC") == 8);
		deb_assert(test_ok);
		if (test_ok) {
			test_ok = (raw_data[0] == 4 && raw_data[1] == 0 && raw_data[2] == 'A' &&
				raw_data[3] == 'B' && raw_data[4] == 'C' && raw_data[5] == 0);
			deb_assert(test_ok);
		}
	}
	if (test_ok) {
		_context = "unpack unicode";
		str unpacked;
		test_ok = (PackerUnicodeString::Unpack(unpacked, raw_data, 8) == 8);
		deb_assert(test_ok);
		if (test_ok) {
			test_ok = (unpacked == "ABC");
			deb_assert(test_ok);
		}
	}

	ReportTestResult(account, "Packer", _context, test_ok);
	return (test_ok);
}

class TerrainTest {
public:
	bool Test();

private:
	logclass();
};

bool TerrainTest::Test() {
	str _context;
	bool test_ok = true;

	cure::ResourceManager* resource_manager = new cure::ResourceManager(1);
	resource_manager->InitDefault();
	cure::TerrainManager terrain_manager(resource_manager);	// Creates the TerrainFunctionManager and throws it at the resource manager.

	// Test loading terrain by simply placing a camera.
	if (test_ok) {
		_context = "load terrain";
		terrain_manager.AddCamera(vec3(0, 0, 0), 10000);
		Thread::Sleep(0.2);
		resource_manager->Tick();
		int ok_count = 0;
		int error_count = 0;
		terrain_manager.GetLoadCount(ok_count, error_count);
		test_ok = (ok_count == 1 && error_count == 0);
		deb_assert(test_ok);
	}

	terrain_manager.Clear();
	delete (resource_manager);

	SystemManager::AddQuitRequest(-1);

	ReportTestResult(log_, "TerrainManager", _context, test_ok);
	return (test_ok);
}



class NetworkClientServerTest {
public:
	bool Test();

private:
	bool TestSpecific(const str& prefix, bool safe);

	logclass();
};

bool NetworkClientServerTest::Test() {
	bool test_ok = true; // TODO: TestSpecific("TCP", true);
	if (test_ok) {
		test_ok = TestSpecific("UDP", false);
	}
	return (test_ok);
}

bool NetworkClientServerTest::TestSpecific(const str& prefix, bool safe) {
	str _context;
	bool test_ok = true;

	//Logger::SetMainLevelThreashold(kLevelError);

	v_set(cure::GetSettings(), kRtvarNetworkLoginTimeout, 2.0);

	if (test_ok) {
		_context = prefix+" network startup error";
		test_ok = Network::Start();
		deb_assert(test_ok);
	}

	// Create client.
	cure::NetworkClient* client = new cure::NetworkClient(cure::GetSettings());
	// Make sure server connect fails (server not up yet).
	if (test_ok) {
		_context = prefix+" client1 invalid force connect";
		log_volatile(log_.Debug("---> Testing: "+_context));
		client->Open(":11332");
		test_ok = !client->Connect(":25344", 0.5);
		deb_assert(test_ok);
	}

	// Create and start server.
	class TestLoginListener: public cure::NetworkServer::LoginListener {
	public:
		TestLoginListener(cure::UserAccountManager* user_account_manager):
			user_account_manager_(user_account_manager) {
		}
		cure::UserAccount::Availability QueryLogin(const cure::LoginId& login_id, cure::UserAccount::AccountId& account_id) {
			return (user_account_manager_->GetUserAccountStatus(login_id, account_id));
		};
		void OnLogin(cure::UserConnection*) {};
		void OnLogout(cure::UserConnection*) {};
	private:
		cure::UserAccountManager* user_account_manager_;
	};
	cure::UserAccountManager* _user_account_manager = new cure::MemoryUserAccountManager();
	TestLoginListener login_listener(_user_account_manager);
	cure::NetworkServer* _server = new cure::NetworkServer(cure::GetSettings(), &login_listener);
	if (test_ok) {
		_context = prefix+" start server";
		log_volatile(log_.Debug("---> Testing: "+_context));
		test_ok = _server->Start(":25344");	// Server picks up ID.
		deb_assert(test_ok);
	}
	// Make sure that client1 can't connect to wrong port.
	if (test_ok) {
		_context = prefix+" invalid client1 port connect";
		log_volatile(log_.Debug("---> Testing: "+_context));
		test_ok = client->Open(":11332");
		deb_assert(test_ok);
		test_ok = !client->Connect(":25343", 0.5);
		deb_assert(test_ok);
	}

	class ServerPoller: public Thread {
	public:
		ServerPoller(cure::NetworkServer* server):
			Thread("Server poller"),
			server_(server) {
		}
		void Run() {
			cure::Packet* packet = server_->GetPacketFactory()->Allocate();
			packet->Release();
			while (!GetStopRequest()) {
				uint32 client_id;
				server_->ReceiveFirstPacket(packet, client_id);
				server_->SendAll();
				Thread::Sleep(0.001);
			}
			server_->GetPacketFactory()->Release(packet);
		};
		cure::NetworkServer* server_;
	};
	ServerPoller server_thread(_server);
	server_thread.Start();

	// Make sure that login without connect fails.
	if (test_ok) {
		_context = prefix+" invalid client1 unconnected login";
		log_volatile(log_.Debug("---> Testing: "+_context));
		str bad_password("feddo");
		cure::MangledPassword password(bad_password);
		cure::LoginId user(str("client1"), password);
		client->StartConnectLogin(":25344", 0, user);	// No connect time = skip connect, just try to login.
		cure::RemoteStatus status = client->WaitLogin();
		test_ok = (status == cure::kRemoteNoConnection);
		deb_assert(test_ok);
	}
	// Make sure that client1 can connect correctly.
	if (test_ok) {
		_context = prefix+" connect client1";
		log_volatile(log_.Debug("---> Testing: "+_context));
		test_ok = client->Open(":11332");
		deb_assert(test_ok);
		test_ok = client->Connect(":25344", 0.5);
		deb_assert(test_ok);
	}
	// Make sure that bad username login is denied.
	if (test_ok) {
		_context = prefix+" invalid client1 username";
		log_volatile(log_.Debug("---> Testing: "+_context));
		str bad_password("feddo");
		cure::MangledPassword password(bad_password);
		cure::LoginId user(str("client2"), password);
		client->StartConnectLogin("", 0, user);	// No connect time = skip connect, just try to login.
		cure::RemoteStatus status = client->WaitLogin();
		test_ok = (status == cure::kRemoteLoginErronousData);
		deb_assert(test_ok);
	}
	// Make sure that client1 connect #2 works.
	if (test_ok) {
		_context = prefix+" connect client1 second time";
		log_volatile(log_.Debug("---> Testing: "+_context));
		test_ok = client->Open(":11332");
		deb_assert(test_ok);
		test_ok = client->Connect(":25344", 0.5);
		deb_assert(test_ok);
	}
	// Make sure that bad password login is denied.
	if (test_ok) {
		_context = prefix+" invalid client1 password";
		log_volatile(log_.Debug("---> Testing: "+_context));
		str bad_password("feddn");
		cure::MangledPassword password(bad_password);
		cure::LoginId user(str("client1"), password);
		client->StartConnectLogin("", 0, user);	// No connect time = skip connect, just try to login.
		cure::RemoteStatus status = client->WaitLogin();
		test_ok = (status == cure::kRemoteLoginErronousData);
		deb_assert(test_ok);
	}
	// Create user on server side.
	if (test_ok) {
		_context = prefix+" create user";
		log_volatile(log_.Debug("---> Testing: "+_context));
		str bad_password("feddo");
		cure::MangledPassword password(bad_password);
		cure::LoginId user(str("client1"), password);
		test_ok = _user_account_manager->AddUserAccount(user);
		deb_assert(test_ok);
	}
	// Connect+login client.
	if (test_ok) {
		_context = prefix+" connect+login client";
		log_volatile(log_.Debug("---> Testing: "+_context));
		str bad_password("feddo");
		cure::MangledPassword password(bad_password);
		cure::LoginId user(str("client1"), password);
		test_ok = client->Open(":11332");
		deb_assert(test_ok);
		client->StartConnectLogin(":25344", 2.0, user);
		cure::RemoteStatus status = client->WaitLogin();
		test_ok = (status == cure::kRemoteOk);
		deb_assert(test_ok);
	}
	// Second login checks.
	cure::NetworkClient* client2 = new cure::NetworkClient(cure::GetSettings());
	// Make sure that client2 can connect correctly.
	if (test_ok) {
		_context = prefix+" connect client2";
		log_volatile(log_.Debug("---> Testing: "+_context));
		test_ok = client2->Open(":11333");
		deb_assert(test_ok);
		test_ok = client2->Connect(":25344", 0.5);
		deb_assert(test_ok);
	}
	// Make sure double login fails.
	if (test_ok) {
		_context = prefix+" invalid double login client";
		log_volatile(log_.Debug("---> Testing: "+_context));
		str bad_password("feddo");
		cure::MangledPassword password(bad_password);
		cure::LoginId user(str("client1"), password);
		client2->StartConnectLogin("", 0, user);
		cure::RemoteStatus status = client2->WaitLogin();
		test_ok = (status == cure::kRemoteLoginAlready);
		deb_assert(test_ok);
	}
	// Create user #2 on server side.
	if (test_ok) {
		_context = prefix+" create user #2";
		log_volatile(log_.Debug("---> Testing: "+_context));
		str bad_password("feddo");
		cure::MangledPassword password(bad_password);
		cure::LoginId user(str("client2"), password);
		test_ok = _user_account_manager->AddUserAccount(user);
		deb_assert(test_ok);
	}
	// Make sure second connect+login works.
	if (test_ok) {
		_context = prefix+" connect+login 2, client2";
		log_volatile(log_.Debug("---> Testing: "+_context));
		str bad_password("feddo");
		cure::MangledPassword password(bad_password);
		cure::LoginId user(str("client2"), password);
		test_ok = client2->Open(":11333");
		deb_assert(test_ok);
		client2->StartConnectLogin(":25344", 2.0, user);
		cure::RemoteStatus status = client2->WaitLogin();
		test_ok = (status == cure::kRemoteOk);
		deb_assert(test_ok);
	}

	uint32 client2_id = 0;
	if (test_ok) {
		_context = prefix+" second client's ID";
		log_volatile(log_.Debug("---> Testing: "+_context));
		client2_id = client2->GetLoginAccountId();
		test_ok = (client2_id >= 1000 && client2_id < 0x7FFFFFFF);
		deb_assert(test_ok);
	}
	uint32 client_id = 0;
	if (test_ok) {
		_context = prefix+" first client's ID";
		log_volatile(log_.Debug("---> Testing: "+_context));
		client_id = client->GetLoginAccountId();
		test_ok = (client_id >= 1000 && client_id < 0x7FFFFFFF);
		deb_assert(test_ok);
	}
	// Done with server thread. We take over from here.
	server_thread.Join();

	// Try transmission.
	const int packet_count = 4;
	if (test_ok) {
		_context = prefix+" client2 send";
		log_volatile(log_.Debug("---> Testing: "+_context));
		cure::Packet* packet = client2->GetPacketFactory()->Allocate();
		packet->Release();
		cure::MessageStatus* status = (cure::MessageStatus*)client2->GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeStatus);
		packet->AddMessage(status);
		status->Store(packet, cure::kRemoteOk, cure::MessageStatus::kInfoChat, 0, "Client2ToServer");
		packet->StoreHeader();
		for (int x = 0; test_ok && x < packet_count; ++x) {
			test_ok = client2->PlaceInSendBuffer(safe, client2->GetSocket(), packet);
			deb_assert(test_ok);
			if (test_ok) {
				_context = prefix+" client2 flush";
				test_ok = client2->SendAll();
				deb_assert(test_ok);
			}
		}
		client2->GetPacketFactory()->Release(packet);
	}
	if (test_ok) {
		_context = prefix+" server receive from client2";
		log_volatile(log_.Debug("---> Testing: "+_context));
		Thread::Sleep(0.2);
		cure::Packet* packet = _server->GetPacketFactory()->Allocate();
		uint32 id = 0xFFFFFFFF;
		test_ok = (_server->ReceiveFirstPacket(packet, id) == cure::NetworkAgent::kReceiveOk && id == client2_id);
		deb_assert(test_ok);
		for (int x = 0; test_ok && x < packet_count; ++x) {
			_context = prefix+" server parse client2 packet";
			test_ok = (packet->GetMessageCount() == 1 && packet->GetMessageAt(0)->GetType() == cure::kMessageTypeStatus);
			deb_assert(test_ok);
			if (test_ok) {
				cure::MessageStatus* message = (cure::MessageStatus*)(packet->GetMessageAt(0));
				str text;
				message->GetMessageString(text);
				test_ok = (text == "Client2ToServer");
				deb_assert(test_ok);
			}
			if (test_ok) {
				if (safe) {
					cure::Packet::ParseResult result = packet->ParseMore();
					if (x == packet_count-1) {
						_context = prefix+" server parse non-present packet";
						test_ok = (result == cure::Packet::kParseNoData);
					} else {
						_context = prefix+" server parse next packet";
						test_ok = (result == cure::Packet::kParseOk);
					}
				} else {
					packet->Clear();
					id = 0xFFFFFFFF;
					if (x == packet_count-1) {
						_context = prefix+" server receive non-present datagram";
						test_ok = (_server->ReceiveFirstPacket(packet, id) == cure::NetworkAgent::kReceiveNoData);
					} else {
						_context = prefix+" server receive next datagram";
						test_ok = (_server->ReceiveFirstPacket(packet, id) == cure::NetworkAgent::kReceiveOk && id == client2_id);
					}
				}
				deb_assert(test_ok);
			}
		}
		_server->GetPacketFactory()->Release(packet);
	}
	if (test_ok) {
		_context = prefix+" server send to client2";
		log_volatile(log_.Debug("---> Testing: "+_context));
		cure::Packet* packet = _server->GetPacketFactory()->Allocate();
		packet->Release();
		cure::MessageStatus* status = (cure::MessageStatus*)_server->GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeStatus);
		packet->AddMessage(status);
		status->Store(packet, cure::kRemoteOk, cure::MessageStatus::kInfoChat, 0, "BajsOxe");
		packet->StoreHeader();
		test_ok = _server->PlaceInSendBuffer(safe, packet, client2_id);
		_server->GetPacketFactory()->Release(packet);
		_server->SendAll();
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = prefix+" client2 receive";
		log_volatile(log_.Debug("---> Testing: "+_context));
		Thread::Sleep(0.2);
		cure::Packet* packet = client2->GetPacketFactory()->Allocate();
		test_ok = (client2->ReceiveTimeout(packet, 0.3)  == cure::NetworkAgent::kReceiveOk);
		if (test_ok) {
			test_ok = (packet->GetMessageCount() == 1 && packet->GetMessageAt(0)->GetType() == cure::kMessageTypeStatus);
			if (test_ok) {
				cure::MessageStatus* message = (cure::MessageStatus*)(packet->GetMessageAt(0));
				str text;
				message->GetMessageString(text);
				test_ok = (text == "BajsOxe");
			}
		}
		client2->GetPacketFactory()->Release(packet);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = prefix+" client1 send";
		log_volatile(log_.Debug("---> Testing: "+_context));
		cure::Packet* packet = client->GetPacketFactory()->Allocate();
		packet->Release();
		cure::MessageStatus* status = (cure::MessageStatus*)client->GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeStatus);
		packet->AddMessage(status);
		status->Store(packet, cure::kRemoteOk, cure::MessageStatus::kInfoChat, 0, "?");
		packet->StoreHeader();
		test_ok = client->PlaceInSendBuffer(safe, client->GetSocket(), packet);
		if (test_ok) {
			test_ok = client->SendAll();
		}
		client->GetPacketFactory()->Release(packet);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = prefix+" server receive from client1";
		log_volatile(log_.Debug("---> Testing: "+_context));
		Thread::Sleep(0.2);
		cure::Packet* packet = _server->GetPacketFactory()->Allocate();
		uint32 id;
		test_ok = (_server->ReceiveFirstPacket(packet, id) == cure::NetworkAgent::kReceiveOk && id == client_id);
		/*if (test_ok) {
			test_ok = message.IsEqual((const uint8*)"123456", 6);
		}*/
		_server->GetPacketFactory()->Release(packet);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = prefix+" server send to client1";
		log_volatile(log_.Debug("---> Testing: "+_context));
		cure::Packet* packet = _server->GetPacketFactory()->Allocate();
		packet->Release();
		cure::MessageStatus* status = (cure::MessageStatus*)_server->GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeStatus);
		packet->AddMessage(status);
		status->Store(packet, cure::kRemoteOk, cure::MessageStatus::kInfoChat, 0, "?");
		packet->StoreHeader();
		test_ok = _server->PlaceInSendBuffer(safe, packet, client_id);
		_server->GetPacketFactory()->Release(packet);
		_server->SendAll();
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = prefix+" client1 receive";
		log_volatile(log_.Debug("---> Testing: "+_context));
		Thread::Sleep(0.2);
		cure::Packet* packet = client->GetPacketFactory()->Allocate();
		test_ok = (client->ReceiveTimeout(packet, 0.3) == cure::NetworkAgent::kReceiveOk);
		/*if (test_ok) {
			test_ok = message.IsEqual((const uint8*)"BajsOxe", 7);
		}*/
		client->GetPacketFactory()->Release(packet);
		deb_assert(test_ok);
	}

	delete (client2);
	delete (client);
	delete (_server);
	delete (_user_account_manager);

	if (test_ok) {
		_context = prefix+" network shutdown error";
		test_ok = Network::Stop();
		deb_assert(test_ok);
	}

	ReportTestResult(log_, prefix+"ClientServer", _context, test_ok);
	return (test_ok);
}



loginstance(kTest, TerrainTest);
loginstance(kTest, NetworkClientServerTest);



class HiscoreTest {
public:
	bool Test();
	logclass();
};

bool HiscoreTest::Test() {
	const std::vector<Logger*> log_array = LogType::GetLoggers();
	std::vector<Logger*>::const_iterator x = log_array.begin();
	LogLevel new_log_level = kLevelLowestType;
	for (; x != log_array.end(); ++x) {
		(*x)->SetLevelThreashold(new_log_level);
	}

	str _context;
	bool test_ok = true;

	if (test_ok) {
		_context = "network startup error";
		test_ok = Network::Start();
		deb_assert(test_ok);
	}

	cure::HiscoreAgent hiscore("gamehiscore.pixeldoctrine.com", 80, "UnitTest");

	if (test_ok) {
		_context = "list parse";
		str a = "{\"offset\":2,\"total_count\":5,\"list\":[{\"name\":\"Jonte\",\"score\":4},{\"name\":\"Bea Sune\",\"score\":34567}]}";
		test_ok = hiscore.ParseList(a);
		deb_assert(hiscore.GetDownloadedList().offset_ == 2);
		deb_assert(hiscore.GetDownloadedList().total_count_ == 5);
		deb_assert(hiscore.GetDownloadedList().entry_list_.size() == 2);
		deb_assert(hiscore.GetDownloadedList().entry_list_[0].name_ == "Jonte");
		deb_assert(hiscore.GetDownloadedList().entry_list_[0].score_ == 4);
		deb_assert(hiscore.GetDownloadedList().entry_list_[1].name_ == "Bea Sune");
		deb_assert(hiscore.GetDownloadedList().entry_list_[1].score_ == 34567);
		hiscore.Close();
		if (test_ok) {
			a = "{ \"offset\" : 13 , \"total_count\" : 5 , \"list\" : [ { \"name\" : \"Mea Culpa~*'-.,;:_\\\"\\\\@#%&\" , \"score\" : 42 } , { \"name\" : \"A\" , \"score\" : 4321 } ] }";
			test_ok = hiscore.ParseList(a);
			deb_assert(hiscore.GetDownloadedList().offset_ == 13);
			deb_assert(hiscore.GetDownloadedList().total_count_ == 5);
			deb_assert(hiscore.GetDownloadedList().entry_list_.size() == 2);
			deb_assert(hiscore.GetDownloadedList().entry_list_[0].name_ == "Mea Culpa~*'-.,;:_\"\\@#%&");
			deb_assert(hiscore.GetDownloadedList().entry_list_[0].score_ == 42);
			deb_assert(hiscore.GetDownloadedList().entry_list_[1].name_ == "A");
			deb_assert(hiscore.GetDownloadedList().entry_list_[1].score_ == 4321);
			hiscore.Close();
		}
		if (test_ok) {
			a = "{ \"total_count\": 5 , \"list\": [ { \"name\": \"Mea Culpa~*'-.,;:_\\\"\\\\@#%&\" , \"score\" :42 } , { \"name\" : \"A\" , \"score\":4321 } ], \"offset\" :13}";
			test_ok = hiscore.ParseList(a);
			deb_assert(hiscore.GetDownloadedList().offset_ == 13);
			deb_assert(hiscore.GetDownloadedList().total_count_ == 5);
			deb_assert(hiscore.GetDownloadedList().entry_list_.size() == 2);
			deb_assert(hiscore.GetDownloadedList().entry_list_[0].name_ == "Mea Culpa~*'-.,;:_\"\\@#%&");
			deb_assert(hiscore.GetDownloadedList().entry_list_[0].score_ == 42);
			deb_assert(hiscore.GetDownloadedList().entry_list_[1].name_ == "A");
			deb_assert(hiscore.GetDownloadedList().entry_list_[1].score_ == 4321);
			hiscore.Close();
		}
		if (test_ok) {
			a = "{\"list\":[ { \"score\" :42,\"name\": \"Mea Culpa~*'-.,;:_\\\"\\\\@#%&\" } , { \"score\":4321,\"name\" : \"A\"}],\"total_count\" :5,\"offset\" :13 }";
			test_ok = hiscore.ParseList(a);
			deb_assert(hiscore.GetDownloadedList().offset_ == 13);
			deb_assert(hiscore.GetDownloadedList().total_count_ == 5);
			deb_assert(hiscore.GetDownloadedList().entry_list_.size() == 2);
			deb_assert(hiscore.GetDownloadedList().entry_list_[0].name_ == "Mea Culpa~*'-.,;:_\"\\@#%&");
			deb_assert(hiscore.GetDownloadedList().entry_list_[0].score_ == 42);
			deb_assert(hiscore.GetDownloadedList().entry_list_[1].name_ == "A");
			deb_assert(hiscore.GetDownloadedList().entry_list_[1].score_ == 4321);
			hiscore.Close();
		}
		deb_assert(test_ok);
	}

	if (test_ok) {
		_context = "score parse";
		str a = "{ \"offset\" : 15 }";
		test_ok = hiscore.ParseScore(a);
		deb_assert(hiscore.GetUploadedPlace() == 15);
		hiscore.Close();
		if (test_ok) {
			a = "{\"offset\":3429}";
			test_ok = hiscore.ParseScore(a);
			deb_assert(hiscore.GetUploadedPlace() == 3429);
			hiscore.Close();
		}
		deb_assert(test_ok);
	}

	/*// Upload some scores.
	const tchar* names[] = { "high_festiva!", "Rune", "Ojsan Mobrink", "Gregolf Gugerthsson", "Sveodilf", "Matorgh", "Elow", "Mastiffen", };
	for (int y = 0; y < LEPRA_ARRAY_COUNT(names); ++y) {
		test_ok = hiscore.StartUploadingScore("Computer", "Level", "Avatar", names[y], lepra::Random::GetRandomNumber()%1000 + 1000);
		for (int x = 0; x < 50; ++x) {
			Thread::Sleep(0.1f);
			cure::ResourceLoadState load_state = hiscore.Poll();
			test_ok = (load_state == cure::kResourceLoadComplete);
			if (load_state != cure::kResourceLoadInProgress) {
				break;
			}
		}
		deb_assert(test_ok);
	}*/

	if (test_ok) {
		_context = "init score download";
		test_ok = hiscore.StartDownloadingList("Computer", "Level", "Avatar", 0, 10);
		deb_assert(test_ok);
	}

	if (test_ok) {
		_context = "downloading score";
		for (int y = 0; y < 50; ++y) {
			Thread::Sleep(0.1f);
			cure::ResourceLoadState load_state = hiscore.Poll();
			test_ok = (load_state == cure::kResourceLoadComplete);
			if (load_state != cure::kResourceLoadInProgress) {
				break;
			}
		}
		deb_assert(test_ok);
	}

	if (test_ok) {
		_context = "checking score";
		cure::HiscoreAgent::List hiscore_list = hiscore.GetDownloadedList();
		test_ok = (hiscore_list.offset_ == 0);
		deb_assert(test_ok);
		if (test_ok) {
			test_ok = (hiscore_list.entry_list_.size() == 8);
			deb_assert(test_ok);
		}
		if (test_ok) {
			test_ok = (hiscore_list.entry_list_[0].name_.size() >= 1 && hiscore_list.entry_list_[0].name_.size() <= 13);
			deb_assert(test_ok);
		}
		if (test_ok) {
			test_ok = (hiscore_list.entry_list_[0].score_ >= 1000 && hiscore_list.entry_list_[0].score_ < 2000);
			deb_assert(test_ok);
		}
		if (test_ok) {
			test_ok = (hiscore_list.entry_list_[0].score_ >= hiscore_list.entry_list_[1].score_ &&
				hiscore_list.entry_list_[1].score_ >= hiscore_list.entry_list_[2].score_ &&
				hiscore_list.entry_list_[2].score_ >= hiscore_list.entry_list_[3].score_ &&
				hiscore_list.entry_list_[3].score_ >= hiscore_list.entry_list_[4].score_);
			deb_assert(test_ok);
		}
	}

	if (test_ok) {
		_context = "init score upload";
		test_ok = hiscore.StartUploadingScore("Computer", "Level", "Avatar", "high_festiva!", lepra::Random::GetRandomNumber()%1000 + 1000);
		deb_assert(test_ok);
	}

	if (test_ok) {
		_context = "uploading score";
		for (int y = 0; y < 50; ++y) {
			Thread::Sleep(0.1f);
			cure::ResourceLoadState load_state = hiscore.Poll();
			test_ok = (load_state == cure::kResourceLoadComplete);
			if (load_state != cure::kResourceLoadInProgress) {
				break;
			}
		}
		deb_assert(test_ok);
	}

	if (test_ok) {
		_context = "score place";
		int position = hiscore.GetUploadedPlace();
		test_ok = (position >= 0 && position <= 7);
		deb_assert(test_ok);
	}
	Network::Stop();

	ReportTestResult(log_, "Hiscore", _context, test_ok);
	return (test_ok);
}

loginstance(kTest, HiscoreTest);



bool TestCure() {
	bool test_ok = true;
	/*if (test_ok) {
		test_ok = TestPython(gCLog);
	}*/
	if (test_ok) {
		TestPacker(gCLog);
	}
	if (test_ok) {
		//TerrainTest lTerrainTest;
		//test_ok = lTerrainTest.Test();
	}
	if (test_ok) {
		NetworkClientServerTest client_server_test;
		test_ok = client_server_test.Test();
	}
	if (test_ok) {
		HiscoreTest hiscore_test;
		test_ok = hiscore_test.Test();
	}
	return (test_ok);
}
