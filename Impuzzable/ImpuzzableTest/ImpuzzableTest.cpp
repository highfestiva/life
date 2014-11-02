
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Cure/Include/Cure.h"
#include "../../Cure/Include/ResourceManager.h"
#include "../../Lepra/Include/Application.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/LepraOS.h"
#include "../../Lepra/Include/LogListener.h"
#include "../AsciiLoader.h"

using namespace Cure;
using namespace Lepra;



namespace Impuzzable
{



void Test2dCube()
{
	assert(false);
}

void Test3dCuboid()
{
	assert(false);
}

void TestRotated2dCube()
{
	assert(false);
}

void TestRotated3dCuboid()
{
	assert(false);
}

void TestSmall2dTriangle()
{
	assert(false);
}

void TestBig3dTriangle()
{
	assert(false);
}

void TestSimpleShape()
{
	assert(false);
}

void TestComplexShape()
{
	assert(false);
}

void TestFileLoad()
{
	ResourceManager lResourceLoader(0);
	AsciiLoader lLoader(&lResourceLoader);
	wstr lAsciiParts = lLoader.Load(_T("parts3.txt"));
	assert(!lAsciiParts.empty());
	std::vector<PieceInfo> lPiecesInfo;
	assert(lLoader.ConvertPieces(lAsciiParts, lPiecesInfo));
	assert(lPiecesInfo.size() == 6);
}



class ImpuzzableTest: public Application
{
	typedef Application Parent;
public:
	ImpuzzableTest(const strutil::strvec& pArgumentList);
	virtual ~ImpuzzableTest();
	virtual void Init();
	virtual int Run();

	/*str GetTypeName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;*/

	//static ImpuzzableTest* mApp;

	logclass();
};



}



LEPRA_RUN_APPLICATION(Impuzzable::ImpuzzableTest, Lepra::Main);



namespace Impuzzable
{



ImpuzzableTest::ImpuzzableTest(const strutil::strvec& pArgumentList):
	Parent(pArgumentList)
{
}

ImpuzzableTest::~ImpuzzableTest()
{
	Cure::Shutdown();
	Tbc::Shutdown();
	Lepra::Shutdown();
}

void ImpuzzableTest::Init()
{
	Lepra::Init();
	Tbc::Init();
	Cure::Init();
	LogType::GetLogger(LogType::SUB_ROOT)->SetupBasicListeners(new StdioConsoleLogListener, new DebuggerLogListener, 0, 0, 0);
}

int ImpuzzableTest::Run()
{
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



loginstance(GAME, ImpuzzableTest);



}
