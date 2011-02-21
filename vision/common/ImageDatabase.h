#ifndef IMAGEDATABASE_H
#define IMAGEDATABASE_H

#include<OByteArray.hpp>
#include<OString.hpp>
#include<sys/types.h>
#include<fstream>

struct ImageInfo : public OSerializable {
	ImageInfo();
	ImageInfo(OString name, uint16_t yaw, uint16_t pitch, uint16_t roll,
			  uint32_t x, uint32_t y, uint32_t alt);
	
	OString		i_name;
	
	//the parameters at the time the image was capture
	uint16_t i_yaw;
	uint16_t i_pitch;
	uint16_t i_roll;
	
	uint32_t i_x;		//gps x coordinate
	uint32_t i_y;		//gps y coordinate
	
	uint32_t i_alt;		//altitude
	
	bool i_downloaded;	//this field tells tells us if the image
						//has been downloaded form the camera to
						//the flight linux computer
	
	void serialize(OByteArray &ar, OO::ArrayBase ba);
	
};

class ImageDatabase : public OSerializable {
public:
	ImageDatabase();
	
	OStringList getImageNames() const;
	
	ImageInfo& get(int num);
	ImageInfo& operator[](int i);
	
	void add(ImageInfo info = ImageInfo());
	
	void save(OString fn);
	bool load(OString fn);
	
	unsigned int size() const;
	unsigned int size();
	
	//This function is called by ByteArray to serialize the entire
	//image database
	void serialize(OByteArray& ar, OO::ArrayBase ba);
	
protected:
	OList<ImageInfo> images;
};

#endif // IMAGEDATABASE_H
