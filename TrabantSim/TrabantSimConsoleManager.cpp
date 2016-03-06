
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "TrabantSimConsoleManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "TrabantSimManager.h"
#include "RtVar.h"



namespace TrabantSim
{



// Must lie before TrabantSimConsoleManager to compile.
const TrabantSimConsoleManager::CommandPair TrabantSimConsoleManager::mCommandIdList[] =
{
	{_T("reset"), COMMAND_RESET},
	{_T("get-platform-name"), COMMAND_GET_PLATFORM_NAME},
	{_T("create-object"), COMMAND_CREATE_OBJECT},
	{_T("create-clones"), COMMAND_CREATE_CLONES},
	{_T("delete-object"), COMMAND_DELETE_OBJECT},
	{_T("delete-all-objects"), COMMAND_DELETE_ALL_OBJECTS},
	{_T("pick-objects"), COMMAND_PICK_OBJECTS},
	{_T("clear-phys"), COMMAND_CLEAR_PHYS},
	{_T("prep-phys-box"), COMMAND_PREP_PHYS_BOX},
	{_T("prep-phys-sphere"), COMMAND_PREP_PHYS_SPHERE},
	{_T("prep-phys-capsule"), COMMAND_PREP_PHYS_CAPSULE},
	{_T("prep-phys-mesh"), COMMAND_PREP_PHYS_MESH},
	{_T("prep-gfx-mesh"), COMMAND_PREP_GFX_MESH},
	{_T("set-vertices"), COMMAND_SET_VERTICES},
	{_T("add-vertices"), COMMAND_ADD_VERTICES},
	{_T("set-indices"), COMMAND_SET_INDICES},
	{_T("add-indices"), COMMAND_ADD_INDICES},
	{_T("are-loaded"), COMMAND_ARE_LOADED},
	{_T("wait-until-loaded"), COMMAND_WAIT_UNTIL_LOADED},
	{_T("explode"), COMMAND_EXPLODE},
	{_T("play-sound"), COMMAND_PLAY_SOUND},
	{_T("pop-collisions"), COMMAND_POP_COLLISIONS},
	{_T("get-keys"), COMMAND_GET_KEYS},
	{_T("get-touch-drags"), COMMAND_GET_TOUCH_DRAGS},
	{_T("get-accelerometer"), COMMAND_GET_ACCELEROMETER},
	{_T("get-mousemove"), COMMAND_GET_MOUSEMOVE},
	{_T("create-joystick"), COMMAND_CREATE_JOYSTICK},
	{_T("get-joystick-data"), COMMAND_GET_JOYSTICK_DATA},
	{_T("get-aspect-ratio"), COMMAND_GET_ASPECT_RATIO},
	{_T("create-engine"), COMMAND_CREATE_ENGINE},
	{_T("create-joint"), COMMAND_CREATE_JOINT},
	{_T("position"), COMMAND_POSITION},
	{_T("orientation"), COMMAND_ORIENTATION},
	{_T("velocity"), COMMAND_VELOCITY},
	{_T("angular-velocity"), COMMAND_ANGULAR_VELOCITY},
	{_T("mass"), COMMAND_MASS},
	{_T("color"), COMMAND_COLOR},
	{_T("engine-force"), COMMAND_ENGINE_FORCE},
	{_T("set-tag-floats"), COMMAND_SET_TAG_FLOATS},
	{_T("set-tag-strings"), COMMAND_SET_TAG_STRINGS},
	{_T("set-tag-phys"), COMMAND_SET_TAG_PHYS},
	{_T("set-tag-engine"), COMMAND_SET_TAG_ENGINE},
	{_T("set-tag-mesh"), COMMAND_SET_TAG_MESH},
	{_T("add-tag"), COMMAND_ADD_TAG},
};



struct ParameterException
{
};

std::vector<float> Strs2Flts(const strutil::strvec& pStrs, size_t pIndex=0)
{
	std::vector<float> lFlts;
	strutil::strvec::const_iterator x = pStrs.begin() + pIndex;
	for(; x != pStrs.end(); ++x)
	{
		double d = 0;
		strutil::StringToDouble(*x, d);
		lFlts.push_back((float)d);
	}
	return lFlts;
}

int StrToUInt(const tchar* s, const tchar** pEnd)
{
	int i = 0;
	for(; *s >= '0' && *s <= '9'; ++s)
	{
		i = i*10 + (*s-'0');
	}
	*pEnd = s;
	return i;
}

float StrToFloat(const tchar* s, const tchar** pEnd)
{
	float f = 0;
	float ff = 0.1f;
	float e = 0;
	bool lBeforeDot = true;
	bool lNegative = false;
	bool lExponent = false;
	bool lNegativeExponent = false;
	for (;; ++s)
	{
		if (*s >= '0' && *s <= '9')
		{
			if (lExponent)
			{
				e = e*10 + (*s-'0');
			}
			else if (lBeforeDot)
			{
				f = f*10 + (*s-'0');
			}
			else
			{
				f += (*s-'0')*ff;
				ff *= 0.1f;
			}
		}
		else if (*s == '.')
		{
			lBeforeDot = false;
		}
		else if (*s == '-')
		{
			if (lExponent)
			{
				lNegativeExponent = true;
			}
			else
			{
				lNegative = true;
			}
		}
		else if (*s == 'e')
		{
			lExponent = true;
		}
		else
		{
			break;
		}
	}
	*pEnd = s;
	f = lNegative? -f : f;
	if (!lExponent)
	{
		return f;
	}
	e = lNegativeExponent? -e : e;
	return f*pow(10,e);
}

str ToStr(int i)
{
	return strutil::IntToString(i, 10);
}

str ToStr(float f)
{
	return strutil::DoubleToString(f, 10);
}

str ParamToStr(const strutil::strvec& pParam, size_t pIndex)
{
	if (pIndex >= pParam.size())
	{
		throw ParameterException();
	}
	return pParam[pIndex];
}

bool ParamToBool(const strutil::strvec& pParam, size_t pIndex)
{
	if (pIndex >= pParam.size())
	{
		throw ParameterException();
	}
	return pParam[pIndex] == _T("true");
}

int ParamToInt(const strutil::strvec& pParam, size_t pIndex, bool* pIsSet = 0)
{
	int lValue = 0;
	if (pIndex >= pParam.size() || !strutil::StringToInt(pParam[pIndex], lValue))
	{
		if (!pIsSet)
		{
			throw ParameterException();
		}
		*pIsSet = false;
	}
	else if (pIsSet)
	{
		*pIsSet = true;
	}
	return lValue;
}

float ParamToFloat(const strutil::strvec& pParam, size_t pIndex, bool* pIsSet = 0)
{
	double lValue = 0;
	if (pIndex >= pParam.size() || !strutil::StringToDouble(pParam[pIndex], lValue))
	{
		if (!pIsSet)
		{
			throw ParameterException();
		}
		*pIsSet = false;
	}
	else if (pIsSet)
	{
		*pIsSet = true;
	}
	return (float)lValue;
}

vec2 ParamToVec2(const strutil::strvec& pParam, size_t pIndex)
{
	return vec2(ParamToFloat(pParam,pIndex,0), ParamToFloat(pParam,pIndex+1,0));
}

vec3 ParamToVec3(const strutil::strvec& pParam, size_t pIndex, bool* pIsSet = 0)
{
	return vec3(ParamToFloat(pParam,pIndex,pIsSet), ParamToFloat(pParam,pIndex+1,pIsSet), ParamToFloat(pParam,pIndex+2,pIsSet));
}

quat ParamToQuat(const strutil::strvec& pParam, size_t pIndex, bool* pIsSet = 0)
{
	return quat(ParamToFloat(pParam,pIndex,pIsSet), ParamToFloat(pParam,pIndex+1,pIsSet), ParamToFloat(pParam,pIndex+2,pIsSet), ParamToFloat(pParam,pIndex+3,pIsSet));
}

void Params2Ints(const strutil::strvec& pParam, std::vector<int>& pInts)
{
	strutil::strvec::const_iterator p;
	for (p = pParam.begin(); p != pParam.end(); ++p)
	{
		const tchar* s = p->c_str();
		const tchar* lEnd;
		do
		{
			int i = StrToUInt(s, &lEnd);
			pInts.push_back(i);
			s = lEnd+1;
		}
		while (*lEnd == ',');
	}
}

void Params2Floats(const strutil::strvec& pParam, std::vector<float>& pFloats, size_t pParamIndex=0)
{
	strutil::strvec::const_iterator p;
	for (p = pParam.begin()+pParamIndex; p != pParam.end(); ++p)
	{
		const tchar* s = p->c_str();
		const tchar* lEnd;
		do
		{
			float f = StrToFloat(s, &lEnd);
			pFloats.push_back(f);
			s = lEnd+1;
		}
		while (*lEnd == ',');
	}
}



TrabantSimConsoleManager::TrabantSimConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea):
	Parent(pResourceManager, pGameManager, pUiManager, pVariableScope, pArea)
{
	InitCommands();
	SetSecurityLevel(1);
}

