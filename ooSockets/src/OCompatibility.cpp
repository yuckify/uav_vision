#include"OCompatibility.hpp"

#ifdef OO_OPENCV

QImage IplImageToQImage(IplImage *img) {
	if(!img) return QImage();
	
	//some simple optimizations if the same sized image
	//is passed to this function repeatedly
	static IplImage* tchannel0 = NULL;
	static IplImage* tchannel1 = NULL;
	static IplImage* tchannel2 = NULL;
	static IplImage* tchannel3 = NULL;
	static IplImage* dframe = NULL;
	static CvSize size = {0, 0};
	static int channels = 0;
	
	//the input image size or channels has changed so deallocate 
	//the temporary variables
	if(img->width != size.width || img->height != size.height || 
	   img->nChannels != channels) {
		//make sure the variables has been allocate for in the first
		//place
		if(tchannel0) {
			//release all the memory in preparation for reallocation
			//for the differing sized image
			cvReleaseImage(&tchannel0);
			cvReleaseImage(&tchannel1);
			cvReleaseImage(&tchannel2);
			cvReleaseImage(&tchannel3);
			cvReleaseImage(&dframe);
			size = {0, 0};
			channels = 0;
			tchannel0 = NULL;
			tchannel1 = NULL;
			tchannel2 = NULL;
			tchannel3 = NULL;
			dframe = NULL;
		}
	}
	
	//allocate the temporary variables if they have not been allocated
	if(!tchannel0) {
		CvSize size;
		size.height = img->height;
		size.width = img->height;
		channels = img->nChannels;
		tchannel0 = cvCreateImage(size, IPL_DEPTH_8U, channels);
		tchannel1 = cvCreateImage(size, IPL_DEPTH_8U, channels);
		tchannel2 = cvCreateImage(size, IPL_DEPTH_8U, channels);
		tchannel3 = cvCreateImage(size, IPL_DEPTH_8U, channels);
		dframe = cvCreateImage(size, img->depth, 4);
		
		//clear the alpha channel
		cvSet(tchannel0,cvScalarAll(255),0);
	}
	
	// with img being the captured frame (3 channel BGR)
	// and dframe the frame to be displayed
	cvSplit(img, tchannel1, tchannel2, tchannel3, NULL);
	cvMerge(tchannel1, tchannel2, tchannel3, tchannel0, dframe);
	
	// point to the image data stored in the IplImage*
	const unsigned char * data = (unsigned char *)(dframe->imageData);
	
	// read other parameters in local variables
	int width = dframe->width;
	int height = dframe->height;
	int bytesPerLine = dframe->widthStep;
	
	// imageframe is my QLabel object
	return QImage(data, width, height, bytesPerLine, QImage::Format_RGB32 );
}

OByteArray& operator<<(OByteArray& data, IplImage*& img) {
	if(!img) return data;
	
	//serialize the header information for the image data
	data<<img->width <<img->height <<img->nChannels <<img->depth;
	
	int unaligned_width = img->width * img->nChannels;
	
	//serialize the image datas
	const char* ptr = img->imageData;
	for(int i=0; i<img->height; i++) {
		data.append(ptr, unaligned_width);
		ptr += img->widthStep;
	}
	
	return data;
}

OByteArray& operator>>(OByteArray& data, IplImage*& img) {
	//deserialize the header of the image
	int width;
	int height;
	int channels;
	int depth;
	
	data>>width >>height >>channels >>depth;
	
	CvSize size;
	size.width = width;
	size.height = height;
	
	//allocate the image
	img = cvCreateImage(size, depth, channels);
	
	int unaligned_width = img->width * img->nChannels;
	
	//deserialize the image data
	char* ptr = img->imageData;
	for(int i=0; i<img->height; i++) {
		data.read(ptr, unaligned_width);
		ptr += img->widthStep;
	}
	
	return data;
}

OByteArray& operator<<(OByteArray& data, CvMat*& mat) {
	if(!mat) return data;
	
	//serialize the header
	data<<mat->rows <<mat->cols <<mat->type;
	
	//serialize the data, this data is unaligned so no
	//iteration is necessary
	data.append((const char*)mat->data.ptr, mat->step * mat->rows);
	
	return data;
}

OByteArray& operator>>(OByteArray& data, CvMat*& mat) {
	int rows;
	int cols;
	int type;
	
	//deserialize the header
	data>>rows >>cols >>type;
	
	//allocate the data
	mat = cvCreateMat(rows, cols, type);
	
	//deserialize the data
	data.read((char*)mat->data.ptr, mat->step * mat->rows);
	
	return data;
}

#endif
