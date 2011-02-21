/*	Copyright (c) 2010 Michael F. Varga
 *	Email:		michael@engineerutopia.com
 *	Website:	engineerutopia.com
 *	
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *	
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *	
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *	THE SOFTWARE.
*/

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include<map>
#include<fstream>
#include<algorithm>
#include<typeinfo>
#include<string>
#include<stdint.h>

#include<OString.hpp>


using namespace std;

/**	This is a simple relational configuration file parser. It parses files of
 *	the format <key>=<value>.
 *	\code
 *	some.key=123
 *	another.key=test
 *	\endcode
 *	This class contains functions to perform automatic lexical casting for ease
 *	of converting between integers and text.
 *	\code
 *	OProperties prop;
 *	prop.open("test.config");
 *	int num = prop.get<int>("some.key");
 *	OString str = prop.get<OString>("another.key");
 *	\endcode
*/
class OProperties {
public:
	/// Make an empty relational map.
	OProperties();
	
	/// Initialize the relational map with some text contained in a file.
	OProperties(OString fn);
	
	/// Returns true if the relational map does not contain any values.
	bool isEmpty() const;
	
	/**	Open a configuration file containing the sime relational text format.
	 *	@param fn The name of the file to be opened.
	 *	@return false is returned if the file is not found.
	*/
	bool open(OString fn);
	
	/**	Save the configuration file using the simple relational text file format.
	 *	@param fn The name of the file to be saved.
	*/
	void save(OString fn);
	
	OString filename() const;
	
	/// Pass the configuration file to OProperties through a OString.
	void setText(OString text);
	
	/// Get the value for the specified 'key' as a OString.
	OString get(OString key) const;
	
	/// Get the value for the specified 'key' as a OString.
	OString getOString(OString key) const;
	
	/// Get the value for the specified 'key' as an int.
	int getInt(OString key) const;
	
	/// Get the value for the specified 'key' as a float.
	float getFloat(OString key) const;
	
	/// Get the value for the specified 'key' as a double.
	double getDouble(OString key) const;
	
	/// Get the value for the specified 'key' as a desired datatype.
	/// Valid datatypes for T are int, unsigned int, long, unsigned long and double.
	template<class T> T get(OString key) const {
		map<OString, OString>::const_iterator it = values.find(key);
		T block;
		if(it == values.end()) return nullType(block);
		return it->second.as<T>();
	}
	
	/**	Insert a key pair, if key does not already exist. Otherwise the value 
	 *	for the key will be overwritten.
	 *	@param key The reference key value to be 
	 *	@param val The value to assign the key.
	*/
	template<class T> void set(OString key, T val) {
		values[key] = val;
	}
	
	/**	Return a reference to the value for the key in the relational map.
	*/
	OString& operator[](OString key);
	
	/// Get the associate map containing the properties.
	map<OString, OString> getMap() const;
	
	/// Set the associative map containing the properties.
	void setMap(map<OString, OString> vals);
	
protected:
	map<OString, OString> values;
	
	OString					v_fn;
	
#ifdef __windows__
	int8_t nullType(int8_t select) const { return 0; }
	uint8_t nullType(uint8_t select) const { return 0; }
	int16_t nullType(int16_t select) const { return 0; }
	uint16_t nullType(uint16_t select) const { return 0; }
	int32_t nullType(int32_t select) const { return 0; }
	uint32_t nullType(uint32_t select) const { return 0; }
	int64_t nullType(int64_t select) const { return 0; }
	uint64_t nullType(uint64_t select) const { return 0; }
	float nullType(float select) const { return 0; }
	double nullType(double select) const { return 0; }
	OString nullType(OString select) const { return ""; }
#else
	int8_t nullType(int8_t select __attribute__((unused))) const { return 0; }
	uint8_t nullType(uint8_t select __attribute__((unused))) const { return 0; }
	int16_t nullType(int16_t select __attribute__((unused))) const { return 0; }
	uint16_t nullType(uint16_t select __attribute__((unused))) const { return 0; }
	int32_t nullType(int32_t select __attribute__((unused))) const { return 0; }
	uint32_t nullType(uint32_t select __attribute__((unused))) const { return 0; }
	int64_t nullType(int64_t select __attribute__((unused))) const { return 0; }
	uint64_t nullType(uint64_t select __attribute__((unused))) const { return 0; }
	float nullType(float select __attribute__((unused))) const { return 0; }
	double nullType(double select __attribute__((unused))) const { return 0; }
	OString nullType(OString select __attribute__((unused))) const { return ""; }
#endif
	
};

#endif // OProperties_H
