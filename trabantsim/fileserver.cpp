
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "fileserver.h"
#include "../lepra/include/diskfile.h"
#include "../lepra/include/packer.h"
#include "../lepra/include/path.h"
#include "../lepra/include/socket.h"
#include "../lepra/include/systemmanager.h"



namespace TrabantSim {



FileServer::FileServer(SyncDelegate* sync_delegate):
	accept_thread_(0),
	accept_socket_(0),
	sync_delegate_(sync_delegate) {
}

FileServer::~FileServer() {
	Stop();
}

void FileServer::Start() {
	if (accept_socket_ || accept_thread_) {
		return;
	}
	log_.Info("Starting file server.");
	SocketAddress address;
	address.Resolve("0.0.0.0:2541");
	accept_socket_ = new TcpListenerSocket(address, true);
	accept_thread_ = new MemberThread<FileServer>("FileServerAcceptor");
	accept_thread_->Start(this, &FileServer::AcceptThreadEntry);
}

void FileServer::Stop() {
	if (accept_thread_) {
		log_.Info("Stopping file server.");
		accept_thread_->RequestStop();
	}
	if (accept_socket_) {
		TcpSocket _socket(0);
		SocketAddress address;
		address.Resolve("localhost:2541");
		_socket.Connect(address);
	}
	if (accept_thread_) {
		accept_thread_->GraceJoin(0.3);
		delete accept_thread_;
		accept_thread_ = 0;
	}
	delete accept_socket_;
	accept_socket_ = 0;
}



char FileServer::ReadCommand(TcpSocket* socket, str& data) {
	if (!accept_socket_) {
		return 'q';
	}
	char _command[5];
	if (socket->Receive(&_command, sizeof(_command)) == 5) {
		int32 data_length = 0;
		PackerInt32::Unpack(data_length, (const uint8*)&_command[1], 4);
		data.resize(data_length);
		if (!data_length) {
			return _command[0];
		}
		int read_bytes = 0;
		while (read_bytes < data_length) {
			int bytes = socket->Receive(&data[read_bytes], data_length-read_bytes);
			if (bytes < 0) {
				break;
			}
			read_bytes += bytes;
		}
		if (read_bytes == data_length) {
			return _command[0];
		}
	}
	return 'q';
}

char FileServer::WriteCommand(TcpSocket* socket, char command, const str& data) {
	if (!accept_socket_) {
		return 'q';
	}
	str write_buffer;
	write_buffer.resize(5+data.size());
	write_buffer[0] = command;
	PackerInt32::Pack((uint8*)&write_buffer[1], (int32)data.size());
	if (!data.empty()) {
		::memcpy(&write_buffer[5], &data[0], data.size());
	}
	int written_bytes = 0;
	while (written_bytes < (int)write_buffer.size()) {
		int bytes = socket->Send(&write_buffer[written_bytes], (int)write_buffer.size() - written_bytes);
		if (bytes < 0) {
			break;
		}
		written_bytes += bytes;
	}
	if (written_bytes == (int)write_buffer.size()) {
		return '+';
	}
	return 'q';
}

void FileServer::ClientCommandEntry(TcpSocket* socket) {
	char _command = ' ';
	str argument;
	const str doc_dir = SystemManager::GetDocumentsDirectory();
	str file_data;
	while (_command != 'q' && accept_socket_) {
		_command = ReadCommand(socket, argument);
		if (_command == 'p') {
			str wild_card = argument;
			wild_card = Path::JoinPath(doc_dir, wild_card);
			strutil::strvec files;
			DiskFile::FindData info;
			for (bool ok = DiskFile::FindFirst(wild_card, info); ok; ok = DiskFile::FindNext(info)) {
				if (info.IsSubDir()) {
					continue;
				}
				files.push_back(Path::GetCompositeFilename(info.GetName()));
			}
			str filenames = strutil::Join(files, "\n");
			WriteCommand(socket, 'l', filenames);
			for (strutil::strvec::iterator f = files.begin(); f != files.end(); ++f) {
				file_data.clear();
				char* _data = 0;
				int64 data_size = 0;
				const str filename = Path::JoinPath(doc_dir, *f);
				if (DiskFile::Load(filename, (void**)&_data, data_size) == kIoOk) {
					file_data.append(_data, (size_t)data_size);
				}
				delete _data;
				_command = WriteCommand(socket, 'r', file_data);
			}
		} else if (_command == 'w') {
			str filename = argument;
			str contents;
			_command = ReadCommand(socket, contents);
			bool ok = false;
			if (_command == 'b') {
				const str file_path = Path::JoinPath(doc_dir, filename);
				if (contents.empty()) {
					ok = DiskFile::Delete(file_path);
				} else {
					DiskFile write_file;
					if (write_file.Open(file_path, DiskFile::kModeWrite)) {
						ok = (write_file.WriteData(&contents[0], contents.size()) == kIoOk);
					}
				}
			}
			_command = WriteCommand(socket, 'w', str(ok? "ok":"error"));
		}
	}
	if (accept_socket_ && sync_delegate_) {
		sync_delegate_->DidSync();
	}
}

void FileServer::AcceptThreadEntry() {
	while (accept_socket_ && !accept_thread_->GetStopRequest()) {
		TcpSocket* _socket = accept_socket_->Accept();
		if (accept_thread_->GetStopRequest()) {
			delete _socket;
			break;
		}
		str hostname;
		if (!_socket->GetTargetAddress().ResolveIpToHostname(hostname)) {
			hostname = _socket->GetTargetAddress().GetIP().GetAsString();
		}
		if (!sync_delegate_->WillSync(hostname)) {
			delete _socket;
			continue;
		}
		if (_socket) {
			MemberThread<FileServer,TcpSocket*>* server = new MemberThread<FileServer,TcpSocket*>("FileServer");
			server->RequestSelfDestruct();
			server->Start(this, &FileServer::ClientCommandEntry, _socket);
		}
	}
	delete accept_socket_;
	accept_socket_ = 0;
}



loginstance(kGame, FileServer);



}