TrabantSimConsoleManager::~TrabantSimConsoleManager()
{
}

bool TrabantSimConsoleManager::Start()
{
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // Touch
	return true;	// Touch device don't need an interactive console.
#endif // Computer / touch
}

const str& TrabantSimConsoleManager::GetActiveResponse() const
{
	return mActiveResponse;
}



unsigned TrabantSimConsoleManager::GetCommandCount() const
{
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(mCommandIdList);
}

const TrabantSimConsoleManager::CommandPair& TrabantSimConsoleManager::GetCommand(unsigned pIndex) const
{
	if (pIndex < Parent::GetCommandCount())
	{
		return (Parent::GetCommand(pIndex));
	}
	return (mCommandIdList[pIndex-Parent::GetCommandCount()]);
}

int TrabantSimConsoleManager::OnCommand(const str& pCommand, const strutil::strvec& pParameterVector)
{
	mActiveResponse = str(_T("ok\n"));
	int lResult = Parent::OnCommand(pCommand, pParameterVector);
	if (lResult < 0)
	{
		lResult = 0;
		TrabantSimManager* lManager = ((TrabantSimManager*)GetGameManager());

		CommandClient lCommand = (CommandClient)TranslateCommand(pCommand);
		try
		{
			switch ((int)lCommand)
			{
				case COMMAND_RESET:
				{
					lManager->UserReset();
				}
				break;
				case COMMAND_GET_PLATFORM_NAME:
				{
#if defined(LEPRA_IOS)
					mActiveResponse += _T("iOS");
#elif defined(LEPRA_MAC)
					mActiveResponse += _T("Mac");
#elif defined(LEPRA_WINDOWS)
					mActiveResponse += _T("Win");
#else
					mActiveResponse += _T("Unknown");
#endif // Platform

				}
				break;
				case COMMAND_CREATE_OBJECT:
				{
					const str lType = ParamToStr(pParameterVector, 0);
					const str lMaterialInfo = ParamToStr(pParameterVector, 1);
					const vec3 lPosition = ParamToVec3(pParameterVector, 2);
					const quat lOrientation = ParamToQuat(pParameterVector, 5);
					// Work out material.
					const bool lIsStatic = strutil::StartsWith(lType, _T("static"));
					const bool lIsTrigger = (lType.find(_T("trigger")) != str::npos);
					ObjectMaterial lMaterial = MaterialSmooth;
					if (lMaterialInfo == _T("flat")) lMaterial = MaterialFlat;
					else if (lMaterialInfo == _T("checker")) lMaterial = MaterialChecker;
					else if (lMaterialInfo == _T("noise")) lMaterial = MaterialNoise;

					const int lObjectId = lManager->CreateObject(lOrientation, lPosition, mGfxMesh, mPhysObjects, lMaterial, lIsStatic, lIsTrigger);
					if (lObjectId == -1)
					{
						throw ParameterException();
					}
					mActiveResponse += ToStr(lObjectId);
				}
				break;
				case COMMAND_CREATE_CLONES:
				{
					const int lOriginalId = ParamToInt(pParameterVector, 0);
					const str lType = ParamToStr(pParameterVector, 1);
					const str lMaterialInfo = ParamToStr(pParameterVector, 2);
					std::vector<float> lPlacements;
					Params2Floats(pParameterVector, lPlacements, 3);
					// Work out material.
					const bool lIsStatic = (lType == _T("static"));
					ObjectMaterial lMaterial = MaterialSmooth;
					if (lMaterialInfo == _T("flat")) lMaterial = MaterialFlat;
					else if (lMaterialInfo == _T("checker")) lMaterial = MaterialChecker;
					else if (lMaterialInfo == _T("noise")) lMaterial = MaterialNoise;

					std::vector<xform> lTransforms;
					int x = 0, cnt = (int)lPlacements.size();
					for (; x <= cnt-7; x += 7)
					{
						lTransforms.push_back(xform(quat(&lPlacements[x+3]), vec3(&lPlacements[x])));
					}
					std::vector<int> lObjectIds;
					lManager->CreateClones(lObjectIds, lOriginalId, lTransforms, lMaterial, lIsStatic);
					if (lObjectIds.empty())
					{
						throw ParameterException();
					}
					bool lFirst = true;
					for (std::vector<int>::iterator y = lObjectIds.begin(); y != lObjectIds.end(); ++y)
					{
						if (!lFirst)
						{
							mActiveResponse += _T(',');
						}
						lFirst = false;
						mActiveResponse += ToStr(*y);
					}
				}
				break;
				case COMMAND_DELETE_OBJECT:
				{
					lManager->DeleteObject(ParamToInt(pParameterVector, 0));
				}
				break;
				case COMMAND_DELETE_ALL_OBJECTS:
				{
					lManager->DeleteAllObjects();
				}
				break;
				case COMMAND_PICK_OBJECTS:
				{
					const vec3 lPosition = ParamToVec3(pParameterVector, 0);
					const vec3 lDirection = ParamToVec3(pParameterVector, 3);
					const vec2 lRange = ParamToVec2(pParameterVector, 6);
					std::vector<int> lObjectIds;
					std::vector<vec3> lPositions;
					lManager->PickObjects(lPosition, lDirection, lRange, lObjectIds, lPositions);
					size_t c = lObjectIds.size();
					for (size_t x = 0; x < c; ++x)
					{
						if (x)
						{
							mActiveResponse += _T(',');
						}
						const vec3& v = lPositions[x];
						mActiveResponse += strutil::Format(_T("%i,%f,%f,%f"), lObjectIds[x], v.x, v.y, v.z);
					}
				}
				break;
				case COMMAND_CLEAR_PHYS:
				{
					PhysObjectArray::iterator x;
					for (x = mPhysObjects.begin(); x != mPhysObjects.end(); ++x)
					{
						delete *x;
					}
					mPhysObjects.clear();
				}
				break;
				case COMMAND_PREP_PHYS_BOX:
				{
					if (pParameterVector.size() != 10)
					{
						mLog.Warningf(_T("usage: %s followed by ten float arguments (quaternion, position, size)"), pCommand.c_str());
						return 1;
					}
					std::vector<float> lFloats = Strs2Flts(pParameterVector);
					PlacedObject* lBox = new BoxObject(quat(&lFloats[0]), vec3(&lFloats[4]), vec3(&lFloats[7]));
					mPhysObjects.push_back(lBox);
				}
				break;
				case COMMAND_PREP_PHYS_SPHERE:
				{
					if (pParameterVector.size() != 8)
					{
						mLog.Warningf(_T("usage: %s followed by eight float arguments (quaternion, position, radius)"), pCommand.c_str());
						return 1;
					}
					std::vector<float> lFloats = Strs2Flts(pParameterVector);
					PlacedObject* lSphere = new SphereObject(quat(&lFloats[0]), vec3(&lFloats[4]), lFloats[7]);
					mPhysObjects.push_back(lSphere);
				}
				break;
				case COMMAND_PREP_PHYS_CAPSULE:
				{
					if (pParameterVector.size() != 9)
					{
						mLog.Warningf(_T("usage: %s followed by eight float arguments (quaternion, position, radius, length)"), pCommand.c_str());
						return 1;
					}
					std::vector<float> lFloats = Strs2Flts(pParameterVector);
					PlacedObject* lCapsule = new CapsuleObject(quat(&lFloats[0]), vec3(&lFloats[4]), lFloats[7], lFloats[8]);
					mPhysObjects.push_back(lCapsule);
				}
				break;
				case COMMAND_PREP_PHYS_MESH:
				{
					if (pParameterVector.size() != 7)
					{
						mLog.Warningf(_T("usage: %s followed by seven float arguments (quaternion and position)"), pCommand.c_str());
						return 1;
					}
					std::vector<float> lFloats = Strs2Flts(pParameterVector);
					MeshObject* lMesh = new MeshObject(quat(&lFloats[0]), vec3(&lFloats[4]));
					lMesh->mVertices.insert(lMesh->mVertices.end(), mVertices.begin(), mVertices.end());
					lMesh->mIndices.insert(lMesh->mIndices.end(), mIndices.begin(), mIndices.end());
					mPhysObjects.push_back(lMesh);
				}
				break;
				case COMMAND_PREP_GFX_MESH:
				{
					if (pParameterVector.size() != 7)
					{
						mLog.Warningf(_T("usage: %s followed by seven float arguments (quaternion and position)"), pCommand.c_str());
						return 1;
					}
					std::vector<float> lFloats = Strs2Flts(pParameterVector);
					mGfxMesh.mOrientation.Set(&lFloats[0]);
					mGfxMesh.mPos.Set(&lFloats[4]);
					mGfxMesh.mVertices.clear();
					mGfxMesh.mIndices.clear();
					mGfxMesh.mVertices.insert(mGfxMesh.mVertices.end(), mVertices.begin(), mVertices.end());
					mGfxMesh.mIndices.insert(mGfxMesh.mIndices.end(), mIndices.begin(), mIndices.end());
				}
				break;
				case COMMAND_SET_VERTICES:
					mVertices.clear();
					// TRICKY: fall through!
				case COMMAND_ADD_VERTICES:
				{
					Params2Floats(pParameterVector, mVertices);
				}
				break;
				case COMMAND_SET_INDICES:
					mIndices.clear();
					// TRICKY: fall through!
				case COMMAND_ADD_INDICES:
				{
					Params2Ints(pParameterVector, mIndices);
				}
				break;
				case COMMAND_ARE_LOADED:
				{
					std::vector<int> lObjectIds;
					Params2Ints(pParameterVector, lObjectIds);
					bool lFirst = true;
					for (std::vector<int>::iterator y = lObjectIds.begin(); y != lObjectIds.end(); ++y)
					{
						if (!lFirst)
						{
							mActiveResponse += _T(',');
						}
						lFirst = false;
						mActiveResponse += lManager->IsLoaded(*y)? _T("1") : _T("0");
					}
				}
				break;
				case COMMAND_WAIT_UNTIL_LOADED:
				{
					int lObjectId = ParamToInt(pParameterVector, 0);
					bool lLoaded = false;
					for (int x = 0; !lLoaded && x < 150; ++x)
					{
						lLoaded = lManager->IsLoaded(lObjectId);
						if (!lLoaded)
						{
							Thread::Sleep(0.01);
						}
					}
					if (!lLoaded)
					{
						mLog.Warningf(_T("Object %i did not load in time. Try calling again."), lObjectId);
					}
				}
				break;
				case COMMAND_EXPLODE:
				{
					lManager->Explode(ParamToVec3(pParameterVector, 0), ParamToVec3(pParameterVector, 3), ParamToFloat(pParameterVector, 6));
				}
				break;
				case COMMAND_PLAY_SOUND:
				{
					lManager->PlaySound(ParamToStr(pParameterVector, 0), ParamToVec3(pParameterVector, 1), ParamToVec3(pParameterVector, 4), ParamToFloat(pParameterVector, 7));
				}
				break;
				case COMMAND_POP_COLLISIONS:
				{
					strutil::strvec lLines;
					typedef TrabantSimManager::CollisionList CollisionList;
					CollisionList lList;
					lManager->PopCollisions(lList);
					int y = 0;
					for (CollisionList::iterator x = lList.begin(); x != lList.end() && y < 10; ++x, ++y)
					{
						lLines.push_back(strutil::Format(_T("%i %f %f %f %f %f %f %i"), x->mObjectId, x->mForce.x, x->mForce.y, x->mForce.z,
								x->mPosition.x, x->mPosition.y, x->mPosition.z, x->mOtherObjectId));
					}
					mActiveResponse += strutil::Join(lLines, _T("\n"));
				}
				break;
				case COMMAND_GET_KEYS:
				{
					strutil::strvec lKeys;
					lManager->GetKeys(lKeys);
					mActiveResponse += strutil::Join(lKeys, _T("\n"));
				}
				break;
				case COMMAND_GET_TOUCH_DRAGS:
				{
					PixelRect lRect = lManager->GetRenderArea();
					const float sx = 1.0f/lRect.GetWidth();
					const float sy = 1.0f/lRect.GetHeight();
					strutil::strvec lLines;
					typedef TrabantSimManager::DragList DragList;
					DragList lList;
					lManager->GetTouchDrags(lList);
					for (DragList::iterator x = lList.begin(); x != lList.end(); ++x)
					{
						lLines.push_back(strutil::Format(_T("%f %f %f %f %f %f %s %i"), x->mLast.x*sx, x->mLast.y*sy, x->mStart.x*sx, x->mStart.y*sy,
								x->mVelocity.x*sx, x->mVelocity.y*sy, x->mIsPress?_T("true"):_T("false"), x->mButtonMask));
					}
					mActiveResponse += strutil::Join(lLines, _T("\n"));
				}
				break;
				case COMMAND_GET_ACCELEROMETER:
				{
					const vec3 a = lManager->GetAccelerometer();
					mActiveResponse += strutil::Format(_T("%f %f %f"), a.x, a.y, a.z);
				}
				break;
				case COMMAND_GET_MOUSEMOVE:
				{
					const vec3 m = lManager->GetMouseMove();
					mActiveResponse += strutil::Format(_T("%f %f %f"), m.x, m.y, m.z);
				}
				break;
				case COMMAND_CREATE_JOYSTICK:
				{
					const int lJoyId = lManager->CreateJoystick(ParamToFloat(pParameterVector, 0), ParamToFloat(pParameterVector, 1), ParamToBool(pParameterVector, 2));
					mActiveResponse += ToStr(lJoyId);
				}
				break;
				case COMMAND_GET_JOYSTICK_DATA:
				{
					strutil::strvec lLines;
					typedef TrabantSimManager::JoystickDataList JoyList;
					JoyList lList = lManager->GetJoystickData();
					for (JoyList::iterator x = lList.begin(); x != lList.end(); ++x)
					{
						lLines.push_back(strutil::Format(_T("%i %f %f"), x->mJoystickId, x->x, x->y));
					}
					mActiveResponse += strutil::Join(lLines, _T("\n"));
				}
				break;
				case COMMAND_GET_ASPECT_RATIO:
				{
					mActiveResponse += ToStr(lManager->GetAspectRatio());
				}
				break;
				case COMMAND_CREATE_ENGINE:
				{
					const int lObjectId = ParamToInt(pParameterVector, 0);
					const str lEngineType = ParamToStr(pParameterVector, 1);
					const vec2 lMaxVelocity = ParamToVec2(pParameterVector, 2);
					const float lStrength = ParamToFloat(pParameterVector, 4);
					const float lFriction = ParamToFloat(pParameterVector, 5);
					TrabantSimManager::EngineTargetList lTargets;
					size_t lCount = pParameterVector.size();
					for (size_t x = 6; x+1 < lCount; x += 2)
					{
						lTargets.push_back(TrabantSimManager::EngineTarget(ParamToInt(pParameterVector, x), ParamToFloat(pParameterVector, x+1)));
					}
					const int lEngineId = lManager->CreateEngine(lObjectId, lEngineType, lMaxVelocity, lStrength, lFriction, lTargets);
					if (lEngineId < 0)
					{
						throw ParameterException();
					}
					mActiveResponse += ToStr(lEngineId);
				}
				break;
				case COMMAND_CREATE_JOINT:
				{
					const int lObjectId = ParamToInt(pParameterVector, 0);
					const str lJointType = ParamToStr(pParameterVector, 1);
					const int lOtherObjectId = ParamToInt(pParameterVector, 2);
					const vec3 lAxis = ParamToVec3(pParameterVector, 3);
					const vec2 lStop = ParamToVec2(pParameterVector, 6);
					const vec2 lSpring = ParamToVec2(pParameterVector, 8);
					const int lJointId = lManager->CreateJoint(lObjectId, lJointType, lOtherObjectId, lAxis, lStop, lSpring);
					if (lJointId < 0)
					{
						throw ParameterException();
					}
					mActiveResponse += ToStr(lJointId);
				}
				break;
				case COMMAND_POSITION:
				{
					bool lIsSet;
					vec3 lValue = ParamToVec3(pParameterVector, 1, &lIsSet);
					lManager->Position(ParamToInt(pParameterVector, 0), lIsSet, lValue);
					if (!lIsSet)
					{
						mActiveResponse += strutil::Format(_T("%f %f %f"), lValue.x, lValue.y, lValue.z);
					}
				}
				break;
				case COMMAND_ORIENTATION:
				{
					bool lIsSet;
					quat lValue = ParamToQuat(pParameterVector, 1, &lIsSet);
					lManager->Orientation(ParamToInt(pParameterVector, 0), lIsSet, lValue);
					if (!lIsSet)
					{
						mActiveResponse += strutil::Format(_T("%f %f %f %f"), lValue.a, lValue.b, lValue.c, lValue.d);
					}
				}
				break;
				case COMMAND_VELOCITY:
				{
					bool lIsSet;
					vec3 lValue = ParamToVec3(pParameterVector, 1, &lIsSet);
					lManager->Velocity(ParamToInt(pParameterVector, 0), lIsSet, lValue);
					if (!lIsSet)
					{
						mActiveResponse += strutil::Format(_T("%f %f %f"), lValue.x, lValue.y, lValue.z);
					}
				}
				break;
				case COMMAND_ANGULAR_VELOCITY:
				{
					bool lIsSet;
					vec3 lValue = ParamToVec3(pParameterVector, 1, &lIsSet);
					lManager->AngularVelocity(ParamToInt(pParameterVector, 0), lIsSet, lValue);
					if (!lIsSet)
					{
						mActiveResponse += strutil::Format(_T("%f %f %f"), lValue.x, lValue.y, lValue.z);
					}
				}
				break;
				case COMMAND_MASS:
				{
					bool lIsSet;
					float lValue = ParamToFloat(pParameterVector, 1, &lIsSet);
					lManager->Mass(ParamToInt(pParameterVector, 0), lIsSet, lValue);
					if (!lIsSet)
					{
						mActiveResponse += ToStr(lValue);
					}
				}
				break;
				case COMMAND_COLOR:
				{
					bool lIsSet;
					vec3 lValue = ParamToVec3(pParameterVector, 1, &lIsSet);
					float lAlpha = ParamToFloat(pParameterVector, 4, &lIsSet);
					lManager->ObjectColor(ParamToInt(pParameterVector, 0), lIsSet, lValue, lAlpha);
					if (!lIsSet)
					{
						mActiveResponse += strutil::Format(_T("%f %f %f"), lValue.x, lValue.y, lValue.z);
					}
				}
				break;
				case COMMAND_ENGINE_FORCE:
				{
					bool lIsSet;
					vec3 lValue = ParamToVec3(pParameterVector, 2, &lIsSet);
					lManager->EngineForce(ParamToInt(pParameterVector, 0), ParamToInt(pParameterVector, 1), lIsSet, lValue);
					if (!lIsSet)
					{
						mActiveResponse += strutil::Format(_T("%f %f %f"), lValue.x, lValue.y, lValue.z);
					}
				}
				break;
				case COMMAND_SET_TAG_FLOATS:
				{
					mTagFloats.clear();
					Params2Floats(pParameterVector, mTagFloats);
				}
				break;
				case COMMAND_SET_TAG_STRINGS:
				{
					mTagStrings = pParameterVector;
				}
				break;
				case COMMAND_SET_TAG_PHYS:
				{
					mTagPhys.clear();
					Params2Ints(pParameterVector, mTagPhys);
				}
				break;
				case COMMAND_SET_TAG_ENGINE:
				{
					mTagEngines.clear();
					Params2Ints(pParameterVector, mTagEngines);
				}
				break;
				case COMMAND_SET_TAG_MESH:
				{
					mTagMeshes.clear();
					Params2Ints(pParameterVector, mTagMeshes);
				}
				break;
				case COMMAND_ADD_TAG:
				{
					const int lObjectId = ParamToInt(pParameterVector, 0);
					const str lTagType = ParamToStr(pParameterVector, 1);
					lManager->AddTag(lObjectId, lTagType, mTagFloats, mTagStrings, mTagPhys, mTagEngines, mTagMeshes);
				}
				break;
				default:
				{
					lResult = -1;
				}
				break;
			}
		}
		catch (ParameterException)
		{
			lResult = 1;
			mActiveResponse = strutil::Format(_T("ERROR: bad or missing parameter for %s (%s)!\n"), pCommand.c_str(), strutil::Join(pParameterVector, _T(", ")).c_str());
			mLog.Warningf(_T("%s has missing or bad arguments (%s)"), pCommand.c_str(), strutil::Join(pParameterVector, _T(", ")).c_str());
		}
	}
	return (lResult);
}



loginstance(CONSOLE, TrabantSimConsoleManager);



}
