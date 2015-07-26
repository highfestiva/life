/*
	Lepra::File:   ImageAnimator.inl
	Class:  ImageAnimator
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

TEMPLATE QUAL::ImageAnimator(TImageIdentifier pInvalidID) :
	mCurrentAnim(0),
	mWaitingAnim(0),
	mCurrentFrame(0),
	mInvalidID(pInvalidID)
{
}

TEMPLATE QUAL::~ImageAnimator()
{
	RemoveAllAnimations();
}

TEMPLATE void QUAL::AddAnimation(const Lepra::PString& pAnimationName,
								 TImageIdentifier* pImageID,  
								 unsigned pNumFrames,          
								 unsigned pFPS,                
								 PlayMode pPlayMode)
{
	// Make sure that there is no animation with the same name.
	RemoveAnimation(pAnimationName);

	Lepra::float64 lTimeStep = 1.0 / (Lepra::float64)pFPS;
	Lepra::float64 lTime = lTimeStep;	// One timestep ahead...
	Lepra::float64 lTotalTime = 0;

	TImageIdentifier* lImageID = 0;
	unsigned* lTimeStamp = 0;
	unsigned lNumFrames = 0;

	bool lLoop = false;

	switch(pPlayMode)
	{
		case LOOP_FORWARD:
		case LOOP_BACKWARDS:
		case PINGPONG:
			lLoop = true;
			break;
	}
	switch(pPlayMode)
	{
		case PINGPONG:
			lNumFrames = pNumFrames * 2 - 2;
			break;
		case FORWARD_TO_START:
		case BACKWARD_TO_END:
			lNumFrames = pNumFrames + 1;
			break;
		default:
			lNumFrames = pNumFrames;
	}

	lImageID = new TImageIdentifier[lNumFrames];
	lTimeStamp = new unsigned[lNumFrames];

	lTotalTime = lTimeStep * (Lepra::float64)lNumFrames;

	int i;
	switch(pPlayMode)
	{
		case LOOP_BACKWARDS:
		case BACKWARD_TO_START:
		case BACKWARD_TO_END:
			for (i = 0; i < (int)lNumFrames; i++)
			{
				int lIndex = (int)(pNumFrames - 1) - i;

				while (lIndex < 0)
				{
					lIndex += pNumFrames;
				}

				lImageID[i] = pImageID[lIndex % pNumFrames];
				lTimeStamp[i] = (unsigned)(lTime * 1000.0);
				lTime += lTimeStep;
			}
			break;
		case PINGPONG:
			for (i = 0; i < (int)pNumFrames; i++)
			{
				lImageID[i] = pImageID[i];
				lTimeStamp[i] = (unsigned)(lTime * 1000.0);
				lTime += lTimeStep;
			}

			for (i; i < (int)lNumFrames; i++)
			{
				lImageID[i] = pImageID[lNumFrames - i];
				lTimeStamp[i] = (unsigned)(lTime * 1000.0);
				lTime += lTimeStep;
			}
			break;
		case LOOP_FORWARD:
		case FORWARD_TO_START:
		case FORWARD_TO_END:
		default:
			for (i = 0; i < (int)lNumFrames; i++)
			{
				lImageID[i] = pImageID[i % pNumFrames];
				lTimeStamp[i] = (unsigned)(lTime * 1000.0);
				lTime += lTimeStep;
			}
			break;
	}

	Animation* lAnim = new Animation(pAnimationName,
									   lImageID,
									   lTimeStamp,
									   lNumFrames,
									   lLoop);

	mAnimTable.Insert(pAnimationName, lAnim);
}

TEMPLATE void QUAL::AddAnimation(const Lepra::PString& pAnimationName,
								 TImageIdentifier* pImageID,  
								 unsigned* pTimeStamp,        
								 unsigned pNumFrames)
{
	// Make sure that there is no animation with the same name.
	RemoveAnimation(pAnimationName);

	lImageID = new TImageIdentifier[lNumFrames];
	lTimeStamp = new unsigned[lNumFrames];

	unsigned lPrevTime = 0;
	unsigned i;
	for (i = 0; i < pNumFrames; i++)
	{
		lImageID[i] = pImageID[i];

		if (pTimeStamp[i] >= lPrevTime)
		{
			lTimeStamp[i] = pTimeStamp[i];
		}
		else
		{
			// The time stamp is wrong... Let's treat it as a difference.
			// This may be totally wrong, but so is the given time stamps!
			lTimeStamp[i] = lPrevTime + pTimeStamp[i];
		}

		lPrevTime = lTimeStamp[i];
	}

	Animation* lAnim = new Animation(pAnimationName,
									   lImageID,
									   lTimeStamp,
									   pNumFrames,
									   false);

	mAnimTable.Insert(pAnimationName, lAnim);
}

TEMPLATE void QUAL::RemoveAnimation(const Lepra::PString& pAnimationName)
{
	AnimTable::Iterator lIter;
	lIter = mAnimTable.Find(pAnimationName);

	if (lIter != mAnimTable.End())
	{
		Animation* lAnim = *lIter;
		mAnimTable.Remove(lIter);

		if (lAnim == mCurrentAnim)
		{
			mCurrentAnim = 0;
		}
		if (lAnim == mWaitingAnim)
		{
			mWaitingAnim = 0;
		}

		delete lAnim;
	}
}

TEMPLATE void QUAL::RemoveAllAnimations()
{
	AnimTable::Iterator lIter;
	for (lIter = mAnimTable.First(); lIter != mAnimTable.End(); ++lIter)
	{
		Animation* lAnim = *lIter;
		delete lAnim;
	}

	mAnimTable.RemoveAll();

	mCurrentAnim = 0;
	mWaitingAnim = 0;
	mCurrentFrame = 0;
}

TEMPLATE void QUAL::StartAnimation(const Lepra::PString& pAnimationName, StartBehaviour pBehaviour)
{
	AnimTable::Iterator lIter = mAnimTable.Find(pAnimationName);

	if (lIter == mAnimTable.End())
		return;

	Animation* lAnim = *lIter;

	switch(pBehaviour)
	{
	case WAIT_FOR_ANIM:
		if (mCurrentAnim != 0)
		{
			mWaitingAnim = lAnim;
			break;
		}
	default:
		mCurrentAnim = lAnim;
		mWaitingAnim = 0;
		mTimer.UpdateTimer();
		mTimer.ClearTimeDiff();
		mCurrentFrame = 0;
		break;
	}
}

TEMPLATE void QUAL::FreezeCurrentAnimation()
{
}

TEMPLATE TImageIdentifier QUAL::GetCurrentImageID()
{
	if (mCurrentAnim == 0)
	{
		mCurrentAnim = mWaitingAnim;
		
		if (mCurrentAnim == 0)
			return mInvalidID;
	}

	unsigned lNumFrames = mCurrentAnim->GetNumFrames();
	const unsigned* lTimeStamp = mCurrentAnim->GetTimeStamps();
	bool lLooping = mCurrentAnim->GetLooping();

	mTimer.UpdateTimer();
	unsigned lTime = (unsigned)mTimer.GetTimeDiff();

	while (lTime > lTimeStamp[mCurrentFrame])
	{
		// Step to next frame.
		mCurrentFrame++;

		// Check if we reached the end...
		if (mCurrentFrame >= lNumFrames)
		{
			// Is there another animation waiting?
			if (mWaitingAnim != 0)
			{
				mCurrentAnim = mWaitingAnim;
				mWaitingAnim = 0;
				mCurrentFrame = 0;
				mTimer.ClearTimeDiff();
				lTime = 0;
				lNumFrames = mCurrentAnim->GetNumFrames();
				lTimeStamp = mCurrentAnim->GetTimeStamps();
				lLooping = mCurrentAnim->GetLooping();
			}
			else if(lLooping == true)
			{
				// Restart animation.
				lTime -= lTimeStamp[lNumFrames - 1];
				mTimer.ReduceTimeDiff((Lepra::uint64)lTimeStamp[lNumFrames - 1]);
				mCurrentFrame = 0;
			}
			else
			{
				mCurrentFrame--;
				break;
			}
		}
	}

	return mCurrentAnim->GetImages()[mCurrentFrame];
}






TEMPLATE QUAL::Animation::Animation() :
	mName(_T("")),
	mImageID(0),
	mTimeStamp(0),
	mNumFrames(0),
	mLooping(false)
{
}

TEMPLATE QUAL::Animation::Animation(const Animation& pAnimation) :
	mName(pAnimation.mName),
	mImageID(0),
	mTimeStamp(0),
	mNumFrames(pAnimation.mNumFrames),
	mLooping(pAnimation.mLooping)
{
	if (mNumFrames > 0)
	{
		mImageID = new TImageIdentifier[mNumFrames];
		mTimeStamp = new unsigned[mNumFrames];

		for (unsigned i = 0; i < mNumFrames; i++)
		{
			mImageID[i] = pAnimation.mImageID[i];
			mTimeStamp[i] = pAnimation.mTimeStamp[i];
		}
	}
}

TEMPLATE QUAL::Animation::Animation(const Lepra::PString& pName,
									TImageIdentifier* pImageID,
									unsigned* pTimeStamp,
									unsigned pNumFrames,
									bool pLooping) :
	mName(pName),
	mImageID(0),
	mTimeStamp(0),
	mNumFrames(pNumFrames),
	mLooping(pLooping)
{
	if (mNumFrames > 0)
	{
		mImageID = new TImageIdentifier[mNumFrames];
		mTimeStamp = new unsigned[mNumFrames];

		for (unsigned i = 0; i < mNumFrames; i++)
		{
			mImageID[i] = pImageID[i];
			mTimeStamp[i] = pTimeStamp[i];
		}
	}
}

TEMPLATE QUAL::Animation::~Animation()
{
	if (mImageID != 0)
	{
		delete[] mImageID;
	}
	if (mTimeStamp != 0)
	{
		delete[] mTimeStamp;
	}
}
