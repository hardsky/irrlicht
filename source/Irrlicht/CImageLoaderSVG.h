
#ifndef __C_IMAGE_LOADER_SVG_H_INCLUDED__
#define __C_IMAGE_LOADER_SVG_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_SVG_LOADER_

#include "IImageLoader.h"

namespace irr
{
	namespace video
	{

		//!	 Surface Loader for SVG files
		class CImageLoaderSvg : public IImageLoader
		{
		public:

			//! desired image size in pixels
			CImageLoaderSvg();
			//! returns true if the file maybe is able to be loaded by this class
			//! based on the file extension (e.g. ".svg")
			virtual bool isALoadableFileExtension(const io::path& filename) const _IRR_OVERRIDE_;

			//! returns true if the file maybe is able to be loaded by this class
			virtual bool isALoadableFileFormat(io::IReadFile* file) const _IRR_OVERRIDE_;

			//! creates a surface from the file
			virtual IImage* loadImage(io::IReadFile* file) const _IRR_OVERRIDE_;

			//! creates a surface from the file
			virtual IImage* loadImage(io::IReadFile* file, int width, int height) const _IRR_OVERRIDE_;
		};


	} // end namespace video
} // end namespace irr

#endif
#endif
