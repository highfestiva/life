# 'Twas generated from ../../Lepra/Lepra.vcproj, 2009-05-25, type='lib'.
# Don't edit manually. See 'generate_makefile.py' for info.

CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS -I../ThirdParty/stlport/stlport -I../ThirdParty/ode-060223/include -DPOSIX -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdCYLINDER_ENABLED -Wall -Wno-unknown-pragmas

SRCS=	\
Source/Application.cpp	\
Source/ArchiveFile.cpp	\
Source/ArgumentList.cpp	\
Source/BmpLoader.cpp	\
Source/BufferStream.cpp	\
Source/Canvas.cpp	\
Source/Checksum.cpp	\
Source/ConsoleCommandManager.cpp	\
Source/CubicSpline.cpp	\
Source/DES.cpp	\
Source/DiskFile.cpp	\
Source/DllLoader.cpp	\
Source/Endian.cpp	\
Source/FdSet.cpp	\
Source/File.cpp	\
Source/FileArchive.cpp	\
Source/GammaLookup.cpp	\
Source/ImageLoader.cpp	\
Source/IOBuffer.cpp	\
Source/IOError.cpp	\
Source/IPAddress.cpp	\
Source/JpegLoader.cpp	\
Source/Lepra.cpp	\
Source/Log.cpp	\
Source/LogListener.cpp	\
Source/LogType.cpp	\
Source/Math.cpp	\
Source/MemFile.cpp	\
Source/MemoryLeakTracker.cpp	\
Source/MetaFile.cpp	\
Source/Network.cpp	\
Source/Number.cpp	\
Source/ParamLoader.cpp	\
Source/Params.cpp	\
Source/Path.cpp	\
Source/Performance.cpp	\
Source/PerformanceScope.cpp	\
Source/PngLoader.cpp	\
Source/ProgressCallback.cpp	\
Source/Random.cpp	\
Source/Reader.cpp	\
Source/SHA1.cpp	\
Source/Socket.cpp	\
Source/SocketAddress.cpp	\
Source/SocketStream.cpp	\
Source/StringUtility.cpp	\
Source/SubFile.cpp	\
Source/SystemManager.cpp	\
Source/TgaLoader.cpp	\
Source/Thread.cpp	\
Source/TiffLoader.cpp	\
Source/Time.cpp	\
Source/Writer.cpp	\
Source/ZipArchive.cpp	\
Source/Posix/PosixAntiCrack.cpp	\
Source/Posix/PosixBusLock.cpp	\
Source/Posix/PosixSystemManager.cpp	\
Source/Posix/PosixThread.cpp

OBJS=	\
Source/Application.o	\
Source/ArchiveFile.o	\
Source/ArgumentList.o	\
Source/BmpLoader.o	\
Source/BufferStream.o	\
Source/Canvas.o	\
Source/Checksum.o	\
Source/ConsoleCommandManager.o	\
Source/CubicSpline.o	\
Source/DES.o	\
Source/DiskFile.o	\
Source/DllLoader.o	\
Source/Endian.o	\
Source/FdSet.o	\
Source/File.o	\
Source/FileArchive.o	\
Source/GammaLookup.o	\
Source/ImageLoader.o	\
Source/IOBuffer.o	\
Source/IOError.o	\
Source/IPAddress.o	\
Source/JpegLoader.o	\
Source/Lepra.o	\
Source/Log.o	\
Source/LogListener.o	\
Source/LogType.o	\
Source/Math.o	\
Source/MemFile.o	\
Source/MemoryLeakTracker.o	\
Source/MetaFile.o	\
Source/Network.o	\
Source/Number.o	\
Source/ParamLoader.o	\
Source/Params.o	\
Source/Path.o	\
Source/Performance.o	\
Source/PerformanceScope.o	\
Source/PngLoader.o	\
Source/ProgressCallback.o	\
Source/Random.o	\
Source/Reader.o	\
Source/SHA1.o	\
Source/Socket.o	\
Source/SocketAddress.o	\
Source/SocketStream.o	\
Source/StringUtility.o	\
Source/SubFile.o	\
Source/SystemManager.o	\
Source/TgaLoader.o	\
Source/Thread.o	\
Source/TiffLoader.o	\
Source/Time.o	\
Source/Writer.o	\
Source/ZipArchive.o	\
Source/Posix/PosixAntiCrack.o	\
Source/Posix/PosixBusLock.o	\
Source/Posix/PosixSystemManager.o	\
Source/Posix/PosixThread.o

all:	libLepra.so $(OBJS)
clean:
	@rm -f libLepra.so $(OBJS)
libLepra.so:	$(OBJS)
	g++ -shared -o $@ $(OBJS)

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)
.c.o:
	g++ $(CFLAGS) -o $@ -c $<
.cpp.o:
	g++ $(CFLAGS) -o $@ -c $<
