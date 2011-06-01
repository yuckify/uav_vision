#include"OCompatibility.hpp"

#ifdef OO_OPENCV
#ifdef OO_QT

QImage IplImageToQImage(IplImage *img) {
	if(!img) return QImage();
	
	//some simple optimizations if the same sized image
	//is passed to this function repeatedly
	static IplImage* tchannel0 = 0;
	static IplImage* tchannel1 = 0;
	static IplImage* tchannel2 = 0;
	static IplImage* tchannel3 = 0;
	static IplImage* dframe = 0;
	static int width = 0;
	static int height = 0;
	static int channels = 0;
	
	//the input image size or channels has changed so deallocate 
	//the temporary variables
	if(img->width != width || img->height != height || 
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
			width = 0;
			height = 0;
			channels = 0;
			tchannel0 = 0;
			tchannel1 = 0;
			tchannel2 = 0;
			tchannel3 = 0;
			dframe = 0;
		}
	}
	
	//allocate the temporary variables if they have not been allocated
	if(!tchannel0) {
		CvSize size;
		size.height = img->height;
		size.width = img->width;
		width = img->width;
		height = img->height;
		channels = img->nChannels;
		tchannel0 = cvCreateImage(size, IPL_DEPTH_8U, 1);
		tchannel1 = cvCreateImage(size, IPL_DEPTH_8U, 1);
		tchannel2 = cvCreateImage(size, IPL_DEPTH_8U, 1);
		tchannel3 = cvCreateImage(size, IPL_DEPTH_8U, 1);
		dframe = cvCreateImage(size, img->depth, 4);
		
		//clear the alpha channel
		cvSet(tchannel0, cvScalarAll(255), 0);
	}
	
	// with img being the captured frame (3 channel BGR)
	// and dframe the frame to be displayed
	cvSplit(img, tchannel1, tchannel2, tchannel3, NULL);
	cvMerge(tchannel1, tchannel2, tchannel3, tchannel0, dframe);
	
	// point to the image data stored in the IplImage*
	const unsigned char * data = (unsigned char *)(dframe->imageData);
	
	// read other parameters in local variables
	int fwidth = dframe->width;
	int fheight = dframe->height;
	int fbytesPerLine = dframe->widthStep;
	
	// imageframe is my QLabel object
	return QImage(data, fwidth, fheight, fbytesPerLine, QImage::Format_RGB32 );
}

#endif

OByteArray& operator<<(OByteArray& data, IplImage*& img) {
	if(!img) return data;
	
	//serialize the header information for the image data
	data<<img->width <<img->height <<img->nChannels <<img->depth;
	
	int unaligned_width = img->width * img->nChannels;
	
	//serialize the image datas
	const char* ptr = img->imageData;
	for(int i=0; i<img->height; i++) {
		data.write(ptr, unaligned_width);
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
	
	//standardize the header with a known alignment
	int32_t rows = mat->rows;
	int32_t cols = mat->cols;
	int32_t type = mat->type;
	
	//serialize the header
	data<<rows <<cols <<type;
	
	//serialize the data, this data is unaligned so no
	//iteration is necessary
	data.write(mat->data.ptr, mat->step * mat->rows);
	
	return data;
}

OByteArray& operator>>(OByteArray& data, CvMat*& mat) {
	int32_t rows;
	int32_t cols;
	int32_t type;
	
	//deserialize the header
	data>>rows >>cols >>type;
	
	//allocate the data
	mat = cvCreateMat(rows, cols, type);
	
	//deserialize the data
	data.read((char*)mat->data.ptr, mat->step * mat->rows);
	
	return data;
}

#endif
