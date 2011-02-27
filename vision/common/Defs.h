#ifndef DEFS_H
#define DEFS_H

#include<stdint.h>

#include<boost/thread/mutex.hpp>

using boost::mutex;
using boost::unique_lock;

#define PacketType		uint8_t
#define PacketLength	uint16_t

#define ImageNumber		uin32_t
#define ImageSegment	uint16_t


enum CameraControl {
	Power			= 'p',
	Capture			= 'c',
	ZoomIn			= 'i',
	ZoomOut			= 'o',
	UsbEnable		= 'u',
	UsbDisable		= 'b',
	Pan				= 'q',
	Tilt			= 'w'
	
	
	
};

namespace Camera {
enum CameraState {
	Ready,
	Sleeping,
	PowerOff,
	Downloading,
	Capturing,
	Zooming,
	
	Connected,
	Disconnected
};
}

/**	Packet Definitions
 *	down:	packets sent from the plane to the ground
 *	up:		packets sent from the ground to the plane
 *	
 *	General Packet Structure:
 *	<length><packet type><payload>
 *	
 *	Packet Payload Definitions:
 *	
 *	Video Packet Segment:
 *	down:	<frame number><segment number><segment data>
 *	up:		
 *	
 *	AutoPilot Telemetry
 *	down:	<string length><string data>
 *	up:		
 *	
*/
enum Packet {
	//streaming packets
	VideoFrameHeader = 1,
	VideoFrameSegment,
	Telemetry,
	
	//high resolutions image details
	ImageList,
	ImageDetails,
	ImageSeg,
	
	CompressionMethod,
	ErrorMsg,
	
	//camera controls
	CameraZoomIn,
	CameraZoomOut,
	CameraPower,
	CameraCapture,
	CameraDownload,
	CameraStatus,
	
	Alignment
};

const unsigned char reliable_data[] = {251,7,218,240,202,202,223, \
									   169,0,173,9,192,115,124,50, \
									   170,140,213,52,177,34,29,78, \
									   93,87,233,196,239,135,166, \
									   164,131,46,254,244,248,201, \
									   212,34,74,2,44,11,244,168,61 \
									   ,32,181,146,84,230,180,240, \
									   54,146,73,159,87,184,40,125, \
									   93,43,171,92,32,164,165,116, \
									   71,239,246,115,250,235,155, \
									   183,12,81,75,224,57,127,209, \
									   238,145,154,14,104,83,54,230, \
									   176,98,18,140,2,55,178,246};

struct PlaneInfo : public OSerializable {
	PlaneInfo() {
		yaw = 0;
		pitch = 0;
		roll = 0;
		x = 0;
		y = 0;
		alt = 0;
	}
	
	void serialize(OByteArray &ar, OO::ArrayBase ba) {
		unique_lock<mutex> locker(tex);
		
		ar & yaw;
		ar & pitch;
		ar & roll;
		ar & x;
		ar & y;
		ar & alt;
	}
	
	void lock() {
		tex.lock();
	}
	
	void unlock() {
		tex.unlock();
	}
	
	mutex tex;
	
	uint16_t yaw;
	uint16_t pitch;
	uint16_t roll;
	uint32_t x;
	uint32_t y;
	uint32_t alt;
};

#endif // DEFS_H
