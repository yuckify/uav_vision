#include "OProperties.hpp"

OProperties::OProperties() {
	
}

OProperties::OProperties(OString fn) {
	open(fn);
}

bool OProperties::isEmpty() const {
	return !values.size();
}

bool OProperties::open(OString fn) {
	v_fn = fn;
	fstream file;
	file.open(fn, ios_base::in);
	if(!file.is_open()) return false;
	
	OString line;
	while(getline(file, line), !file.eof() && !line.isEmpty()) 
		values[line.substringBefore("=")] = line.substringAfter("=");
	
	file.close();
	
	return true;
}

void OProperties::save(OString fn) {
	v_fn = fn;
	fstream file;
	file.open(fn, ios_base::out);
	if(!file.is_open()) return;
	
	for_each(values.begin(), values.end(),
			 [&file] (pair<OString, OString> p) -> void {
		file<<p.first <<"=" <<p.second <<endl;
	});
	
	file.close();
}

OString OProperties::filename() const {
	return v_fn;
}

void OProperties::setText(OString text) {
	OStringList list = text.split("\n");
	
	while(list.indexOf("") != -1) list.remove("");
	
	for(unsigned i=0; i<list.size(); i++) {
		OString line = list[i];
		values[line.substringBefore("=")] = line.substringAfter("=");
	}
}

OString OProperties::get(OString key) const {
	map<OString, OString>::const_iterator it = values.find(key);
	if(it == values.end()) return "";
	return it->second;
}

OString OProperties::getOString(OString key) const {
	map<OString, OString>::const_iterator it = values.find(key);
	if(it == values.end()) return "";
	return it->second;
}

int OProperties::getInt(OString key) const {
	map<OString, OString>::const_iterator it = values.find(key);
	if(it == values.end()) return 0;
	return it->second.toInt();
}

float OProperties::getFloat(OString key) const {
	map<OString, OString>::const_iterator it = values.find(key);
	if(it == values.end()) return 0;
	return it->second.toDouble();
}

double OProperties::getDouble(OString key) const {
	map<OString, OString>::const_iterator it = values.find(key);
	if(it == values.end()) return 0;
	return it->second.toDouble();
}

OString& OProperties::operator [](OString key) {
	return values[key];
}

map<OString, OString> OProperties::getMap() const {
	return values;
}

void OProperties::setMap(map<OString, OString> vals) {
	values = vals;
}
