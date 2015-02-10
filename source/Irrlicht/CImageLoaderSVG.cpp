#include "CImageLoaderSVG.h"

#ifdef _IRR_COMPILE_WITH_SVG_LOADER_

#include <rsvg.h>
#include <cairo.h>
#include <boost/shared_array.hpp>

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

			GError* pError;
			RsvgDimensionData Dims;
			RsvgHandle* pRsvg = rsvg_handle_new();//rsvg_handle_new_from_file(sPath, &pError);
			boost::shared_array<unsigned char> tmpBuff(new unsigned char[(int)(file->getSize())]);
			file->read(tmpBuff.get(), (u32)file->getSize());
			rsvg_handle_write(pRsvg, tmpBuff.get(), file->getSize(), &pError);
			rsvg_handle_get_dimensions(pRsvg, &Dims);

			cairo_surface_t *img = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, Dims.width, Dims.height);
			cairo_t *cr = cairo_create (img);

			rsvg_handle_render_cairo(pRsvg, cr);

			cairo_surface_flush(img);

			cairo_destroy (cr);

			if(!width || !height){
					width = Dims.width;
					height = Dims.height;
			}

			int nStride = cairo_format_stride_for_width (CAIRO_FORMAT_ARGB32, width);
			boost::shared_array<unsigned char> lImageBuffer(new unsigned char[(int)(nStride * height)]);
			memset(lImageBuffer.get(), 0, (int)(nStride * height));

			cairo_surface_t *surface =cairo_image_surface_create_for_data(lImageBuffer.get(),
																																		CAIRO_FORMAT_ARGB32,
																																		width,
																																		height,
																																		nStride);

			cr = cairo_create (surface);

			cairo_matrix_t matr;
			cairo_matrix_init(&matr, -1, 0, 0, -1, width/ 2.0, height / 2.0); //поворачиваем на 180 вокруг центра картинки
			cairo_matrix_translate(&matr, -width/ 2.0, -height / 2.0);

			if(width != Dims.width || height != Dims.height){
				cairo_matrix_scale(&matr, ((double)width)/Dims.width, ((double)height)/Dims.height);
			}

			cairo_transform(cr, &matr);

			cairo_set_source_surface (cr, img, 0, 0);
			cairo_paint (cr);

			cairo_surface_flush(surface);

			cairo_surface_destroy (img);


			rsvg_handle_close(pRsvg, &pError);
			g_object_unref(G_OBJECT(pRsvg));

			cairo_destroy (cr);
			cairo_surface_destroy (surface);

			/*opengl*/
			/*RGBA*/
			/*0123*/
			/*ARGB32 from cairo*/
			/*if little-endian*/
			/*BGRA*/
			/*0123 - cairo*/
			/*2103 - opengl*/
			/*if big-endian*/
			/*ARGB*/
			/*0123 - cairo*/
			/*3003 - opengl*/
			video::IImage* image = new CImage(ECF_A8R8G8B8, core::dimension2d<u32>(width, height));
			unsigned char* data = (unsigned char*)image->lock();

			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					data[i * height * 4 + j*4 + 2] = lImageBuffer[i * height * 4 + j*4 + 0];
					data[i * height * 4 + j*4 + 1] = lImageBuffer[i * height * 4 + j*4 + 1];
					data[i * height * 4 + j*4 + 0] = lImageBuffer[i * height * 4 + j*4 + 2];
					data[i * height * 4 + j*4 + 3] = lImageBuffer[i * height * 4 + j*4 + 3];
				}
			}

			image->unlock();

			return image;
		}

		IImageLoader* createImageLoaderSVG()
		{
			return new CImageLoaderSvg();
		}


	}// end namespace irr
}//end namespace video

#endif
