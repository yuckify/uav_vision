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

#ifndef OFormLetter_H
#define OFormLetter_H

#include<fstream>
#include<iostream>
#include<map>

#include<OString.hpp>
#include<OList.hpp>

using namespace std;

/*!	OFormLetter is a simple class for programmatically
 *	generating a letter from a prewritten form that 
 *	might be emailed.\n\n
 *	Example:\n
 *	Dear <tag>__NAME__<\\tag>,
 *	\tWe are happy to see you are actively using our \n
 *	<tag>__SERVICE__<\\tag>. We want you to know if you actuire <tag>__MIN__<\\tag> \n
 *	points we will reward you with <tag>__REWARD__<\\tag>.\n\n
 *	
 *	Sincerely,\n
 *	\t<tag>__ADMIN__<\\tag>\n\n
 *	
 *	This example form letter contains the keys:\n
 *	__NAME__\n
 *	__SERVICE__\n
 *	__MIN__\n
 *	__REWARD__\n
 *	__ADMIN__\n
 *	These keys might be replaced with some user determined values to generate
 *	lots of letters quickly.\n\n
 *	
 *	The code to process this form letter might look like.\n\n
 *	\code
 *	OFormLetter letter("example.txt");
 *
 *	OString name = getNextName();//get another user name from somewhere
 *	
 *	letter.set("__NAME__", name);
 *	letter.set("__SERVICE__", "email");
 *	letter.set("__MIN__", "over 9000");
 *	letter.set("__REWARD__", "free service for a year");
 *	letter.set("__ADMIN__", "Joe");
 *	
 *	letter.generate();
 *	\endcode
 *	
*/
class OFormLetter {
public:
	OFormLetter();
	
	/// Default constructor loads a form letter is specified.
	/// Otherwise it does nothing.
	OFormLetter(OString fn);
	
	/// Open a form letter and load it's contents.
	bool open(OString fn);
	
	/// Get the start tag for a key.
	/// By default it is '<tag>'.
	OString startTag() const;
	
	/// Pass the form letter text to OFormLetter through a OString.
	void setText(OString text);
	
	/// Set the start tag for a key.
	/// By default it is '<tag>'.
	void startTag(OString str);
	
	/// Get the end tag for a key.
	/// By defualt it is '<\tag>'.
	OString endTag() const;
	
	/// Set the end tag for a key.
	/// By default it is '<\tag>'.
	void endTag(OString str);
	
	/// Set a 'key' in the form letter to be replaced with
	/// some 'value'. If this key already exists, the 'value'
	/// of the current key will be so to 'value'.
	void set(OString key, OString value);
	
	/// Remove the 'key' from the listing of keys to be
	/// replaced.
	void remove(OString key);
	
	/// Find all the keys in the form letter and replace them with
	/// the desired values.
	void generate();
	
	/// Get the letter that was generated from the form.
	OString letter() const;
	
	bool saveLetter(OString fn);
	
	/// Returns map<OString, OString> which contains the actual associative
	/// map for the key-value pair.
	map<OString, OString> getMap() const;
	
	/// Set the list of associate values to be used in generating the letter.
	void setMap(map<OString, OString> vals);
	
protected:
	/// The form letter that is loaded from a file.
	OString let;
	
	/// The finished letter that is generated.
	OString fin;
	
	/// The starting tag to denote a key in a form letter.
	OString starttag;
	
	/// The ending tag to denote a key in a form letter.
	OString endtag;
	
	/*
	struct pair {
		pair(OString str1, OString str2) : 
				key(str1), value(str2) {}

		OString key;
		OString value;
	};
	
	/// The list of values to replace the keys in the form letter.
	GList<pair> subs;
	*/
	
	map<OString, OString> subs;
	
};

#endif // OFormLetter_H
