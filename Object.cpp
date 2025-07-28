#include "chunk.h"
#include "Object.h"


Object::Object(const Object& other) {
	this->type = other.type;
}

ObjString::ObjString( const char* start, int length) :s(start, length), Object(ObjType::OBJ_STRING) {
	
};
ObjString::ObjString(const std::string& a) :s(a), Object(ObjType::OBJ_STRING) {};

ObjString::ObjString(const ObjString& other) : Object(ObjType::OBJ_STRING) {
	this->s = other.s;
	//this->hash = other.hash;
};

void ObjString::print() const { // The print method you want to access
	s.empty() ? std::cout<<" <script> " : std::cout << s;
}

ObjType Object::obj_type() const {
	return type;
}
//ObjString::~ObjString() {
//	//std::cout << "string object destrutor\n";
//}
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
ObjString ObjString::operator+(const ObjString& other) const {
	std::string combined = s + other.s;
	return ObjString(combined);
}

//FNV-1a hash function
uint32_t ObjString::hash_string(const std::string& s) {
	uint32_t hash = 2166136261u;
	for (const char c : s) {
		hash ^= static_cast<uint8_t>(c);
		hash *= 16777619;
	}
	return hash;
}

bool ObjString::operator==(const ObjString& other) {
	return this->s == other.s;
}
bool ObjString::operator!=(const ObjString& other) {
	return this->s != other.s;
}


const std::string& ObjString::get_string() const {
	return s;
}

const uint32_t ObjString::get_hash() {
	return hash_string(this->get_string());
}

std::shared_ptr<Object> ObjString::clone() const  {
	return std::make_shared<ObjString>(*this); 
}

ObjFunction::ObjFunction(const std::string& func_name) :Object(ObjType::OBJ_FUNCTION), arity(0) {
	name = std::make_shared<ObjString>(func_name);// check if its necessary
}
ObjFunction::ObjFunction(const ObjFunction& other): Object(ObjType::OBJ_FUNCTION) {
	this->arity = other.arity;
	this->name = std::static_pointer_cast<ObjString>(other.name->clone());
	this->chunk = other.chunk;

}

void ObjFunction::print() const {
	name->print();
}
//check once
bool ObjFunction::compare(const Object* other)const {
	if (other->obj_type() != this->obj_type()) return false;
	const ObjFunction* other_func = dynamic_cast<const ObjFunction*>(other);
	if ((other_func->name->get_hash() != this->name->get_hash()) && (other_func->arity != this->arity)) return false;
	return true;
}
std::shared_ptr<Object> ObjFunction::clone()const {
	return std::make_shared<ObjFunction>(*this);
}