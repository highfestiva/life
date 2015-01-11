
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
	{_T("create-object"), COMMAND_CREATE_OBJECT},
	{_T("delete-object"), COMMAND_DELETE_OBJECT},
	{_T("delete-all-objects"), COMMAND_DELETE_ALL_OBJECTS},
	{_T("clear-phys"), COMMAND_CLEAR_PHYS},
	{_T("prep-phys-box"), COMMAND_PREP_PHYS_BOX},
	{_T("prep-phys-sphere"), COMMAND_PREP_PHYS_SPHERE},
	{_T("prep-phys-mesh"), COMMAND_PREP_PHYS_MESH},
	{_T("prep-gfx-mesh"), COMMAND_PREP_GFX_MESH},
	{_T("set-vertices"), COMMAND_SET_VERTICES},
	{_T("set-indices"), COMMAND_SET_INDICES},
	{_T("wait-until-loaded"), COMMAND_WAIT_UNTIL_LOADED},
	{_T("explode"), COMMAND_EXPLOAD},
	{_T("play-sound"), COMMAND_PLAY_SOUND},
	{_T("pop-collisions"), COMMAND_POP_COLLISIONS},
	{_T("get-touch-drags"), COMMAND_GET_TOUCH_DRAGS},
	{_T("get-accelerometer"), COMMAND_GET_ACCELEROMETER},
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
};



struct ParameterException
{
};

std::vector<float> Strs2Flts(const strutil::strvec& pStrs)
{
	std::vector<float> lFlts;
	strutil::strvec::const_iterator x = pStrs.begin();
	for(; x != pStrs.end(); ++x)
	{
		double d = 0;
		strutil::StringToDouble(*x, d);
		lFlts.push_back((float)d);
	}
	return lFlts;
}

int StrToUInt(const char* s, const char** pEnd)
{
	int i = 0;
	for(; *s >= '0' && *s <= '9'; ++s)
	{
		i = i*10 + (*s-'0');
	}
	*pEnd = s;
	return i;
}

float StrToFloat(const char* s, const char** pEnd)
{
	float f = 0;
	float ff = 0.1f;
	bool lBeforeDot = true;
	bool lNegative = false;
	for (;; ++s)
	{
		if (*s >= '0' && *s <= '9')
		{
			if (lBeforeDot)
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
			lNegative = true;
		}
		else
		{
			break;
		}
	}
	*pEnd = s;
	return lNegative? -f : f;
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

str TrabantSimConsoleManager::GetActiveResponse() const
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
	mActiveResponse = _T("ok\n");
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
				case COMMAND_CREATE_OBJECT:
				{
					const str lType = ParamToStr(pParameterVector, 0);
					const str lMaterialInfo = ParamToStr(pParameterVector, 1);
					const bool lIsStatic = (lType == _T("static"));
					ObjectMaterial lMaterial = MaterialSmooth;
					if (lMaterialInfo == _T("flat")) lMaterial = MaterialFlat;
					else if (lMaterialInfo == _T("checker")) lMaterial = MaterialChecker;
					const int lObjectId = lManager->CreateObject(mGfxMesh, mPhysObjects, lMaterial, lIsStatic);
					mActiveResponse += ToStr(lObjectId);
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
					PlacedObject* lBox = new SphereObject(quat(&lFloats[0]), vec3(&lFloats[4]), lFloats[7]);
					mPhysObjects.push_back(lBox);
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
				{
					mVertices.clear();
					strutil::strvec::const_iterator p;
					for (p = pParameterVector.begin(); p != pParameterVector.end(); ++p)
					{
						const tchar* s = p->c_str();
						const tchar* lEnd;
						do
						{
							float f = StrToFloat(s, &lEnd);
							mVertices.push_back(f);
							s = lEnd+1;
						}
						while (*lEnd == ',');
					}
				}
				break;
				case COMMAND_SET_INDICES:
				{
					mIndices.clear();
					strutil::strvec::const_iterator p;
					for (p = pParameterVector.begin(); p != pParameterVector.end(); ++p)
					{
						const tchar* s = p->c_str();
						const tchar* lEnd;
						do
						{
							int i = StrToUInt(s, &lEnd);
							mIndices.push_back(i);
							s = lEnd+1;
						}
						while (*lEnd == ',');
					}
				}
				break;
				case COMMAND_WAIT_UNTIL_LOADED:
				{
					for (int x = 0; x < 20; ++x)
					{
						if (lManager->IsLoaded(ParamToInt(pParameterVector, 0)))
						{
							break;
						}
						Thread::Sleep(0.05);
					}
				}
				break;
				case COMMAND_EXPLOAD:
				{
					lManager->Expload(ParamToVec3(pParameterVector, 0), ParamToVec3(pParameterVector, 3));
				}
				break;
				case COMMAND_PLAY_SOUND:
				{
					lManager->PlaySound(ParamToStr(pParameterVector, 0), ParamToVec3(pParameterVector, 1), ParamToVec3(pParameterVector, 4));
				}
				break;
				case COMMAND_POP_COLLISIONS:
				{
					strutil::strvec lLines;
					typedef TrabantSimManager::CollisionList CollisionList;
					CollisionList lList = lManager->PopCollisions();
					for (CollisionList::iterator x = lList.begin(); x != lList.end(); ++x)
					{
						lLines.push_back(strutil::Format(_T("%i %f %f %f %f %f %f %i"), x->mObjectId, x->mForce.x, x->mForce.y, x->mForce.z,
								x->mPosition.x, x->mPosition.y, x->mPosition.z, x->mOtherObjectId));
					}
					mActiveResponse += strutil::Join(lLines, _T("\n"));
				}
				break;
				case COMMAND_GET_TOUCH_DRAGS:
				{
					PixelRect lRect = lManager->GetRenderArea();
					const float sx = 1.0f/lRect.GetWidth();
					const float sy = 1.0f/lRect.GetHeight();
					strutil::strvec lLines;
					typedef TrabantSimManager::DragList DragList;
					DragList lList = lManager->GetTouchDrags();
					for (DragList::iterator x = lList.begin(); x != lList.end(); ++x)
					{
						lLines.push_back(strutil::Format(_T("%f %f %f %f"), x->mLast.x*sx, x->mLast.y*sy, x->mStart.x*sx, x->mStart.y*sy));
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
				case COMMAND_CREATE_JOYSTICK:
				{
					const int lJoyId = lManager->CreateJoystick(ParamToFloat(pParameterVector, 0), ParamToFloat(pParameterVector, 1));
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
					const vec2 lOffset = ParamToVec2(pParameterVector, 4);
					const float lFriction = ParamToFloat(pParameterVector, 6);
					const str lEngineSound = ParamToStr(pParameterVector, 7);
					const int lEngineId = lManager->CreateEngine(lObjectId, lEngineType, lMaxVelocity+lOffset, lFriction, lEngineSound);
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
					const int lJointId = lManager->CreateJoint(lObjectId, lJointType, lOtherObjectId, lAxis);
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
					lManager->ObjectColor(ParamToInt(pParameterVector, 0), lIsSet, lValue);
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
