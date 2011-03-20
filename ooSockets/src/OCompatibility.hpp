#ifndef OCOMPATIBILITY_HPP
#define OCOMPATIBILITY_HPP

#include<OO.hpp>
#include<OByteArray.hpp>

#ifdef OO_OPENCV
#ifdef OO_QT
#include<QImage>
#endif
#include<opencv/cv.h>
#endif

#ifdef OO_OPENCV

/**	Turn an IplImage into a QImage.
*/
#ifdef OO_QT
QImage IplImageToQImage(IplImage* img);
#endif

/**	OpenCV integration. Serialize an IplImage. This method ONLY 
 *	serializes the image data.
*/
OByteArray& operator<<(OByteArray& data, IplImage*& img);

/**	OpenCV integration. Deserialize an IplImage.
*/
OByteArray& operator>>(OByteArray& data, IplImage*& img);

/**	OpenCV integration. Serialize a CvMat.
*/
OByteArray& operator<<(OByteArray& data, CvMat*& mat);

/**	OpenCV integration. Deserialize a CvMat.
*/
OByteArray& operator>>(OByteArray& data, CvMat*& mat);

#endif



#endif // OCOMPATIBILITY_HPP
