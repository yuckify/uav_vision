#include"OFormLetter.hpp"

OFormLetter::OFormLetter() : 
		starttag("<tag>"), endtag("<\\tag>") {
	
}

OFormLetter::OFormLetter(OString fn) : 
		starttag("<tag>"), endtag("<\\tag>") {
	
	//open the form letter using the specified file path
	open(fn);
}

bool OFormLetter::open(OString fn) {
	//clear the previous letter is there were one
	let.clear();
	
	//if the input was empty then do nothing
	if(fn.isEmpty()) {
		return false;
	}
	
	//open the specified file and load the text data
	fstream file;
	file.open(fn.toCString(), ios_base::in);
	
	//if the file doesn't open then something is wrong,
	//so just return
	if(!file.is_open())
		return false;
	
	//load the file
	register char ch;
	while((ch = file.get()) != EOF) {
		let.push_back(ch);
	}
	return true;
}

OString OFormLetter::startTag() const {
	return starttag;
}

void OFormLetter::setText(OString text) {
	let = text;
}

void OFormLetter::startTag(OString str) {
	starttag = str;
}

OString OFormLetter::endTag() const {
	return endtag;
}

void OFormLetter::endTag(OString str) {
	endtag = str;
}

void OFormLetter::set(OString key, OString value) {
	//if the key is already in the map then replace it's
	//value
	subs[key] = value;
}

void OFormLetter::remove(OString key) {
	subs.erase(subs.find(key));
}

void OFormLetter::generate() {
	OString tmp;
	register int start = 0;
	register int end = 0;
	register int startlen = starttag.length();
	register int endlen = endtag.length();
	register int old = 0;
	
	while((start = let.indexOf(starttag, old)) > 0) {
		tmp.append(let.substring(old, start));
		end = let.indexOf(endtag, old);
		
		if(end == -1) {
			return;
		}
		
		//get the key in the form letter
		OString key = let.substring(start + startlen, end);
		
		//replace the key in the form letter with the
		//value listed in the key map
		if(!subs[key].isEmpty()) {
			tmp.append(subs[key]);
		}
		
		old = end + endlen;
	}
	tmp.append(let.substring(old));
	fin = tmp;
}

OString OFormLetter::letter() const {
	return let;
}

bool OFormLetter::saveLetter(OString fn) {
	fstream file;
	file.open(fn.toCString(), ios_base::out);
	if(!file.is_open()) return false;
	
	file<<fin;
	
	file.close();
	return true;
}

map<OString, OString> OFormLetter::getMap() const {
	return subs;
}

void OFormLetter::setMap(map<OString, OString> vals) {
	subs = vals;
}
