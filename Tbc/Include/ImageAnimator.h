/*
	Class:  ImageAnimator
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES:

	This class keeps track on texture/image animations. It doesn't
	actually update the images shown on screen, but will always
	let you know which image to show at any given time.
*/

#pragma once

#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/Timer.h"

#define TEMPLATE template<class TImageIdentifier>
#define QUAL ImageAnimator<TImageIdentifier>


namespace Tbc
{

TEMPLATE class ImageAnimator
{
public:

	enum PlayMode
	{
		LOOP_FORWARD = 0,	// Loops forever.
		LOOP_BACKWARDS,		// ...
		PINGPONG,		// Will play the anim back and forth forever.
		FORWARD_TO_END,		// Play once to the last frame and freeze.
		FORWARD_TO_START,	// Play once and loop over to the first frame, then freeze.
		BACKWARD_TO_END,	// Play once backwards and loop over to the first frame, the freeze.
		BACKWARD_TO_START,	// Play once backwards until the first frame, then freeze.
	};

	enum StartBehaviour
	{
		INTERRUPT_ANIM = 0,	// Interrupts the currently playing animation.
		WAIT_FOR_ANIM,		// Waits until the currently playing animation has ended.
					// If the animation is looping or in pingpong mode it will
					// be interrupted when it has completed the current cycle.
	};

	ImageAnimator(TImageIdentifier pInvalidID);
	virtual ~ImageAnimator();

	void AddAnimation(const Lepra::PString& pAnimationName,    // The unique name for the animation.
			  TImageIdentifier* pImageID,             // A pointer to an array of image IDs.
			  unsigned pNumFrames,                   // The number of IDs in the array.
			  unsigned pFPS,                         // Frames Per Second.
			  PlayMode pPlayMode);

	// Add an animation with uneven spaces between the frames.
	// In this case, the animation can only be played in FORWARD_TO_END mode.
	void AddAnimation(const Lepra::PString& pAnimationName,    // The unique name for the animation.
			  TImageIdentifier* pImageID,             // A pointer to an array of image IDs.
			  unsigned* pTimeStamp,                 // Time in ms each frame is visible.
			  unsigned pNumFrames);                  // The number of IDs in the array.


	void RemoveAnimation(const Lepra::PString& pAnimationName);
	void RemoveAllAnimations();

	void StartAnimation(const Lepra::PString& pAnimationName, StartBehaviour pBehaviour);
	void FreezeCurrentAnimation();

	TImageIdentifier GetCurrentImageID();

private:

	class Animation
	{
	public:
		Animation();
		Animation(const Animation& pAnimation);
		Animation(const Lepra::PString& pName,
				  TImageIdentifier* pImageID,
				  unsigned* pTimeStamp,
				  unsigned pNumFrames,
				  bool pLooping);
		~Animation();

		inline const Lepra::PString& GetName();
		inline const TImageIdentifier* GetImages();
		inline const unsigned* GetTimeStamps();
		inline unsigned GetNumFrames();
		inline bool GetLooping();

	private:
		Lepra::PString mName;
		TImageIdentifier* mImageID;
		unsigned* mTimeStamp;			// Time stamps in milliseconds.
		unsigned mNumFrames;
		bool mLooping;
	};

	typedef Lepra::HashTable<Lepra::PString, Animation*, Lepra::PString> AnimTable;

	AnimTable mAnimTable;
	Animation* mCurrentAnim;
	Animation* mWaitingAnim;

	Lepra::Timer mTimer;
	unsigned mCurrentFrame;

	TImageIdentifier mInvalidID;
};


TEMPLATE const Lepra::PString& QUAL::Animation::GetName()
{
	return mName;
}

TEMPLATE const TImageIdentifier* QUAL::Animation::GetImages()
{
	return mImageID;
}

TEMPLATE const unsigned* QUAL::Animation::GetTimeStamps()
{
	return mTimeStamp;
}

TEMPLATE unsigned QUAL::Animation::GetNumFrames()
{
	return mNumFrames;
}

TEMPLATE bool QUAL::Animation::GetLooping()
{
	return mLooping;
}

#include "ImageAnimator.inl"

}

#undef TEMPLATE
#undef QUAL
