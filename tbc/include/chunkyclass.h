
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <set>
#include "tbc.h"



namespace tbc {



class ChunkyPhysics;



class ChunkyClass {
public:
	struct Tag {
		str tag_name_;
		std::vector<float> float_value_list_;
		std::vector<str> string_value_list_;
		std::vector<int> body_index_list_;
		std::vector<int> engine_index_list_;
		std::vector<int> mesh_index_list_;
	};

	ChunkyClass();
	virtual ~ChunkyClass();

	const str& GetPhysicsBaseName() const;
	str& GetPhysicsBaseName();

	bool UnpackTag(uint8* buffer, unsigned size);
	void AddTag(const Tag& tag);
	void RemoveTag(size_t tag_index);
	size_t GetTagCount() const;
	const Tag& GetTag(size_t tag_index) const;
	const Tag* GetTag(const str& tag_name) const;

	void AddPhysRoot(int phys_index);
	bool IsPhysRoot(int phys_index) const;

protected:
private:
	str physics_base_name_;
	std::vector<Tag> tag_array_;
	std::set<int> phys_root_set_;

	logclass();
};



}
