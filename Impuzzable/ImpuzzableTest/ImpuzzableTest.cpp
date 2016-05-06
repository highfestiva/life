
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../cure/include/cure.h"
#include "../../cure/include/resourcemanager.h"
#include "../../lepra/include/application.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/lepraos.h"
#include "../../lepra/include/loglistener.h"
#include "../asciiloader.h"

using namespace cure;
using namespace lepra;



namespace Impuzzable {



void Test2dCube() {
	assert(false);
}

void Test3dCuboid() {
	assert(false);
}

void TestRotated2dCube() {
	assert(false);
}

void TestRotated3dCuboid() {
	assert(false);
}

void TestSmall2dTriangle() {
	assert(false);
}

void TestBig3dTriangle() {
	assert(false);
}

void TestSimpleShape() {
	assert(false);
}

void TestComplexShape() {
	assert(false);
}

void TestFileLoad() {
	ResourceManager resource_loader(0);
	AsciiLoader loader(&resource_loader);
	wstr ascii_parts = loader.Load("parts3.txt");
	assert(!ascii_parts.empty());
	std::vector<PieceInfo> pieces_info;
	assert(loader.ConvertPieces(ascii_parts, pieces_info));
	assert(pieces_info.size() == 6);
}



class ImpuzzableTest: public Application {
	typedef Application Parent;
public:
	ImpuzzableTest(const strutil::strvec& argument_list);
	virtual ~ImpuzzableTest();
	virtual void Init();
	virtual int Run();

	/*str GetTypeName() const;
	str GetVersion() const;
	cure::ApplicationTicker* CreateTicker() const;*/

	//static ImpuzzableTest* app_;

	logclass();
};



}



LEPRA_RUN_APPLICATION(Impuzzable::ImpuzzableTest, lepra::Main);



namespace Impuzzable {



ImpuzzableTest::ImpuzzableTest(const strutil::strvec& argument_list):
	Parent(argument_list) {
}

ImpuzzableTest::~ImpuzzableTest() {
	cure::Shutdown();
	tbc::Shutdown();
	lepra::Shutdown();
}

void ImpuzzableTest::Init() {
	lepra::Init();
	tbc::Init();
	cure::Init();
	LogType::GetLogger(LogType::SUB_ROOT)->SetupBasicListeners(new StdioConsoleLogListener, new DebuggerLogListener, 0, 0, 0);
}

int ImpuzzableTest::Run() {
	Test2dCube();
	Test3dCuboid();
	TestRotated2dCube();
	TestRotated3dCuboid();
	TestSmall2dTriangle();
	TestBig3dTriangle();
	TestSimpleShape();
	TestComplexShape();
	TestFileLoad();
	return 0;
}



loginstance(kGame, ImpuzzableTest);



}
