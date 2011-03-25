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
	Unknown,
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
	ImageDetails,
	Image,
	
	CompressionMethod,
	ErrorMsg,
	
	//camera controls
	CameraZoomIn,
	CameraZoomOut,
	CameraPower,
	CameraCapture,
	CameraDownload,
	CameraStatus,
	
};

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
