#include "Object.h"
void ObjString::print() const { // The print method you want to access
	std::cout << s;
}

ObjType Object::obj_type() const {
	return type;
}
ObjString::~ObjString() {
	std::cout << "string object destrutor\n";
}
bool ObjString::compare(const Object* other) const {
	if (other->obj_type() != this->obj_type()) return false;
	const ObjString* other_str = dynamic_cast<const ObjString*>(other);
	if (!other_str) return false;
	return s == other_str->s;
}

const std::string& ObjString::get_string() {
	return s;
}
ObjString& ObjString::operator+=(const ObjString& obj) {
	s = s + obj.s;
	return *this;
}  