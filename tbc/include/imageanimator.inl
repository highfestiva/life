/*
	lepra::File:   ImageAnimator.inl
	Class:  ImageAnimator
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

TEMPLATE QUAL::ImageAnimator(TImageIdentifier invalid_id) :
	current_anim_(0),
	waiting_anim_(0),
	current_frame_(0),
	invalid_id_(invalid_id) {
}

TEMPLATE QUAL::~ImageAnimator() {
	RemoveAllAnimations();
}

TEMPLATE void QUAL::AddAnimation(const lepra::PString& animation_name,
								 TImageIdentifier* image_id,
								 unsigned num_frames,
								 unsigned fps,
								 PlayMode play_mode) {
	// Make sure that there is no animation with the same name.
	RemoveAnimation(animation_name);

	lepra::float64 time_step = 1.0 / (lepra::float64)fps;
	lepra::float64 time = time_step;	// One timestep ahead...
	lepra::float64 total_time = 0;

	TImageIdentifier* _image_id = 0;
	unsigned* _time_stamp = 0;
	unsigned _num_frames = 0;

	bool loop = false;

	switch(play_mode) {
		case kLoopForward:
		case kLoopBackwards:
		case kPingpong:
			loop = true;
			break;
	}
	switch(play_mode) {
		case kPingpong:
			_num_frames = num_frames * 2 - 2;
			break;
		case kForwardToStart:
		case kBackwardToEnd:
			_num_frames = num_frames + 1;
			break;
		default:
			_num_frames = num_frames;
	}

	_image_id = new TImageIdentifier[_num_frames];
	_time_stamp = new unsigned[_num_frames];

	total_time = time_step * (lepra::float64)_num_frames;

	int i;
	switch(play_mode) {
		case kLoopBackwards:
		case kBackwardToStart:
		case kBackwardToEnd:
			for (i = 0; i < (int)_num_frames; i++) {
				int index = (int)(num_frames - 1) - i;

				while (index < 0) {
					index += num_frames;
				}

				_image_id[i] = image_id[index % num_frames];
				_time_stamp[i] = (unsigned)(time * 1000.0);
				time += time_step;
			} break;
		case kPingpong:
			for (i = 0; i < (int)num_frames; i++) {
				_image_id[i] = image_id[i];
				_time_stamp[i] = (unsigned)(time * 1000.0);
				time += time_step;
			}

			for (i; i < (int)_num_frames; i++) {
				_image_id[i] = image_id[_num_frames - i];
				_time_stamp[i] = (unsigned)(time * 1000.0);
				time += time_step;
			} break;
		case kLoopForward:
		case kForwardToStart:
		case kForwardToEnd:
		default:
			for (i = 0; i < (int)_num_frames; i++) {
				_image_id[i] = image_id[i % num_frames];
				_time_stamp[i] = (unsigned)(time * 1000.0);
				time += time_step;
			} break;
	}

	Animation* anim = new Animation(animation_name,
									   _image_id,
									   _time_stamp,
									   _num_frames,
									   loop);

	anim_table_.Insert(animation_name, anim);
}

TEMPLATE void QUAL::AddAnimation(const lepra::PString& animation_name,
								 TImageIdentifier* image_id,
								 unsigned* time_stamp,
								 unsigned num_frames) {
	// Make sure that there is no animation with the same name.
	RemoveAnimation(animation_name);

	_image_id = new TImageIdentifier[_num_frames];
	_time_stamp = new unsigned[_num_frames];

	unsigned prev_time = 0;
	unsigned i;
	for (i = 0; i < num_frames; i++) {
		_image_id[i] = image_id[i];

		if (time_stamp[i] >= prev_time) {
			_time_stamp[i] = time_stamp[i];
		} else {
			// The time stamp is wrong... Let's treat it as a difference.
			// This may be totally wrong, but so is the given time stamps!
			_time_stamp[i] = prev_time + time_stamp[i];
		}

		prev_time = _time_stamp[i];
	}

	Animation* anim = new Animation(animation_name,
									   _image_id,
									   _time_stamp,
									   num_frames,
									   false);

	anim_table_.Insert(animation_name, anim);
}

TEMPLATE void QUAL::RemoveAnimation(const lepra::PString& animation_name) {
	AnimTable::Iterator iter;
	iter = anim_table_.Find(animation_name);

	if (iter != anim_table_.End()) {
		Animation* anim = *iter;
		anim_table_.Remove(iter);

		if (anim == current_anim_) {
			current_anim_ = 0;
		}
		if (anim == waiting_anim_) {
			waiting_anim_ = 0;
		}

		delete anim;
	}
}

TEMPLATE void QUAL::RemoveAllAnimations() {
	AnimTable::Iterator iter;
	for (iter = anim_table_.First(); iter != anim_table_.End(); ++iter) {
		Animation* anim = *iter;
		delete anim;
	}

	anim_table_.RemoveAll();

	current_anim_ = 0;
	waiting_anim_ = 0;
	current_frame_ = 0;
}

TEMPLATE void QUAL::StartAnimation(const lepra::PString& animation_name, StartBehaviour behaviour) {
	AnimTable::Iterator iter = anim_table_.Find(animation_name);

	if (iter == anim_table_.End())
		return;

	Animation* anim = *iter;

	switch(behaviour) {
	case kWaitForAnim:
		if (current_anim_ != 0) {
			waiting_anim_ = anim;
			break;
		}
	default:
		current_anim_ = anim;
		waiting_anim_ = 0;
		timer_.UpdateTimer();
		timer_.ClearTimeDiff();
		current_frame_ = 0;
		break;
	}
}

TEMPLATE void QUAL::FreezeCurrentAnimation() {
}

TEMPLATE TImageIdentifier QUAL::GetCurrentImageID() {
	if (current_anim_ == 0) {
		current_anim_ = waiting_anim_;

		if (current_anim_ == 0)
			return invalid_id_;
	}

	unsigned _num_frames = current_anim_->GetNumFrames();
	const unsigned* _time_stamp = current_anim_->GetTimeStamps();
	bool _looping = current_anim_->GetLooping();

	timer_.UpdateTimer();
	unsigned time = (unsigned)timer_.GetTimeDiff();

	while (time > _time_stamp[current_frame_]) {
		// Step to next frame.
		current_frame_++;

		// Check if we reached the end...
		if (current_frame_ >= _num_frames) {
			// Is there another animation waiting?
			if (waiting_anim_ != 0) {
				current_anim_ = waiting_anim_;
				waiting_anim_ = 0;
				current_frame_ = 0;
				timer_.ClearTimeDiff();
				time = 0;
				_num_frames = current_anim_->GetNumFrames();
				_time_stamp = current_anim_->GetTimeStamps();
				_looping = current_anim_->GetLooping();
			} else if(_looping == true) {
				// Restart animation.
				time -= _time_stamp[_num_frames - 1];
				timer_.ReduceTimeDiff((lepra::uint64)_time_stamp[_num_frames - 1]);
				current_frame_ = 0;
			} else {
				current_frame_--;
				break;
			}
		}
	}

	return current_anim_->GetImages()[current_frame_];
}






TEMPLATE QUAL::Animation::Animation() :
	name_(""),
	image_id_(0),
	time_stamp_(0),
	num_frames_(0),
	looping_(false) {
}

TEMPLATE QUAL::Animation::Animation(const Animation& animation) :
	name_(animation.name_),
	image_id_(0),
	time_stamp_(0),
	num_frames_(animation.num_frames_),
	looping_(animation.looping_) {
	if (num_frames_ > 0) {
		image_id_ = new TImageIdentifier[num_frames_];
		time_stamp_ = new unsigned[num_frames_];

		for (unsigned i = 0; i < num_frames_; i++) {
			image_id_[i] = animation.image_id_[i];
			time_stamp_[i] = animation.time_stamp_[i];
		}
	}
}

TEMPLATE QUAL::Animation::Animation(const lepra::PString& name,
									TImageIdentifier* image_id,
									unsigned* time_stamp,
									unsigned num_frames,
									bool looping) :
	name_(name),
	image_id_(0),
	time_stamp_(0),
	num_frames_(num_frames),
	looping_(looping) {
	if (num_frames_ > 0) {
		image_id_ = new TImageIdentifier[num_frames_];
		time_stamp_ = new unsigned[num_frames_];

		for (unsigned i = 0; i < num_frames_; i++) {
			image_id_[i] = image_id[i];
			time_stamp_[i] = time_stamp[i];
		}
	}
}

TEMPLATE QUAL::Animation::~Animation() {
	if (image_id_ != 0) {
		delete[] image_id_;
	}
	if (time_stamp_ != 0) {
		delete[] time_stamp_;
	}
}
