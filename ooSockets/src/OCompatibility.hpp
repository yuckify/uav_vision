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

#ifdef OO_QT
/**	Turn an IplImage into a QImage.
*/
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
