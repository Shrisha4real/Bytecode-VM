#pragma once
#include<memory>
#include"Table.h"

class StringInterner
{
	std::shared_ptr<Table> strings;
public:
	StringInterner();
	std::shared_ptr<ObjString> allocate_string(const std::string& s);
	std::shared_ptr<ObjString> copy_string(const char* c, int length);
	std::shared_ptr<ObjString> take_string(std::string&& str);
	std::shared_ptr<Table> get_table() const;
};


