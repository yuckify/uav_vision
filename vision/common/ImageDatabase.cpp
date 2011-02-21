#include "ImageDatabase.h"

ImageInfo::ImageInfo() {
	i_yaw = 0;
	i_pitch = 0;
	i_roll = 0;
	i_x = 0;
	i_y = 0;
	i_downloaded = false;
}

ImageInfo::ImageInfo(OString name, uint16_t yaw, uint16_t pitch, 
					 uint16_t roll, uint32_t x, uint32_t y, uint32_t alt) {
	i_name = name;
	i_yaw = yaw;
	i_pitch = pitch;
	i_roll = roll;
	i_x = x;
	i_y = y;
	i_alt = alt;
	i_downloaded = false;
}

void ImageInfo::serialize(OByteArray &ar, OO::ArrayBase ba) {
	ar & i_name;
	ar & i_yaw;
	ar & i_pitch;
	ar & i_roll;
	ar & i_x;
	ar & i_y;
	ar & i_alt;
	ar & i_downloaded;
}

ImageDatabase::ImageDatabase() {
	
}

OStringList ImageDatabase::getImageNames() const {
	OStringList list;
	for(unsigned int i=0; i<images.size(); i++) {
		list.push_back(images[i].i_name);
	}
	return list;
}

ImageInfo& ImageDatabase::get(int num) {
	return images[num];
}

ImageInfo& ImageDatabase::operator[](int i) {
	return images[i];
}

void ImageDatabase::add(ImageInfo info) {
	images.push_back(info);
}

void ImageDatabase::save(OString fn) {
	fstream file;
	file.open(fn.toCString(), ios_base::out);
	if(!file.is_open()) return;
	
	for(unsigned int i=0; i<images.size(); i++) {
		ImageInfo& info = images[i];
		file<<info.i_name <<"," <<info.i_yaw <<"," <<info.i_pitch <<"," 
				<<info.i_roll <<"," <<info.i_x <<"," <<info.i_y <<endl;
	}
}

bool ImageDatabase::load(OString fn) {
	fstream file;
	file.open(fn.toCString(), ios_base::in);
	if(!file.is_open()) return false;
	
	for(unsigned int i=0; i<images.size(); i++) {
		OString line;
		getline(file, line);
		
		if(line.isEmpty()) continue;
		
		OStringList args = line.split(",");
		
		if(args.size() < 6) continue;
		
		images.push_back(ImageInfo(args[0], (uint16_t)args[1].toInt(),
								   (uint16_t)args[2].toInt(), (uint16_t)args[3].toInt(),
								   args[4].toInt(), args[5].toInt(), args[6].toInt()));
	}
	
	return true;
}

unsigned int ImageDatabase::size() {
	return images.size();
}

unsigned int ImageDatabase::size() const {
	return images.size();
}

void ImageDatabase::serialize(OByteArray &ar, OO::ArrayBase ba) {
	ar & images;
}
