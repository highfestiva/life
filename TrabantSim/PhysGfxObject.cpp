
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "physgfxobject.h"



namespace TrabantSim {



PlacedObject::PlacedObject(const quat& orientation, const vec3& pos):
	orientation_(orientation),
	pos_(pos) {
}

PlacedObject::~PlacedObject() {
}



BoxObject::BoxObject(const quat& _quat, const vec3& pos, const vec3& size):
	Parent(_quat,pos),
	size_(size) {
}



SphereObject::SphereObject(const quat& _quat, const vec3& pos, float radius):
	Parent(_quat,pos),
	radius_(radius) {
}



CapsuleObject::CapsuleObject(const quat& _quat, const vec3& pos, float radius, float length):
	Parent(_quat,pos),
	radius_(radius),
	length_(length) {
}



MeshObject::MeshObject():
	Parent(quat(), vec3()) {
}

MeshObject::MeshObject(const quat& _quat, const vec3& pos):
	Parent(_quat,pos) {
}



}
