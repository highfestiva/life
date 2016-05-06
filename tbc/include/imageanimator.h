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

#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/timer.h"

#define TEMPLATE template<class TImageIdentifier>
#define QUAL ImageAnimator<TImageIdentifier>


namespace tbc {

TEMPLATE class ImageAnimator {
public:

	enum PlayMode {
		kLoopForward = 0,	// Loops forever.
		kLoopBackwards,		// ...
		kPingpong,		// Will play the anim back and forth forever.
		kForwardToEnd,		// Play once to the last frame and freeze.
		kForwardToStart,	// Play once and loop over to the first frame, then freeze.
		kBackwardToEnd,	// Play once backwards and loop over to the first frame, the freeze.
		kBackwardToStart,	// Play once backwards until the first frame, then freeze.
	};

	enum StartBehaviour {
		kInterruptAnim = 0,	// Interrupts the currently playing animation.
		kWaitForAnim,		// Waits until the currently playing animation has ended.
					// If the animation is looping or in pingpong mode it will
					// be interrupted when it has completed the current cycle.
	};

	ImageAnimator(TImageIdentifier invalid_id);
	virtual ~ImageAnimator();

	void AddAnimation(const lepra::PString& animation_name,    // The unique name for the animation.
			  TImageIdentifier* image_id,             // A pointer to an array of image IDs.
			  unsigned num_frames,                   // The number of IDs in the array.
			  unsigned fps,                         // Frames Per Second.
			  PlayMode play_mode);

	// Add an animation with uneven spaces between the frames.
	// In this case, the animation can only be played in kForwardToEnd mode.
	void AddAnimation(const lepra::PString& animation_name,    // The unique name for the animation.
			  TImageIdentifier* image_id,             // A pointer to an array of image IDs.
			  unsigned* time_stamp,                 // Time in ms each frame is visible.
			  unsigned num_frames);                  // The number of IDs in the array.


	void RemoveAnimation(const lepra::PString& animation_name);
	void RemoveAllAnimations();

	void StartAnimation(const lepra::PString& animation_name, StartBehaviour behaviour);
	void FreezeCurrentAnimation();

	TImageIdentifier GetCurrentImageID();

private:

	class Animation {
	public:
		Animation();
		Animation(const Animation& animation);
		Animation(const lepra::PString& name,
				  TImageIdentifier* image_id,
				  unsigned* time_stamp,
				  unsigned num_frames,
				  bool looping);
		~Animation();

		inline const lepra::PString& GetName();
		inline const TImageIdentifier* GetImages();
		inline const unsigned* GetTimeStamps();
		inline unsigned GetNumFrames();
		inline bool GetLooping();

	private:
		lepra::PString name_;
		TImageIdentifier* image_id_;
		unsigned* time_stamp_;			// Time stamps in milliseconds.
		unsigned num_frames_;
		bool looping_;
	};

	typedef lepra::HashTable<lepra::PString, Animation*, lepra::PString> AnimTable;

	AnimTable anim_table_;
	Animation* current_anim_;
	Animation* waiting_anim_;

	lepra::Timer timer_;
	unsigned current_frame_;

	TImageIdentifier invalid_id_;
};


TEMPLATE const lepra::PString& QUAL::Animation::GetName() {
	return name_;
}

TEMPLATE const TImageIdentifier* QUAL::Animation::GetImages() {
	return image_id_;
}

TEMPLATE const unsigned* QUAL::Animation::GetTimeStamps() {
	return time_stamp_;
}

TEMPLATE unsigned QUAL::Animation::GetNumFrames() {
	return num_frames_;
}

TEMPLATE bool QUAL::Animation::GetLooping() {
	return looping_;
}

#include "imageanimator.inl"

}

#undef TEMPLATE
#undef QUAL
