
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "FileServer.h"
#include "../Lepra/Include/DiskFile.h"
#include "../Lepra/Include/Packer.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/Socket.h"
#include "../Lepra/Include/SystemManager.h"



namespace TrabantSim
{



FileServer::FileServer(SyncDelegate* pSyncDelegate):
	mAcceptThread(0),
	mAcceptSocket(0),
	mSyncDelegate(pSyncDelegate)
{
}

FileServer::~FileServer()
{
	Stop();
}

void FileServer::Start()
{
	if (mAcceptSocket || mAcceptThread)
	{
		return;
	}
	mLog.Info(_T("Starting file server."));
	SocketAddress lAddress;
	lAddress.Resolve(_T("0.0.0.0:2541"));
	mAcceptSocket = new TcpListenerSocket(lAddress, true);
	mAcceptThread = new MemberThread<FileServer>("FileServerAcceptor");
	mAcceptThread->Start(this, &FileServer::AcceptThreadEntry);
}

void FileServer::Stop()
{
	if (mAcceptThread)
	{
		mLog.Info(_T("Stopping file server."));
		mAcceptThread->RequestStop();
	}
	if (mAcceptSocket)
	{
		TcpSocket lSocket(0);
		SocketAddress lAddress;
		lAddress.Resolve(_T("localhost:2541"));
		lSocket.Connect(lAddress);
	}
	if (mAcceptThread)
	{
		mAcceptThread->GraceJoin(0.3);
		delete mAcceptThread;
		mAcceptThread = 0;
	}
	delete mAcceptSocket;
	mAcceptSocket = 0;
}



char FileServer::ReadCommand(TcpSocket* pSocket, astr& pData)
{
	if (!mAcceptSocket)
	{
		return 'q';
	}
	char lCommand[5];
	if (pSocket->Receive(&lCommand, sizeof(lCommand)) == 5)
	{
		int32 lDataLength = 0;
		PackerInt32::Unpack(lDataLength, (const uint8*)&lCommand[1], 4);
		pData.resize(lDataLength);
		if (!lDataLength)
		{
			return lCommand[0];
		}
		int lReadBytes = 0;
		while (lReadBytes < lDataLength)
		{
			int lBytes = pSocket->Receive(&pData[lReadBytes], lDataLength-lReadBytes);
			if (lBytes < 0)
			{
				break;
			}
			lReadBytes += lBytes;
		}
		if (lReadBytes == lDataLength)
		{
			return lCommand[0];
		}
	}
	return 'q';
}

char FileServer::WriteCommand(TcpSocket* pSocket, char pCommand, const astr& pData)
{
	if (!mAcceptSocket)
	{
		return 'q';
	}
	astr lWriteBuffer;
	lWriteBuffer.resize(5+pData.size());
	lWriteBuffer[0] = pCommand;
	PackerInt32::Pack((uint8*)&lWriteBuffer[1], (int32)pData.size());
	if (!pData.empty())
	{
		::memcpy(&lWriteBuffer[5], &pData[0], pData.size());
	}
	int lWrittenBytes = 0;
	while (lWrittenBytes < (int)lWriteBuffer.size())
	{
		int lBytes = pSocket->Send(&lWriteBuffer[lWrittenBytes], (int)lWriteBuffer.size() - lWrittenBytes);
		if (lBytes < 0)
		{
			break;
		}
		lWrittenBytes += lBytes;
	}
	if (lWrittenBytes == (int)lWriteBuffer.size())
	{
		return '+';
	}
	return 'q';
}

void FileServer::ClientCommandEntry(TcpSocket* pSocket)
{
	char lCommand = ' ';
	astr lArgument;
	const str lDocDir = SystemManager::GetDocumentsDirectory();
	astr lFileData;
	while (lCommand != 'q' && mAcceptSocket)
	{
		lCommand = ReadCommand(pSocket, lArgument);
		if (lCommand == 'p')
		{
			str lWildCard = strutil::Encode(lArgument);
			lWildCard = Path::JoinPath(lDocDir, lWildCard);
			strutil::strvec lFiles;
			DiskFile::FindData lInfo;
			for (bool lOk = DiskFile::FindFirst(lWildCard, lInfo); lOk; lOk = DiskFile::FindNext(lInfo))
			{
				if (lInfo.IsSubDir())
				{
					continue;
				}
				lFiles.push_back(Path::GetCompositeFilename(lInfo.GetName()));
			}
			astr lFilenames = astrutil::Encode(strutil::Join(lFiles, _T("\n")));
			WriteCommand(pSocket, 'l', lFilenames);
			for (strutil::strvec::iterator f = lFiles.begin(); f != lFiles.end(); ++f)
			{
				lFileData.clear();
				char* lData = 0;
				int64 lDataSize = 0;
				const str lFilename = Path::JoinPath(lDocDir, *f);
				if (DiskFile::Load(lFilename, (void**)&lData, lDataSize) == IO_OK)
				{
					lFileData.append(lData, (size_t)lDataSize);
				}
				delete lData;
				lCommand = WriteCommand(pSocket, 'r', lFileData);
			}
		}
		else if (lCommand == 'w')
		{
			str lFilename = strutil::Encode(lArgument);
			astr lContents;
			lCommand = ReadCommand(pSocket, lContents);
			bool lOk = false;
			if (lCommand == 'b')
			{
				const str lFilePath = Path::JoinPath(lDocDir, lFilename);
				if (lContents.empty())
				{
					lOk = DiskFile::Delete(lFilePath);
				}
				else
				{
					DiskFile lWriteFile;
					if (lWriteFile.Open(lFilePath, DiskFile::MODE_WRITE))
					{
						lOk = (lWriteFile.WriteData(&lContents[0], lContents.size()) == IO_OK);
					}
				}
			}
			lCommand = WriteCommand(pSocket, 'w', astr(lOk? "ok":"error"));
		}
	}
	if (mAcceptSocket && mSyncDelegate)
	{
		mSyncDelegate->DidSync();
	}
}

void FileServer::AcceptThreadEntry()
{
	while (mAcceptSocket && !mAcceptThread->GetStopRequest())
	{
		TcpSocket* lSocket = mAcceptSocket->Accept();
		if (mAcceptThread->GetStopRequest())
		{
			delete lSocket;
			break;
		}
		str lHostname;
		if (!lSocket->GetTargetAddress().ResolveIpToHostname(lHostname))
		{
			lHostname = lSocket->GetTargetAddress().GetIP().GetAsString();
		}
		if (!mSyncDelegate->WillSync(lHostname))
		{
			delete lSocket;
			continue;
		}
		if (lSocket)
		{
			MemberThread<FileServer,TcpSocket*>* lServer = new MemberThread<FileServer,TcpSocket*>("FileServer");
			lServer->RequestSelfDestruct();
			lServer->Start(this, &FileServer::ClientCommandEntry, lSocket);
		}
	}
	delete mAcceptSocket;
	mAcceptSocket = 0;
}



loginstance(GAME, FileServer);



}
