#include "CImageLoaderSVG.h"

#ifdef _IRR_COMPILE_WITH_SVG_LOADER_

#include <rsvg.h>
#include <cairo.h>
#include <boost/scoped_array.hpp>

#include "CImage.h"
#include "CReadFile.h"
#include "os.h"

namespace irr
{
	namespace video
	{

		CImageLoaderSvg::CImageLoaderSvg()
		{
		}

		//! returns true if the file maybe is able to be loaded by this class
		//! based on the file extension (e.g. ".svg")
		bool CImageLoaderSvg::isALoadableFileExtension(const io::path& filename) const
		{
			return core::hasFileExtension ( filename, "svg" );
		}


		//! returns true if the file maybe is able to be loaded by this class
		bool CImageLoaderSvg::isALoadableFileFormat(io::IReadFile* file) const
		{
			return false;
		}

		// load in the image data
		IImage* CImageLoaderSvg::loadImage(io::IReadFile* file) const
		{
			return loadImage(file, 0, 0);
		}

		// load in the image data
		IImage* CImageLoaderSvg::loadImage(io::IReadFile* file, int width, int height) const
		{
			if (!file)
				return 0;

			boost::scoped_array<unsigned char> tmpBuff(new unsigned char[(int)(file->getSize())]);
			if(!file->read(tmpBuff.get(), (u32)file->getSize()))
				os::Printer::log("Could not read file", file->getFileName(), ELL_ERROR);

			GError* pError;
			RsvgHandle* pRsvg = rsvg_handle_new();
			if(!rsvg_handle_write(pRsvg, tmpBuff.get(), file->getSize(), &pError))
				os::Printer::log("Rsvg could not load svg file", file->getFileName(), ELL_ERROR);
			rsvg_handle_close(pRsvg, &pError);//этим вызовом мы не освобождаем хендл, а указываем, что изображение полностью загружено

			RsvgDimensionData Dims;
			rsvg_handle_get_dimensions(pRsvg, &Dims);
			cairo_surface_t *img = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
			cairo_t *cr = cairo_create (img);

			if(width != Dims.width || height != Dims.height){
				cairo_scale(cr, ((double)width)/Dims.width, ((double)height)/Dims.height);
				cairo_save(cr);
			}

			if(!rsvg_handle_render_cairo(pRsvg, cr))
				os::Printer::log("Rsvg could not render svg", file->getFileName(), ELL_ERROR);

			unsigned char* cairo_data = cairo_image_surface_get_data(img);
			video::IImage* image = new CImage(ECF_A8R8G8B8, core::dimension2d<u32>(width, height), cairo_data,
																				false, false);

			cairo_destroy (cr);
			cairo_surface_destroy (img);
			g_object_unref(G_OBJECT(pRsvg));

			return image;
		}

		IImageLoader* createImageLoaderSVG()
		{
			return new CImageLoaderSvg();
		}


	}// end namespace irr
}//end namespace video

//we need this stub for using boost::shared_array with compiler flag -fno-exceptions
#ifdef BOOST_NO_EXCEPTIONS
namespace boost
{
	void throw_exception(std::exception const & e)
	{
	}
}
#endif

#endif
