
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include <sys/sysctl.h>
#include <stdlib.h>
#include <termios.h>
#include "../../include/posix/maclog.h"
#include "../../include/diskfile.h"
#include "../../include/jsonstring.h"
#include "../../include/lepraos.h"
#include "../../include/log.h"
#include "../../include/path.h"
#include "../../include/string.h"
#include "../../include/systemmanager.h"
#include "../../include/thread.h"



namespace lepra {



static termios g_initial_termios;



static str ExecGetStdout(const char* executable) {
	FILE* file = ::popen(executable, "r");
	if (!file) {
		return str();
	}
	char raw[1024];
	::memset(raw, 0, sizeof(raw));
	::fread(raw, sizeof(raw), 1, file);
#ifdef LEPRA_MAC
	::pclose(file);
#else	// Posix.
	::fclose(file);
#endif	// Mac / Posix.
	str stdout(raw);
	if (stdout.length() > 1) {
		stdout.resize(stdout.length()-1);
	}
	return (stdout);
}



void SystemManager::Init() {
	::tcgetattr(STDIN_FILENO, &g_initial_termios);
	termios __termios = g_initial_termios;
	__termios.c_lflag &= ~(ICANON|ECHO);
	__termios.c_cc[VMIN] = 1;
	__termios.c_cc[VTIME] = 5;
	::tcsetattr(STDIN_FILENO, TCSANOW, &__termios);

	Thread::InitializeMainThread();
}

void SystemManager::Shutdown() {
	ResetTerminal();
}

void SystemManager::ResetTerminal() {
	::tcsetattr(STDIN_FILENO, TCSANOW, &g_initial_termios);
}

str SystemManager::GetRootDirectory() {
	return ("/");
}

str SystemManager::GetCurrentDirectory() {
	char buffer[2048];
	buffer[0] = 0;
	if (::getcwd(buffer, sizeof(buffer)) == NULL) {
		log_.Error("Failed to GetCurrentDirectory()");
	}
	return str(buffer);
}

str SystemManager::GetUserDirectory() {
	return str(::getenv("HOME"));
}

str SystemManager::GetDocumentsDirectory() {
#ifdef LEPRA_IOS
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* path = [paths objectAtIndex:0];
	return MacLog::Decode(path);
#else // Mac
	return Path::JoinPath(GetUserDirectory(), "Documents");
#endif
}

str SystemManager::GetIoDirectory(const str& app_name) {
	const str io_dir = Path::JoinPath(GetUserDirectory(), "."+app_name, "");
	if (!DiskFile::PathExists(io_dir)) {
		DiskFile::CreateDir(io_dir);
	}
	return (io_dir);
}

str SystemManager::GetDataDirectoryFromPath(const str& argv0) {
#if defined(LEPRA_IOS)
	return Path::GetDirectory(argv0);
#elif defined(LEPRA_MAC)
	str data_dir = Path::GetDirectory(argv0);
	data_dir = Path::GetParentDirectory(data_dir);
	return Path::JoinPath(data_dir, "Resources/data/");
#else // Posix
	(void)argv0;
	return "data/";
#endif // iOS/Mac/Posix
}



str SystemManager::GetLoginName() {
	const char* login_name = ::getlogin();
	if (!login_name) {
		login_name = "<Unknown>";
	}
	return (str(login_name));
}

str SystemManager::QueryFullUserName() {
	// TODO: use ::getlogin() to search /usr/passwd or something similarly gory.
	return (GetLoginName());
}

void SystemManager::WebBrowseTo(const str& url) {
	if (::fork() == 0) {
		str _url = url;
		bool found = false;
#ifdef LEPRA_MAC
		found = found || (::system(("open "+_url).c_str()) == 0);
		found = found || (::system(("/Applications/Firefox.app/Contents/MacOS/firefox "+_url).c_str()) == 0);
#else // Other Posix
		found = found || (::system(("sensible-browser "+_url).c_str()) == 0);
		found = found || (::system(("firefox "+_url).c_str()) == 0);
#endif // OS X / Other Posix
		::_exit(0);
		::exit(0);
	}
}

void SystemManager::EmailTo(const str& to, const str& subject, const str& body) {
	if (::fork() == 0) {
		const str url_subject = JsonString::UrlEncode(subject);
		const str url_body = JsonString::UrlEncode(body);
		str w_url = "mailto:" + to + "?subject=" + url_subject + "&body=" + url_body;
		str _url = w_url;
		bool found = false;
#ifdef LEPRA_MAC
		found = found || (::system(("open "+_url).c_str()) == 0);
		found = found || (::system(("/Applications/Mail.app/Contents/MacOS/mail "+_url).c_str()) == 0);
#else // Other Posix
		found = found || (::system(("sensible-browser "+_url).c_str()) == 0);
		found = found || (::system(("firefox "+_url).c_str()) == 0);
#endif // OS X / Other Posix
		::_exit(0);
		::exit(0);
	}
}

str SystemManager::GetHwName() {
#ifdef LEPRA_MAC
	char machine[4096];
	size_t size = sizeof(machine);
	::memset(machine, 0, size);
	::sysctlbyname("hw.machine", machine, &size, 0, 0);
	return str(machine);
#else // Other Posix
	return "PC";
#endif // OS X / Other Posix
}

unsigned SystemManager::GetLogicalCpuCount() {
	return GetPhysicalCpuCount();
}

unsigned SystemManager::GetPhysicalCpuCount() {
	return GetCoreCount();
}

unsigned SystemManager::GetCoreCount() {
#ifdef LEPRA_MAC
	int mib[4];
	mib[0] = CTL_HW;
	mib[1] = HW_NCPU;
	unsigned int ncpu = 1;
	size_t len = sizeof(ncpu);
	::sysctl(mib, 2, &ncpu, &len, NULL, 0);
	if (ncpu < 1) {
		ncpu = 1;
	}
	return ncpu;
#else // Linux
	return ::sysconf(_SC_NPROCESSORS_ONLN);
#endif // Apple / Linux.
}

str SystemManager::GetCpuName() {
#if defined(LEPRA_GCC_X86_32)
	return ("x86");
#elif defined(LEPRA_GCC_X86_64)
	return ("x64");
#elif defined(LEPRA_GCC_POWERPC)
	return ("PowerPC");
#elif defined(LEPRA_GCC_ARM_32)
	return ("ARM");
#elif defined(LEPRA_GCC_ARM_64)
	return ("ARM64");
#else // Unkonwn CPU type.
	return ("Unknown");
#endif // CPU check.
}

str SystemManager::GetOsName() {
	str os_name = ExecGetStdout("uname");
	if (os_name.empty()) {
		return ("Posix");
	}
	return (os_name);
}

uint64 SystemManager::GetAmountRam() {
	// TODO: sigh... popen("free").read().parse()?
	return (500*1024*1024);
}

uint64 SystemManager::GetAvailRam() {
	// TODO: sigh... popen("free").read().parse()?
	return (300*1024*1024);
}

uint64 SystemManager::GetAmountVirtualMemory() {
	// TODO: sigh... popen("free").read().parse()?
	return (700*1024*1024);
}

uint64 SystemManager::GetAvailVirtualMemory() {
	// TODO: sigh... popen("free").read().parse()?
	return (500*1024*1024);
}

void SystemManager::ExitProcess(int exit_code) {
	::exit(exit_code);
}



}
