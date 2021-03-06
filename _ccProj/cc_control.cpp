/////////////////////////////////////////////////////////////////////////////
// Name:        ccSkinFrame.cpp
// Purpose:     Frame without Border and owerr draw background
// Author:      uuware
// Modified by:
// Created:     2016/1/15
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "cc_control.h"
/*
image.GetSubImage:84ms
bitmap.GetSubBitmap:72ms
ccDrawSkin::GetSubImage:79ms
ccDrawSkin::PasteSubImage:27ms
ccDrawSkin::OverlayImage:0ms
wxMemoryDC.DrawBitmap(wxBitmap(image)):1041ms
wxMemoryDC.DrawBitmap(bitmap):153ms
memDC.GetTextExtent:0ms
memDC.DrawText:1ms
wxGCDC.DrawBitmap:12581ms*10
wxGraphicsContext.DrawBitmap(wxImage):5269ms*10
wxGraphicsContext.DrawBitmap(wxGraphicsBitmap):74ms
image.Scale:872ms
wxPaintDC.DrawBitmap(wxBitmap(image)):475ms
wxPaintDC.DrawBitmap(bitmap):0ms
wxPaintDC.DrawBitmap(wxBitmap(image)):473ms
wxPaintDC.DrawBitmap(bitmap):0ms
wxPaintDC.DrawBitmap(wxBitmap(image)):574ms
wxPaintDC.DrawBitmap(bitmap):55ms
*/
void _test_draw()
{
#ifdef _DEBUG
	int CNT = 100;
	wxStopWatch sw;
	wxImage image;
	image.LoadFile(_T("skins\\skin01.png"));

	wxBitmap bitmap(image);

	wxImage image2;
	image2.Create(1024, 1024, true);
	wxBitmap bitmap2(image2);
	bitmap2.UseAlpha();
	wxMemoryDC memDC(bitmap2);

	wxRect rect(0, 0, 400, 700);
	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		wxImage imagex2 = image.GetSubImage(rect);
	}
	wxLogDebug("image.GetSubImage:%ldms", sw.Time());

	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		wxBitmap bitmap2 = bitmap.GetSubBitmap(rect);
	}
	wxLogDebug("bitmap.GetSubBitmap:%ldms", sw.Time());

	sw.Start(0);
	wxImage imagex2;
	for(int n = 0; n < CNT; n++)
	{
		ccDrawSkin::GetSubImage(image, rect, imagex2);
	}
	wxLogDebug("ccDrawSkin::GetSubImage:%ldms", sw.Time());

	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		ccDrawSkin::PasteSubImage(&image2, &image, 0, 0);
	}
	wxLogDebug("ccDrawSkin::PasteSubImage:%ldms", sw.Time());

	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		ccDrawSkin::OverlayImage(&image2, &image, NULL, 0, 0);
	}
	wxLogDebug("ccDrawSkin::OverlayImage:%ldms", sw.Time());

	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		memDC.DrawBitmap(wxBitmap(image), 0, 0, true);
	}
	wxLogDebug("wxMemoryDC.DrawBitmap(wxBitmap(image)):%ldms", sw.Time());

	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		memDC.DrawBitmap(bitmap, 0, 0, true);
	}
	wxLogDebug("wxMemoryDC.DrawBitmap(bitmap):%ldms", sw.Time());

	wxCoord w, h;
    wxString text = wxT("sample title");
	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		memDC.GetTextExtent(text, &w, &h);
	}
	wxLogDebug("memDC.GetTextExtent:%ldms", sw.Time());

	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		memDC.DrawText(text, 0, 0);
	}
	wxLogDebug("memDC.DrawText:%ldms", sw.Time());

	wxGCDC dc( memDC );
	dc.SetBackground( *wxTRANSPARENT_BRUSH );
	dc.Clear();
	sw.Start(0);
	for(int n = 0; n < CNT/10; n++)
	{
		dc.DrawBitmap(image, 0, 0, true);
	}
	wxLogDebug("wxGCDC.DrawBitmap:%ldms*10", sw.Time());

	wxGraphicsContext* m_gcontext = wxGraphicsContext::Create(memDC);
	sw.Start(0);
	for(int n = 0; n < CNT/10; n++)
	{
		m_gcontext->DrawBitmap(image, 0, 0, image.GetWidth(), image.GetHeight());
	}
	wxLogDebug("wxGraphicsContext.DrawBitmap(wxImage):%ldms*10", sw.Time());

	wxGraphicsBitmap gbit = m_gcontext->CreateBitmapFromImage(image);
	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		m_gcontext->DrawBitmap(gbit, 0, 0, image.GetWidth(), image.GetHeight());
	}
	wxLogDebug("wxGraphicsContext.DrawBitmap(wxGraphicsBitmap):%ldms", sw.Time());

	delete m_gcontext;

	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		wxImage image2 = image.Scale(image.GetWidth()*2, image.GetHeight()*1.5);
	}
	wxLogDebug("image.Scale:%ldms", sw.Time());

	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		//wxBitmap bitmap2 = ccDrawSkin::Scale(bitmap, bitmap.GetWidth()*2, bitmap.GetHeight()*1.5);
	}
	wxLogDebug("ccDrawSkin::Scale:%ldms", sw.Time());
#endif
}
void _test_draw(wxPaintDC& paintDC)
{
	int CNT = 100;
	wxStopWatch sw;
	wxImage image;
	image.LoadFile(_T("skins\\skin01.png"));
	wxBitmap bitmap(image);

	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		paintDC.DrawBitmap(wxBitmap(image), 0, 0, true);
	}
	wxLogDebug("wxPaintDC.DrawBitmap(wxBitmap(image)):%ldms", sw.Time());

	sw.Start(0);
	for(int n = 0; n < CNT; n++)
	{
		paintDC.DrawBitmap(bitmap, 0, 0, true);
	}
	wxLogDebug("wxPaintDC.DrawBitmap(bitmap):%ldms", sw.Time());
}

wxBitmap ccDrawSkin::Scale(wxBitmap& bitmapOrg, int width, int height)
{
	wxBitmap bitmap;
	wxCHECK_MSG( bitmapOrg.Ok(), bitmap, wxT("invalid bitmap") );
	//can't scale to/from 0 size
	wxCHECK_MSG( (width > 0) && (height > 0), bitmap, wxT("invalid new bitmap size") );

	long old_height = bitmapOrg.GetHeight(), old_width = bitmapOrg.GetWidth();
	wxCHECK_MSG( (old_height > 0) && (old_width > 0), bitmap, wxT("invalid old bitmap size") );
	bitmap.Create( width, height, bitmapOrg.GetDepth() );

	long x_delta = (old_width<<16) / width;
	long y_delta = (old_height<<16) / height;
	bitmap.SetDepth(32);
	bitmapOrg.SetDepth(32);


	wxAlphaPixelData src_data(bitmapOrg);
	wxAlphaPixelData::Iterator src_it(src_data);
	wxAlphaPixelData dst_data(bitmap);
	wxAlphaPixelData::Iterator dst_it(dst_data);
	long y2 = 0;
    for (int y = 0; y < height; y++)
	{
		long x2 = 0;
        for (int x = 0; x < width; x++)
        {
			src_it.MoveTo(src_data, x2, y2);
			dst_it.MoveTo(dst_data, x, y);
			dst_it.Red() = src_it.Red();
			dst_it.Green() = src_it.Green();
			dst_it.Blue() = src_it.Blue();
			dst_it.Alpha() = src_it.Alpha();

			x += x_delta;
		}
		y += y_delta;
	}
	return bitmap;
#if 0
	wxNativePixelData target_data(bitmap);
	wxCHECK_MSG( target_data, bitmap, wxT("unable to create bitmap") );
	wxNativePixelData source_data(bitmapOrg);
	wxNativePixelData::Iterator dest_pixel(target_data);
	wxNativePixelData::Iterator src_pixel(source_data);
	//this may not be needed
	dest_pixel.Offset(target_data, 0, 0);

	long y = 0;
	for ( long j = 0; j < height; j++ )
	{
		wxNativePixelData::Iterator rowStart = dest_pixel;
		long x = 0;
		for ( long i = 0; i < width; i++, ++dest_pixel )
		{
			src_pixel.Offset(source_data, x>>16, y>>16);
			dest_pixel.Red() = src_pixel.Red();
			dest_pixel.Green() = src_pixel.Green();
			dest_pixel.Blue() = src_pixel.Blue();
			dest_pixel.Alpha() = src_pixel.Alpha();
			src_pixel.Offset(source_data, 0 - (x>>16), 0 - (y>>16));
			x += x_delta;
		}
		dest_pixel = rowStart;
		dest_pixel.OffsetY(target_data, 1);
		y += y_delta;
	}
	return bitmap;
#endif
}

///This will explicitly shift the image color from
///srcColour to dstColour.
bool ccDrawSkin::ChangeImageColour(wxImage& srcImage, wxColour & srcColour, wxColour & dstColour)
{
	// This function takes a source image, which it assumes to
	// be grayscale, and smoothly changes the overall color
	// to the specified color, and returns the result as a
	// new image.

	unsigned char *src = srcImage.GetData();
	int width = srcImage.GetWidth();
	int height = srcImage.GetHeight();

	//Get the source color
	int srcVal[3], srcOpp[3];
	srcVal[0] = srcColour.Red();
	srcVal[1] = srcColour.Green();
	srcVal[2] = srcColour.Blue();

	int dstVal[3], dstOpp[3];
	dstVal[0] = dstColour.Red();
	dstVal[1] = dstColour.Green();
	dstVal[2] = dstColour.Blue();

	int i;
	for (i = 0; i < 3; i++) {
		srcOpp[i] = 256 - srcVal[i];  // avoid zero!
		dstOpp[i] = 255 - dstVal[i];
	}

	int c = 0;
	for (i = 0; i < width * height * 3; i++) {
		int s = (int)*src;

		if (s >= srcVal[c])
			 *src = dstVal[c] + dstOpp[c] * (s - srcVal[c]) / srcOpp[c];
		else
			*src = dstVal[c] * s / srcVal[c];
		src++;
		c = (c + 1) % 3;
	}

	return true;
}

/// Takes a background image, foreground image, and mask
/// (i.e. the alpha channel for the foreground), and
/// returns an new image where the foreground has been
/// overlaid onto the background using alpha-blending,
/// at location (xoff, yoff).
bool ccDrawSkin::OverlayImage(wxImage * background, wxImage * foreground,
                      wxImage * mask, int xoff, int yoff)
{
	unsigned char *bg = background->GetData();
	unsigned char *fg = foreground->GetData();
	unsigned char *mk = NULL;

	int bgWidth = background->GetWidth();
	int bgHeight = background->GetHeight();
	int fgWidth = foreground->GetWidth();
	int fgHeight = foreground->GetHeight();
	int mkWidth = 0;
	int mkHeight = 0;
	if(mask != NULL)
	{
		mk = mask->GetData();
		mkWidth = mask->GetWidth();
		mkHeight = mask->GetHeight();
	}

	//Now, determine the dimensions of the images to be masked together
	//on top of the background.  This should be equal to the area of the
	//smaller of the foreground and the mask, as long as it is
	//within the area of the background, given the offset.

	//Make sure the foreground size is no bigger than the mask
	int wCutoff = (fgWidth < mkWidth) ? fgWidth : mkWidth;
	int hCutoff = (fgHeight < mkHeight) ? fgHeight : mkHeight;

	// If the masked foreground + offset is bigger than the background, masking
	// should only occur within these bounds of the foreground image
	wCutoff = (bgWidth - xoff > wCutoff) ? wCutoff : bgWidth - xoff;
	hCutoff = (bgHeight - yoff > hCutoff) ? hCutoff : bgHeight - yoff;

	// Go through the foreground image bit by bit and mask it on to the
	// background, at an offset of xoff,yoff.
	// BUT...Don't go beyond the size of the background image,
	// the foreground image, or the mask
	int x, y;
	for (y = 0; y < hCutoff; y++) {
		unsigned char *bkp = bg + 3 * ((y + yoff) * bgWidth + xoff);

		for (x = 0; x < wCutoff; x++) {
			int value = 0;
			if(mk != NULL)
			{
				value = mk[3 * (y * mkWidth + x)];
			}
			int opp = 255 - value;

			for (int c = 0; c < 3; c++)
				bkp[x * 3 + c] = ((bkp[x * 3 + c] * opp) + (fg[3 * (y * fgWidth + x) + c] * value)) / 255;
		}
	}
	return true;
}

/// Pastes one image into another including the alpha channel.
/// Differs from OverlayImage in that:
///   Happens in place to existing background image.
///   Pastes image on top; no blending with existing background is done.
void ccDrawSkin::PasteSubImage( wxImage * background, wxImage * foreground, int xoff, int yoff )
{
	unsigned char *bg = background->GetData();
	unsigned char *fg = foreground->GetData();
	unsigned char *bgAlpha = background->HasAlpha() ? background->GetAlpha() : NULL;
	unsigned char *fgAlpha = foreground->HasAlpha() ? foreground->GetAlpha() : NULL;
	// For testing... Set as if no alpha in foreground....
	// fgAlpha = NULL;

	int bgWidth = background->GetWidth();
	int bgHeight = background->GetHeight();
	int fgWidth = foreground->GetWidth();
	int fgHeight = foreground->GetHeight();

	int wCutoff = fgWidth;
	int hCutoff = fgHeight;

	// If the masked foreground + offset is bigger than the background, masking
	// should only occur within these bounds of the foreground image
	wCutoff = (bgWidth - xoff > wCutoff) ? wCutoff : bgWidth - xoff;
	hCutoff = (bgHeight - yoff > hCutoff) ? hCutoff : bgHeight - yoff;

	// Go through the foreground image bit by bit and place it on to the
	// background, at an offset of xoff,yoff.
	// Don't go beyond the size of the background image,
	// or the foreground image.
	int y;
	unsigned char *bkp;
	unsigned char *fgp;
	unsigned char *bgAlphap;
	unsigned char *fgAlphap;
	for (y = 0; y < hCutoff; y++) {
		// RGB bytes
		bkp = bg + 3 * ((y + yoff) * bgWidth + xoff);
		fgp = fg + 3 * ( y * fgWidth);
		memcpy( bkp, fgp, 3 * wCutoff );

		// Alpha bytes.
		if( bgAlpha )
		{
			bgAlphap = bgAlpha + ((y+yoff) * bgWidth + xoff );
			if( fgAlpha )
			{
				fgAlphap = fgAlpha + (y * fgWidth );
				memcpy( bgAlphap, fgAlphap, wCutoff );
			}
			else
			{
				memset( bgAlphap, 255, wCutoff );
			}
		}
	}
}

void ccDrawSkin::PasteSubImage( wxImage& background, wxImage& foreground, int xoff, int yoff, int toW, int toH)
{
	unsigned char *bg = background.GetData();
	unsigned char *fg = foreground.GetData();
	unsigned char *bgAlpha = background.HasAlpha() ? background.GetAlpha() : NULL;
	unsigned char *fgAlpha = foreground.HasAlpha() ? foreground.GetAlpha() : NULL;

	int bgWidth = background.GetWidth();
	int bgHeight = background.GetHeight();
	int fgWidth = foreground.GetWidth();
	int fgHeight = foreground.GetHeight();

	int wCutoff = fgWidth;
	int hCutoff = fgHeight;

	// If the masked foreground + offset is bigger than the background, masking
	// should only occur within these bounds of the foreground image
	wCutoff = (bgWidth - xoff > wCutoff) ? wCutoff : bgWidth - xoff;
	hCutoff = (bgHeight - yoff > hCutoff) ? hCutoff : bgHeight - yoff;
	if(toW <= 0)
	{
		toW = fgWidth;
	}
	if(toH <= 0)
	{
		toH = fgHeight;
	}

	// Go through the foreground image bit by bit and place it on to the
	// background, at an offset of xoff,yoff.
	// Don't go beyond the size of the background image,
	// or the foreground image.
	int y;
	unsigned char *bkp;
	unsigned char *fgp;
	unsigned char *bgAlphap;
	unsigned char *fgAlphap;
	for (y = 0; y < hCutoff; y++) {
		// RGB bytes
		bkp = bg + 3 * ((y + yoff) * bgWidth + xoff);
		fgp = fg + 3 * ( y * fgWidth);
		memcpy( bkp, fgp, 3 * wCutoff );

		// Alpha bytes.
		if( bgAlpha )
		{
			bgAlphap = bgAlpha + ((y+yoff) * bgWidth + xoff );
			if( fgAlpha )
			{
				fgAlphap = fgAlpha + (y * fgWidth );
				memcpy( bgAlphap, fgAlphap, wCutoff );
			}
			else
			{
				memset( bgAlphap, 255, wCutoff );
			}
		}
	}
}

/// Gets a rectangle from within another image, INCLUDING the alpha channel
/// \bug in wxWidgets, wxImage::GetSubImage should do this itself.
bool ccDrawSkin::GetSubImage( const wxImage & Src, const wxRect &rect, wxImage & Dst )
{
	//First part of this code is lifted from wxImage::GetSubImage() source code.
	wxCHECK_MSG( Src.Ok(), false, wxT("invalid image") );

	wxCHECK_MSG( (rect.GetLeft()>=0) && (rect.GetTop()>=0) && (
		rect.GetRight()<=Src.GetWidth()) && (rect.GetBottom()<=Src.GetHeight()),
		false, wxT("invalid subimage size") );

	int subwidth = rect.GetWidth();
	const int subheight = rect.GetHeight();

	Dst.Create( subwidth, subheight, false );

	unsigned char *subdata = Dst.GetData(), *data = Src.GetData();

	wxCHECK_MSG( subdata, false, wxT("unable to create image") );

	// JKC: Quick hack - don't deal with masks - need to understand macro M_IMGDATA first.
	// if (Src.M_IMGDATA->m_hasMask)
	// image.SetMaskColour( Src.M_IMGDATA->m_maskRed, Src.M_IMGDATA->m_maskGreen, Src.M_IMGDATA->m_maskBlue );

	int subleft = 3*rect.GetLeft();
	int width = 3*Src.GetWidth();
	subwidth *= 3;

	data += rect.GetTop() * width + subleft;

	for (long j = 0; j < subheight; ++j)
	{
		memcpy( subdata, data, subwidth);
		subdata += subwidth;
		data += width;
	}

   // OK, so we've copied the RGB data.
   // Now do the Alpha channel.
   if(Src.HasAlpha())
   {
		Dst.InitAlpha();

		subleft /= 3;
		width /= 3;
		subwidth /= 3;

		data = Src.GetAlpha();
		subdata = Dst.GetAlpha();

		data += rect.GetTop()*width+subleft;

		for (long j = 0; j < subheight; ++j)
		{
			memcpy( subdata, data, subwidth);
			subdata += subwidth;
			data += width;
		}
   }
   return true;
}

bool ccDrawSkin::SkinRegion(wxRegion& region, const wxImage& image, int tolerance)
{
    // Loop through the image row by row, pixel by pixel, building up
    // rectangles to add to the region.
    int width = image.GetWidth();
    int height = image.GetHeight();
    for (int y = 0; y < height; y++)
    {
        wxRect rect;
        rect.y = y;
        rect.height = 1;

        for (int x=0; x < width; x++)
        {
            // search for a continuous range of non-transparent pixels
            int x0 = x;
            while ( x < width)
            {
				unsigned char A = image.GetAlpha(x,y);
                if (( A <= tolerance))  // It's transparent
                    break;
                x++;
            }

            // Add the run of non-transparent pixels (if any) to the region
            if (x > x0) {
                rect.x = x0;
                rect.width = x - x0;
                region.Union(rect);
            }
        }
    }

    return true;
}

bool ccDrawSkin::SkinRegion(wxRegion& region, wxBitmap& bitmap, int tolerance)
{
    // Loop through the image row by row, pixel by pixel, building up
    // rectangles to add to the region.
	if(!bitmap.HasAlpha())
	{
		return false;
	}

	wxAlphaPixelData bmdata(bitmap);
	wxAlphaPixelData::Iterator dst(bmdata);
    int width = bitmap.GetWidth();
    int height = bitmap.GetHeight();
    for (int y = 0; y < height; y++)
	{
        wxRect rect;
        rect.y = y;
        rect.height = 1;
        for (int x = 0; x < width; x++)
        {
            // search for a continuous range of non-transparent pixels
            int x0 = x;
            while ( x < width)
            {
				dst.MoveTo(bmdata, x, y);
                if (( dst.Alpha() <= tolerance))  // It's transparent
                    break;
                x++;
            }

            // Add the run of non-transparent pixels (if any) to the region
            if (x > x0) {
                rect.x = x0;
                rect.width = x - x0;
                region.Union(rect);
            }
        }
	}

    return true;
}

ccDrawSkin::ccDrawSkin()
{
	m_nTop = 0;
	m_nBottom = 0;
	m_nLeftRight = 0;
	m_isLoaded = false;
	m_backColour = wxColour(100, 100, 100);
	m_transparentColor = wxNullColour;
}

ccDrawSkin::~ccDrawSkin()
{
	for( ccPointMap::iterator it = m_mapImg.begin(); it != m_mapImg.end(); ++it )
	{
		//wxString  path = it->first;
		wxImage* pImage = (wxImage*)it->second;
		delete pImage;
	}
}
/*
bool ccDrawSkin::FillImage(wxMemoryDC& dc, const wxBitmap& bitmap, int nStartW, int nStepW, int nEndW, int nStartH, int nStepH, int nEndH)
{
	int nlocEndW = nEndW + nStepW;
	int nlocEndH = nEndH + nStepH;
	while(nStartH < nlocEndH)
	{
		int nlocStartW = nStartW;
		while(nlocStartW < nlocEndW)
		{
			dc.DrawBitmap( bitmap, nlocStartW, nStartH, true );
			nlocStartW += nStepW;
		}
		nStartH += nStepH;
	}
	return true;
}*/

bool ccDrawSkin::LoadImg(const wxString& sImgPath, const wxString& sRect, wxImage& Dst)
{
	wxImage* pImage = (wxImage*)m_mapImg[sImgPath];
    if (pImage == NULL)
	{
		if(!wxFileExists(sImgPath))
		{
			return false;
		}
		pImage = new wxImage();
		pImage->LoadFile(sImgPath);
		m_mapImg[sImgPath] = (void*)pImage;
		//wxLogDebug("Image Alpha:%d, Mask:%d", (int)pImage->GetAlpha(), (int)pImage->HasMask());

		//transparent?
		if(m_transparentColor.IsOk() && pImage->IsOk() && pImage->HasMask())
		{
			if(pImage->GetMaskRed() != m_transparentColor.Red() || pImage->GetMaskGreen() != m_transparentColor.Green()
				|| pImage->GetMaskBlue() != m_transparentColor.Blue())
			{
				pImage->Replace(pImage->GetMaskRed(), pImage->GetMaskGreen(), pImage->GetMaskBlue(),
					m_transparentColor.Red(), m_transparentColor.Green(), m_transparentColor.Blue());
				pImage->SetMaskColour(m_transparentColor.Red(), m_transparentColor.Green(), m_transparentColor.Blue());
			}
		}
	}
	if(!pImage->IsOk())
	{
		return false;
	}

	//int imgh = image.GetHeight();
	//int imgw = image.GetWidth();
	//if(sRect.x == 0 && sRect.y == 0 && sRect.width == imgw && sRect.height == imgh)
	wxRect rect = ccUtil::RectFromString(sRect);
	Dst = pImage->GetSubImage(rect);
	if(!Dst.IsOk())
	{
		wxLogDebug("Image NG, path:%s, rect:%s", sImgPath, sRect);
	}
    wxCHECK_MSG( Dst.IsOk(), false, wxT("invalid image") );
	return true;
}

bool ccDrawSkin::LoadSkin(const wxString& sXmlPath)
{
	// start processing the XML file
	m_isLoaded = false;
	xml.Clear();
	for( ccPointMap::iterator it = m_mapImg.begin(); it != m_mapImg.end(); ++it )
	{
		//wxString  path = it->first;
		wxImage* pImage = (wxImage*)it->second;
		delete pImage;
	}
	m_mapImg.clear();

	if(!xml.Load(sXmlPath))
	{
		return false;
	}
	if(xml.GetSize(wxT("/skin/frame")) != 1 || xml.GetSize(wxT("/skin/button")) != 1)
	{
		return false;
	}

	m_xmlBasePath = ccUtil::GetAbsolutePathOnly(sXmlPath);
	wxString skinImg = xml.GetAttribute(wxT("/skin"), wxT("image"));
	if(!skinImg.IsEmpty())
	{
		skinImg = ccUtil::GetComparePath(m_xmlBasePath, skinImg);
	}
	wxString frameImg = xml.GetAttribute(wxT("/skin/frame"), wxT("image"));
	if(frameImg.IsEmpty())
	{
		frameImg = skinImg;
	}
	else
	{
		frameImg = ccUtil::GetComparePath(m_xmlBasePath, frameImg);
	}

	wxString sValue;
	//m_frameRadius = xml.GetValue(wxT("/skin/frame/radius"), 10);
	//m_frameIconsize = xml.GetValue(wxT("/skin/frame/iconsize"), 16);
	m_frameIconLeft = GetValue(wxT("/skin/frame/iconleft"), -1);
	m_frameIconTop = GetValue(wxT("/skin/frame/icontop"), -1);
	m_frameTitleTop = GetValue(wxT("/skin/frame/titletop"), -1);

	sValue = xml.GetValue(wxT("/skin/frame/transparentcolor"));
	if(!sValue.IsEmpty())
	{
		m_transparentColor.Set(sValue);
	}
	sValue = xml.GetValue(wxT("/skin/frame/backgroundcolor"));
	if(!sValue.IsEmpty())
	{
		m_backColour.Set(sValue);
	}
	sValue = xml.GetValue(wxT("/skin/frame/titlecolor"));
	if(!sValue.IsEmpty())
	{
		m_titleColor.Set(sValue);
	}
	else
	{
		m_titleColor.Set(wxT("#000000"));
	}

	sValue = xml.GetValue(wxT("/skin/frame/topleft"));
	LoadImg(frameImg, sValue, m_imgTopLeft);
	if(!m_transparentColor.IsOk() && m_imgTopLeft.IsOk() && m_imgTopLeft.HasMask())
	{
		//from first image?
		m_transparentColor.Set(m_imgTopLeft.GetMaskRed(), m_imgTopLeft.GetMaskGreen(), m_imgTopLeft.GetMaskBlue());
	}

	sValue = xml.GetValue(wxT("/skin/frame/topcenter"));
	LoadImg(frameImg, sValue, m_imgTopCenter);
	sValue = xml.GetValue(wxT("/skin/frame/topright"));
	LoadImg(frameImg, sValue, m_imgTopRight);

	//for title's back and title's right
	m_imgTopTitleLeft= wxNullImage;
	m_imgTopTitle= wxNullImage;
	m_imgTopTitleRight= wxNullImage;
	sValue = xml.GetValue(wxT("/skin/frame/toptitleleft"));
	if(!sValue.IsEmpty())
	{
		LoadImg(frameImg, sValue, m_imgTopTitleLeft);
		sValue = xml.GetValue(wxT("/skin/frame/toptitle"));
		if(!sValue.IsEmpty())
		{
			LoadImg(frameImg, sValue, m_imgTopTitle);
			sValue = xml.GetValue(wxT("/skin/frame/toptitleright"));
			if(!sValue.IsEmpty())
			{
				LoadImg(frameImg, sValue, m_imgTopTitleRight);
			}
		}
	}

	sValue = xml.GetValue(wxT("/skin/frame/bottomleft"));
	LoadImg(frameImg, sValue, m_imgBottomLeft);
	sValue = xml.GetValue(wxT("/skin/frame/bottomcenter"));
	LoadImg(frameImg, sValue, m_imgBottomCenter);
	sValue = xml.GetValue(wxT("/skin/frame/bottomright"));
	LoadImg(frameImg, sValue, m_imgBottomRight);
	sValue = xml.GetValue(wxT("/skin/frame/leftside"));
	LoadImg(frameImg, sValue, m_imgLeft);
	sValue = xml.GetValue(wxT("/skin/frame/rightside"));
	LoadImg(frameImg, sValue, m_imgRight);

	wxString strRect;
	wxString strTmp;
	wxString btnImg = xml.GetAttribute(wxT("/skin/button"), wxT("image"));
	if(btnImg.IsEmpty())
	{
		btnImg = skinImg;
	}
	else
	{
		btnImg = ccUtil::GetComparePath(m_xmlBasePath, btnImg);
	}

	int nInd;
	nInd = _CC_ccDrawSkin_BTN_NORMAL_;
	strTmp = xml.GetAttribute(wxT("/skin/button/normal"), wxT("image"));
	if(strTmp.IsEmpty())
	{
		strTmp = btnImg;
	}
	else
	{
		strTmp = ccUtil::GetComparePath(m_xmlBasePath, strTmp);
	}
	strRect = xml.GetValue(wxT("/skin/button/normal/left"));
	LoadImg(strTmp, strRect, m_ccSkinButtonLeft[nInd]);
	strRect = xml.GetValue(wxT("/skin/button/normal/center"));
	LoadImg(strTmp, strRect, m_ccSkinButtonCenter[nInd]);
	strRect = xml.GetValue(wxT("/skin/button/normal/right"));
	LoadImg(strTmp, strRect, m_ccSkinButtonRight[nInd]);

	nInd = _CC_ccDrawSkin_BTN_MOUSEON_;
	strTmp = xml.GetAttribute(wxT("/skin/button/mouseon"), wxT("image"));
	if(strTmp.IsEmpty())
	{
		strTmp = btnImg;
	}
	else
	{
		strTmp = ccUtil::GetComparePath(m_xmlBasePath, strTmp);
	}
	strRect = xml.GetValue(wxT("/skin/button/mouseon/left"));
	LoadImg(strTmp, strRect, m_ccSkinButtonLeft[nInd]);
	strRect = xml.GetValue(wxT("/skin/button/mouseon/center"));
	LoadImg(strTmp, strRect, m_ccSkinButtonCenter[nInd]);
	strRect = xml.GetValue(wxT("/skin/button/mouseon/right"));
	LoadImg(strTmp, strRect, m_ccSkinButtonRight[nInd]);

	nInd = _CC_ccDrawSkin_BTN_PRESSED_;
	strTmp = xml.GetAttribute(wxT("/skin/button/pressed"), wxT("image"));
	if(strTmp.IsEmpty())
	{
		strTmp = btnImg;
	}
	else
	{
		strTmp = ccUtil::GetComparePath(m_xmlBasePath, strTmp);
	}
	strRect = xml.GetValue(wxT("/skin/button/pressed/left"));
	LoadImg(strTmp, strRect, m_ccSkinButtonLeft[nInd]);
	strRect = xml.GetValue(wxT("/skin/button/pressed/center"));
	LoadImg(strTmp, strRect, m_ccSkinButtonCenter[nInd]);
	strRect = xml.GetValue(wxT("/skin/button/pressed/right"));
	LoadImg(strTmp, strRect, m_ccSkinButtonRight[nInd]);

	nInd = _CC_ccDrawSkin_BTN_DISABLED_;
	strTmp = xml.GetAttribute(wxT("/skin/button/disabled"), wxT("image"));
	if(strTmp.IsEmpty())
	{
		strTmp = btnImg;
	}
	else
	{
		strTmp = ccUtil::GetComparePath(m_xmlBasePath, strTmp);
	}
	strRect = xml.GetValue(wxT("/skin/button/disabled/left"));
	if(strRect.IsEmpty())
	{
		m_ccSkinButtonLeft[nInd] = m_ccSkinButtonRight[_CC_ccDrawSkin_BTN_NORMAL_].ConvertToDisabled(_CC_ccDrawSkin_CONVERT_DISABLED_);
	}
	else
	{
		LoadImg(strTmp, strRect, m_ccSkinButtonLeft[nInd]);
	}
	strRect = xml.GetValue(wxT("/skin/button/disabled/center"));
	if(strRect.IsEmpty())
	{
		m_ccSkinButtonCenter[nInd] = m_ccSkinButtonRight[_CC_ccDrawSkin_BTN_NORMAL_].ConvertToDisabled(_CC_ccDrawSkin_CONVERT_DISABLED_);
	}
	else
	{
		LoadImg(strTmp, strRect, m_ccSkinButtonCenter[nInd]);
	}
	strRect = xml.GetValue(wxT("/skin/button/disabled/right"));
	if(strRect.IsEmpty())
	{
		m_ccSkinButtonRight[nInd] = m_ccSkinButtonRight[_CC_ccDrawSkin_BTN_NORMAL_].ConvertToDisabled(_CC_ccDrawSkin_CONVERT_DISABLED_);
	}
	else
	{
		LoadImg(strTmp, strRect, m_ccSkinButtonRight[nInd]);
	}

	m_isLoaded = true;
	return true;
}

wxString ccDrawSkin::GetValue(const wxString& sXmlPath)
{
	return xml.GetValue(sXmlPath);
}

int ccDrawSkin::GetValue(const wxString& sXmlPath, int notFound)
{
	return xml.GetValue(sXmlPath, notFound);
}

bool ccDrawSkin::DrawText(wxMemoryDC& memDC, const wxString& textOrig, const wxRect& rect, int alignment)
{
    // we don't support displaying multiple lines currently (and neither does wxMSW FWIW) so just merge all the lines
    wxString text = textOrig;
	text.Replace(wxT("\r\n"), wxT(" "));
	text.Replace(wxT("\n"), wxT(" "));

    wxCoord w, h;
    memDC.GetTextExtent(text, &w, &h);

	int nTop;
	if((alignment & wxALIGN_CENTER_VERTICAL) == wxALIGN_CENTER_VERTICAL)
	{
		nTop = rect.y + (rect.height - h - 1)/2;
	}
	else if((alignment & wxALIGN_BOTTOM) == wxALIGN_BOTTOM)
	{
		nTop = rect.y + rect.height - h;
	}
	else //if((alignment & wxALIGN_TOP) == wxALIGN_TOP)
	{
		nTop = rect.y;
	}
    // determine if the string can fit inside the current width
	if (w <= rect.width)
    {
        // it can, draw it using the items alignment
		int nLeft;
		if((alignment & wxALIGN_CENTER_HORIZONTAL) == wxALIGN_CENTER_HORIZONTAL)
		{
			nLeft = rect.x + (rect.width - w - 1)/2;
		}
		else if((alignment & wxALIGN_RIGHT) == wxALIGN_RIGHT)
		{
			nLeft = rect.x + rect.width - w;
		}
		else //if((alignment & wxALIGN_LEFT) == wxALIGN_LEFT)
		{
			nLeft = rect.x;
		}

        memDC.DrawText(text, nLeft, nTop);
    }
    else // otherwise, truncate and add an ellipsis if possible
    {
        // determine the base width
        wxString ellipsis(wxT("..."));
        wxCoord base_w;
        memDC.GetTextExtent(ellipsis, &base_w, &h);

        // continue until we have enough space or only one character left
        wxCoord w_c, h_c;
        size_t len = text.length();
        wxString drawntext = text.Left(len);
        while (len > 1)
        {
            memDC.GetTextExtent(drawntext.Last(), &w_c, &h_c);
            drawntext.RemoveLast();
            len--;
            w -= w_c;
			if (w + base_w <= rect.width)
			{
                break;
			}
        }

        // if still not enough space, remove ellipsis characters
        while (ellipsis.length() > 0 && w + base_w > rect.width)
        {
            ellipsis = ellipsis.Left(ellipsis.length() - 1);
            memDC.GetTextExtent(ellipsis, &base_w, &h);
        }

        // now draw the text
        memDC.DrawText(drawntext, rect.x, nTop);
        memDC.DrawText(ellipsis, rect.x + w, nTop);
    }
	return true;
}

bool ccDrawSkin::DrawFrame(ccSkinFrame* ccSkinFrame, wxBitmap& bitmap, const wxBitmap& titleIcon)
{
    wxCHECK_MSG( m_isLoaded, false, wxT("not loaded") );

	int imgh = bitmap.GetHeight();
	int imgw = bitmap.GetWidth();
	int nTopH = m_imgTopCenter.GetHeight();
	int nBottomH = m_imgBottomCenter.GetHeight();
	if(!m_imgTopLeft.IsOk() || !m_imgTopCenter.IsOk() || !m_imgTopRight.IsOk() || !m_imgLeft.IsOk() || !m_imgRight.IsOk()
		|| !m_imgBottomLeft.IsOk() || !m_imgBottomCenter.IsOk() || !m_imgBottomRight.IsOk())
	{
		return false;
	}

	//wxLogDebug("top height, left:%d, center:%d, right:%d", m_imgTopLeft.GetHeight(), nTopH, m_imgTopRight.GetHeight());
	//wxLogDebug("bottom height, left:%d, center:%d, right:%d", m_imgBottomLeft.GetHeight(), nBottomH, m_imgBottomRight.GetHeight());
	//top side(should m_imgTopLeft and others have same height!)
	//bottom side(should m_imgBottomLeft,m_imgBottomCenter,m_imgBottomRight have same height!)
	wxCHECK_MSG( (m_imgTopLeft.GetHeight() == nTopH && m_imgTopRight.GetHeight() == nTopH), false, wxT("skin top height not valid") );
	wxCHECK_MSG( (m_imgBottomLeft.GetHeight() == nBottomH && m_imgBottomRight.GetHeight() == nBottomH), false, wxT("skin bottom height not valid") );

	wxMemoryDC dc(bitmap);
	//wxGCDC dc( memDC );
	//dc.SetBackground( *wxTRANSPARENT_BRUSH );
	//dc.Clear();

	if(m_transparentColor.IsOk())
	{
		dc.SetPen(m_transparentColor);
		dc.SetBrush(m_transparentColor);
		dc.DrawRectangle(wxRect(0, 0, imgw, imgh));
	}

	//dc.SetPen(*wxTRANSPARENT_PEN);
	//dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetFont(ccSkinFrame->GetFont());
	//memDC.DrawRoundedRectangle(0, 0, imgw, imgh, m_frameRadius);

	//left side
	//right side
	if(imgh - nTopH - nBottomH > 0)
	{
		wxImage tmpLeft = m_imgLeft.Scale(m_imgLeft.GetWidth(), imgh - nTopH - nBottomH);
		wxImage tmpRight = m_imgRight.Scale(m_imgRight.GetWidth(), imgh - nTopH - nBottomH);
		dc.DrawBitmap(tmpLeft, 0, nTopH, true);
		dc.DrawBitmap(tmpRight, imgw - m_imgRight.GetWidth(), nTopH, true);
	}

	//drow top like:topleft---title---title right---center---topright
	//           or:topleft---center---topright
	wxString sTitle = ccSkinFrame->GetTitle();
	wxCoord textw;
	wxCoord texth;
	dc.GetTextExtent(sTitle, &textw, &texth);
	int iconSplit = 0;
	int iconSize =  0;
	if(titleIcon.IsOk())
	{
		iconSplit = 3;
		iconSize = titleIcon.GetWidth();
	}
	int iconLeft = m_frameIconLeft;
	int iconTop = m_frameIconTop;
	int titleTop = m_frameTitleTop;
	if(iconTop < 0)
	{
		iconTop = (nTopH - iconSize)/2;
	}
	if(titleTop < 0)
	{
		titleTop = (nTopH - texth)/2;
	}
	if(iconLeft < 0)
	{
		iconLeft = m_imgTopLeft.GetWidth();
		if(m_imgTopTitleLeft.IsOk())
		{
			iconLeft += m_imgTopTitleLeft.GetWidth();
		}
	}
	int titleW = textw + iconSize + iconSplit;
	//int nMiniW = iconLeft + m_imgTopRight.GetWidth();
	int nNeedTitleW = 0;
	if(m_imgTopTitleLeft.IsOk() && m_imgTopTitle.IsOk() && m_imgTopTitleRight.IsOk())
	{
		nNeedTitleW = m_imgTopTitleLeft.GetWidth() + m_imgTopTitleRight.GetWidth();
	}
	int nStartLeft = 0;
	if(imgw > m_imgTopRight.GetWidth())
	{
		//topleft
		dc.DrawBitmap(m_imgTopLeft, nStartLeft, 0, true);
		nStartLeft += m_imgTopLeft.GetWidth();
	}
	if(nNeedTitleW > 0 && ((
		iconLeft >= m_imgTopLeft.GetWidth() + m_imgTopTitleLeft.GetWidth()
		&& iconLeft + 16 < imgw - m_imgTopTitleRight.GetWidth() - m_imgTopRight.GetWidth()
		) ||
		(iconLeft < m_imgTopLeft.GetWidth() + m_imgTopTitleLeft.GetWidth()
		&& imgw - 16 - m_imgTopLeft.GetWidth() - m_imgTopTitleLeft.GetWidth() - m_imgTopTitleRight.GetWidth() - m_imgTopRight.GetWidth() > 0))
		)
	{
		//title left
		dc.DrawBitmap(m_imgTopTitleLeft, nStartLeft, 0, true);
		nStartLeft += m_imgTopTitleLeft.GetWidth();

		//title
		if(iconLeft + titleW > imgw - m_imgTopTitleRight.GetWidth() - m_imgTopRight.GetWidth())
		{
			titleW = imgw - iconLeft - m_imgTopTitleRight.GetWidth() - m_imgTopRight.GetWidth();
		}
		int titleImgW = titleW + (iconLeft - m_imgTopLeft.GetWidth() - m_imgTopTitleLeft.GetWidth());
		wxImage tmpTopCenter = m_imgTopTitle.Scale(titleImgW, nTopH);
		dc.DrawBitmap(tmpTopCenter, nStartLeft, 0, true);
		nStartLeft += titleImgW;

		//title right
		dc.DrawBitmap(m_imgTopTitleRight, nStartLeft, 0, true);
		nStartLeft += m_imgTopTitleRight.GetWidth();
	}
	else
	{
		//title
		if(iconLeft + titleW > imgw - m_imgTopRight.GetWidth())
		{
			titleW = imgw - iconLeft - m_imgTopRight.GetWidth();
		}
	}

	//top center
	if(nStartLeft < imgw - m_imgTopRight.GetWidth())
	{
		wxImage tmpTopCenter = m_imgTopCenter.Scale(imgw - nStartLeft - m_imgTopRight.GetWidth(), nTopH);
		dc.DrawBitmap(tmpTopCenter, nStartLeft, 0, true);
	}

	//topright
	dc.DrawBitmap(m_imgTopRight, imgw - m_imgTopRight.GetWidth(), 0, true);
	if(titleW >= 16)
	{
		if(iconSize > 0)
		{
			dc.DrawBitmap(titleIcon, iconLeft, iconTop);
		}
		if(titleW - iconSize - iconSplit > 5)
		{
			dc.SetTextForeground(m_titleColor);
			this->DrawText(dc, sTitle, wxRect(iconLeft + iconSize + iconSplit, titleTop, titleW - iconSize - iconSplit, nTopH - titleTop), wxALIGN_TOP|wxALIGN_LEFT);
		}
	}

	//bottom center
	int nBottomCenterW = imgw - m_imgBottomLeft.GetWidth() - m_imgBottomRight.GetWidth();
	if(nBottomCenterW > 0)
	{
		wxImage tmpBottomCenter = m_imgBottomCenter.Scale(nBottomCenterW, nBottomH);
		dc.DrawBitmap(tmpBottomCenter, m_imgBottomLeft.GetWidth(), imgh - nBottomH, true);
	}
	dc.DrawBitmap(m_imgBottomLeft, 0, imgh - nBottomH, true);
	dc.DrawBitmap(m_imgBottomRight, imgw - m_imgBottomRight.GetWidth(), imgh - nBottomH, true);

	if(imgh - nTopH - nBottomH > 0 && imgw - m_imgLeft.GetWidth() - m_imgRight.GetWidth() > 0)
	{
		dc.SetPen(m_backColour);
		dc.SetBrush(m_backColour);
		dc.DrawRectangle(wxRect(m_imgLeft.GetWidth(), nTopH, imgw - m_imgLeft.GetWidth() - m_imgRight.GetWidth(), imgh - nTopH - nBottomH));
	}

	//dc.SetBrush(wxNullBrush);
	dc.SelectObject(wxNullBitmap);
	return true;
}

bool ccDrawSkin::DrawTool(wxColour backColour, wxSize size, const wxString& tagPath, wxBitmap& bitmapNormal, wxBitmap& bitmapMouseon, wxBitmap& bitmapPressed, wxBitmap& bitmapDisabled)
{
	if(xml.GetSize(tagPath) < 1)
	{
		return false;
	}

	wxString strTmp = xml.GetAttribute(tagPath, wxT("image"));
	if(strTmp.IsEmpty())
	{
		wxString tagPath2 = ccUtil::GetParentPath(tagPath);
		while(!tagPath2.IsEmpty() && strTmp.IsEmpty())
		{
			strTmp = xml.GetAttribute(tagPath2, wxT("image"));
			tagPath2 = ccUtil::GetParentPath(tagPath2);
		}
	}
	if(strTmp.IsEmpty())
	{
		return false;
	}

	strTmp = ccUtil::GetComparePath(m_xmlBasePath, strTmp);
	wxColour back;
	if(m_transparentColor.IsOk())
	{
		if(backColour.IsOk())
		{
			back = backColour;
		}
		else
		{
			back = m_backColour;
		}
	}

	wxImage image;
	wxString strRect = xml.GetValue(tagPath + wxT("/normall"));
	LoadImg(strTmp, strRect, image);
	if(size.x > 0 && (image.GetWidth() != size.x || image.GetHeight() != size.y))
	{
		image = image.Scale(size.x, size.y);
	}
	if(back.IsOk())
	{
		image.Replace(m_transparentColor.Red(), m_transparentColor.Green(), m_transparentColor.Blue(),
			back.Red(), back.Green(), back.Blue());
	}
	bitmapNormal = wxBitmap(image);

	strRect = xml.GetValue(tagPath + wxT("/mouseon"));
	LoadImg(strTmp, strRect, image);
	if(size.x > 0 && (image.GetWidth() != size.x || image.GetHeight() != size.y))
	{
		image = image.Scale(size.x, size.y);
	}
	if(back.IsOk())
	{
		image.Replace(m_transparentColor.Red(), m_transparentColor.Green(), m_transparentColor.Blue(),
			back.Red(), back.Green(), back.Blue());
	}
	bitmapMouseon = wxBitmap(image);

	strRect = xml.GetValue(tagPath + wxT("/pressed"));
	LoadImg(strTmp, strRect, image);
	if(size.x > 0 && (image.GetWidth() != size.x || image.GetHeight() != size.y))
	{
		image = image.Scale(size.x, size.y);
	}
	if(back.IsOk())
	{
		image.Replace(m_transparentColor.Red(), m_transparentColor.Green(), m_transparentColor.Blue(),
			back.Red(), back.Green(), back.Blue());
	}
	bitmapPressed = wxBitmap(image);

	strRect = xml.GetValue(tagPath + wxT("/disabled"));
	if(strRect.IsEmpty())
	{
		bitmapDisabled = bitmapNormal.ConvertToDisabled(_CC_ccDrawSkin_CONVERT_DISABLED_);
	}
	else
	{
		LoadImg(strTmp, strRect, image);
		if(size.x > 0 && (image.GetWidth() != size.x || image.GetHeight() != size.y))
		{
			image = image.Scale(size.x, size.y);
		}
		if(back.IsOk())
		{
			image.Replace(m_transparentColor.Red(), m_transparentColor.Green(), m_transparentColor.Blue(),
				back.Red(), back.Green(), back.Blue());
		}
		bitmapDisabled = wxBitmap(image);
	}
	return true;
}

bool ccDrawSkin::DrawButton(ccSkinButtonBase* ccSkinButton, wxBitmap& bitmap, int nType)
{
    wxCHECK_MSG( m_isLoaded, false, wxT("not loaded") );
	if(nType < _CC_ccDrawSkin_NORMAL_ || nType > _CC_ccDrawSkin_DISABLED_)
	{
		wxFAIL_MSG( wxT("nType not valid") );
		return false;
	}
	if(!ccSkinButton->IsEnabled())
	{
		nType = _CC_ccDrawSkin_BTN_DISABLED_;
	}

	int imgh = bitmap.GetHeight();
	int imgw = bitmap.GetWidth();
    wxMemoryDC memDC;
    memDC.SelectObject(bitmap);
	//memDC.SetBrush(m_backColour);
	/*
	if(m_transparentColor.IsOk())
	{
		memDC.SetPen(m_transparentColor);
		memDC.SetBrush(m_transparentColor);
		memDC.DrawRectangle(wxRect(0, 0, imgw, imgh));
	}
	*/
	wxWindow *win = ccSkinButton->GetParent();
	memDC.SetPen(win->GetBackgroundColour());
	memDC.SetBrush(win->GetBackgroundColour());
	memDC.DrawRectangle(wxRect(0, 0, imgw, imgh));
	memDC.SetBrush(wxNullBrush);
	memDC.SetPen(wxNullPen);
	memDC.SetTextForeground(ccSkinButton->GetForegroundColour());

	//btn height(should center and others have same height!)
	int nTopH = m_ccSkinButtonCenter[nType].GetHeight();
	int nLeft = m_ccSkinButtonLeft[nType].GetWidth();
	int nRight = m_ccSkinButtonRight[nType].GetWidth();
	if(imgh != nTopH)
	{
		wxImage tmpLeft = m_ccSkinButtonLeft[nType].Scale(nLeft, imgh);
		wxImage tmpCenter = m_ccSkinButtonCenter[nType].Scale(imgw - nLeft - nRight, imgh);
		wxImage tmpRight = m_ccSkinButtonRight[nType].Scale(nRight, imgh);

		//left
		memDC.DrawBitmap(tmpLeft, 0, 0, true);
		//center
		//FillImage(memDC, tmpCenter, nLeft, nCenter, imgw, 0, imgh, 0);
		memDC.DrawBitmap(tmpCenter, nLeft, 0, true);
		//right
		memDC.DrawBitmap(tmpRight, imgw - nRight, 0, true);
	}
	else
	{
		//left
		memDC.DrawBitmap(m_ccSkinButtonLeft[nType], 0, 0, true);
		//center
		//FillImage(memDC, m_ccSkinButtonCenter[nType], m_ccSkinButtonCenter[nType].GetWidth(), m_ccSkinButtonCenter[nType].GetWidth(), imgw, 0, m_ccSkinButtonCenter[nType].GetHeight(), 0);
		wxImage tmpCenter = m_ccSkinButtonCenter[nType].Scale(imgw - nLeft - nRight, imgh);
		memDC.DrawBitmap(tmpCenter, nLeft, 0, true);
		//right
		memDC.DrawBitmap(m_ccSkinButtonRight[nType], imgw - nRight, 0, true);
	}

	memDC.SetFont(ccSkinButton->GetFont());
	memDC.DrawLabel(ccSkinButton->GetLabel(), wxRect(0, 0, imgw, imgh), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
	//memDC.SetBrush(wxNullBrush);
	memDC.SelectObject(wxNullBitmap);
	/*
	if(m_transparentColor.IsOk())
	{
		wxMask* mask = new wxMask(bitmap, m_transparentColor);
		bitmap.SetMask(mask);
	}
	*/
	return true;
}

/*
//topl, top, topr, left, right, bottoml, bottom, bottomr
bool ccDrawSkin::LoadBackImg(const wxString& sImgPath, int nTop, int nBottom, int nLeftRight)
{
	if(!wxFileExists(sImgPath))
	{
		return false;
	}
	wxImage image;
    image.LoadFile(sImgPath);
	if(!image.IsOk())
	{
		return false;
	}

	m_nTop = nTop;
	m_nBottom = nBottom;
	m_nLeftRight = nLeftRight;
	int imgh = image.GetHeight();
	int imgw = image.GetWidth();
    m_imgBack[0] = image.GetSubImage( wxRect(0, 0, nLeftRight, nTop) );
    m_imgBack[1] = image.GetSubImage( wxRect(nLeftRight, 0, imgw - nLeftRight*2, nTop) );
    m_imgBack[2] = image.GetSubImage( wxRect(imgw - nLeftRight, 0, nLeftRight, nTop) );

    m_imgBack[3] = image.GetSubImage( wxRect(0, nTop, nLeftRight, imgh - nTop - nBottom) );
    m_imgBack[4] = image.GetSubImage( wxRect(imgw - nLeftRight, nTop, nLeftRight, imgh - nTop - nBottom) );

    m_imgBack[5] = image.GetSubImage( wxRect(0, imgh - nBottom, nLeftRight, nBottom) );
    m_imgBack[6] = image.GetSubImage( wxRect(nLeftRight, imgh - nBottom, imgw - nLeftRight*2, nBottom) );
    m_imgBack[7] = image.GetSubImage( wxRect(imgw - nLeftRight, imgh - nBottom, nLeftRight, nBottom) );
	m_isLoaded = true;
	return true;
}

//topl, top, topr, left, right, bottoml, bottom, bottomr
bool ccDrawSkin::ccDrawSkingImg(wxBitmap& bitmap)
{
	if(!m_isLoaded)
	{
		return false;
	}
	int imgh = bitmap.GetHeight();
	int imgw = bitmap.GetWidth();
	int nCenterWidth = m_imgBack[1].GetWidth();
	int nCenterHeight = m_imgBack[6].GetHeight();
    wxMemoryDC memDC;
    memDC.SelectObject( bitmap );
	memDC.SetBrush(m_backColour);

	memDC.DrawRectangle(0, 0, imgw, imgh);
	memDC.DrawBitmap( m_imgBack[0], 0, 0 );
	FillImage(memDC, m_imgBack[3], 0, m_nLeftRight, 0, m_nTop, nCenterHeight, imgh - m_nTop - m_nBottom);
	FillImage(memDC, m_imgBack[4], imgw - m_nLeftRight, m_nLeftRight, imgw - m_nLeftRight, m_nTop, nCenterHeight, imgh - m_nTop - m_nBottom);
	FillImage(memDC, m_imgBack[1], m_nLeftRight, nCenterWidth, imgw - m_nLeftRight*2, 0, m_nTop, 0);
	FillImage(memDC, m_imgBack[6], m_nLeftRight, nCenterWidth, imgw - m_nLeftRight*2, imgh - m_nBottom, m_nBottom, imgh - m_nBottom);
	memDC.DrawBitmap( m_imgBack[2], imgw - m_nLeftRight, 0 );
	memDC.DrawBitmap( m_imgBack[5], 0, imgh - m_nBottom );
	memDC.DrawBitmap( m_imgBack[7], imgw - m_nLeftRight, imgh - m_nBottom );

	memDC.SetBrush(wxNullBrush);
	memDC.SelectObject( wxNullBitmap );
	return true;
}

*/

// ccLabel
ccLabel::ccLabel( wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size )
	: wxStaticText( parent, id, title, pos, size )
{
	SetBackgroundColour(parent->GetBackgroundColour());
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	this->Bind(wxEVT_PAINT, &ccLabel::OnPaint, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, &ccLabel::OnEraseBackground, this);
}

ccLabel::~ccLabel()
{
}

void ccLabel::OnPaint(wxPaintEvent& event)
{
	wxStaticText::OnPaint(event);
	return;
	const wxString label = this->GetLabel();
	if(label.IsEmpty())
	{
		return;
	}
	wxSize size = this->GetSize();
	wxPaintDC paintDC(this);
    PrepareDC(paintDC);

	paintDC.SetBackground(this->GetParent()->GetBackgroundColour());
	paintDC.Clear();

	paintDC.SetBackgroundMode(wxTRANSPARENT);
	paintDC.SetFont(this->GetFont());
	paintDC.SetPen(this->GetForegroundColour());
	paintDC.DrawText(label, 0, 0);
	paintDC.SetPen(wxNullPen);
}

void ccLabel::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

// ccCheckbox
ccCheckbox::ccCheckbox( wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size )
	: wxCheckBox( parent, id, title, pos, size )
{
	SetBackgroundColour(parent->GetBackgroundColour());
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	this->Bind(wxEVT_PAINT, &ccCheckbox::OnPaint, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, &ccCheckbox::OnEraseBackground, this);
}

ccCheckbox::~ccCheckbox()
{
}

void ccCheckbox::OnPaint(wxPaintEvent& event)
{
	wxCheckBox::OnPaint(event);
}

void ccCheckbox::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

// ccClient
ccClient::ccClient( ccSkinFrame *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
	: wxScrolledWindow( parent, id, pos, size, style )
{
    SetBackgroundColour(parent->GetBackgroundColour());
	m_owner = parent;
}

ccClient::~ccClient()
{
}

// ccStatusBar
ccStatusBar::ccStatusBar( wxWindow *parent, int height, long style )
	: wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style )
{
    SetBackgroundColour(parent->GetBackgroundColour());
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	this->SetSize(0, height);

	this->Bind(wxEVT_PAINT, &ccStatusBar::OnPaint, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, &ccStatusBar::OnEraseBackground, this);
	this->Bind(wxEVT_SIZE, &ccStatusBar::OnResize, this);
}

ccStatusBar::~ccStatusBar()
{
}

void ccStatusBar::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxSize size = this->GetSize();
	wxPaintDC paintDC(this);
    PrepareDC(paintDC);

	paintDC.SetBackground(this->GetParent()->GetBackgroundColour());
	paintDC.Clear();

	paintDC.SetPen(*wxWHITE);
	paintDC.DrawLine(0, 1, size.x, 1);
	paintDC.SetPen(*wxBLACK);
	paintDC.DrawLine(0, 0, size.x, 0);
	paintDC.SetPen(wxNullPen);
}

void ccStatusBar::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void ccStatusBar::OnResize(wxSizeEvent& WXUNUSED(event))
{
    int mainW, mainH;
    this->DoGetSize(&mainW, &mainH);

	int nRight = mainW;
	// Then check wx level children.
    wxWindowList::compatibility_iterator first = GetChildren().GetFirst();
	if(!first || first->GetData() == NULL)
	{
		return;
	}

	wxWindow *childFirst = first->GetData();
	wxRect rectFirst = childFirst->GetRect();
    wxWindowList::compatibility_iterator current = GetChildren().GetLast();
    while (current)
    {
        wxWindow *childWin = current->GetData();
		if(childWin != childFirst && childWin->IsShown())
		{
			wxRect rect = childWin->GetRect();
			nRight = nRight - rect.width;
			childWin->SetPosition(wxPoint(nRight, rect.y));
		}
        current = current->GetPrevious();
    }

	if(nRight < 0)
	{
		nRight = 0;
	}
	childFirst->SetSize(rectFirst.x, rectFirst.y, nRight - rectFirst.x, rectFirst.height);
}

// main program
//IMPLEMENT_DYNAMIC_CLASS( ccSkinFrame, wxFrame )

//wxBEGIN_EVENT_TABLE(ccSkinFrame, wxFrame)
  //EVT_MOUSE_EVENTS(ccSkinFrame::OnMouseEvent)
  //EVT_PAINT(ccSkinFrame::OnPaint)
  //EVT_ERASE_BACKGROUND(ccSkinFrame::OnEraseBackground)
  //EVT_SIZE(ccSkinFrame::OnResize)
  //EVT_MOUSE_CAPTURE_LOST(ccSkinFrame::OnCaptureLost)
//wxEND_EVENT_TABLE()

ccSkinFrame::ccSkinFrame(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size)
: wxFrame( parent, id, title,
          pos, size, wxNO_BORDER | wxFRAME_SHAPED | wxFULL_REPAINT_ON_RESIZE )
{
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	m_lstDestroy.DeleteContents(true);
	//#646464
	//wxFRAME_NO_TASKBAR,wxRESIZE_BORDER|wxNO_BORDER,wxFRAME_SHAPED,wxFULL_REPAINT_ON_RESIZE
	//wxDefaultPosition, wxDefaultSize

    m_client = new ccClient(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_statusbar = NULL;
	m_frameLeft = 2;
	m_frameTop = 16;
	m_frameRight = 2;
	m_frameBottom = 2;
	m_controloxTopside = 0;
	m_controloxRightside = 5;
	m_controloxSplitwidth = 0;

	m_cursorNS = new wxCursor(wxCURSOR_SIZENS);
	m_cursorWE = new wxCursor(wxCURSOR_SIZEWE);
	m_cursorNESW = new wxCursor(wxCURSOR_SIZENESW);
	m_cursorNWSE = new wxCursor(wxCURSOR_SIZENWSE);
	m_DirectMode = -1;
	m_topMenu = NULL;

	m_bitmapBack = NULL;
	toolClose = new ccSkinTool(this, wxID_ANY, wxT("close"), wxPoint(size.x - 30, 1));
	toolMax = new ccSkinTool(this, wxID_ANY, wxT("max"), wxPoint(size.x - 50, 1));
	toolMaxMax = new ccSkinTool(this, wxID_ANY, wxT("max"), wxPoint(size.x - 50, 1));
	toolMin = new ccSkinTool(this, wxID_ANY, wxT("min"), wxPoint(size.x - 70, 1));
	toolMenu = new ccSkinTool(this, wxID_ANY, wxT("menu"), wxPoint(size.x - 90, 1));
	//show only set topmenu
	toolMaxMax->Show(false);
	toolMenu->Show(false);

	this->Bind(wxEVT_SIZE, &ccSkinFrame::OnResize, this);
	this->Bind(wxEVT_LEFT_DOWN, &ccSkinFrame::OnLeftDownEvent, this);
	this->Bind(wxEVT_LEFT_UP, &ccSkinFrame::OnLeftUpEvent, this);
	//this->Bind(wxEVT_ENTER_WINDOW, &ccSkinFrame::OnEnterEvent, this);
	this->Bind(wxEVT_LEAVE_WINDOW, &ccSkinFrame::OnLeaveEvent, this);
	this->Bind(wxEVT_MOTION, &ccSkinFrame::OnMotionEvent, this);
	this->Bind(wxEVT_MOUSE_CAPTURE_LOST, &ccSkinFrame::OnCaptureLost, this);
	this->Bind(wxEVT_PAINT, &ccSkinFrame::OnPaint, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, &ccSkinFrame::OnEraseBackground, this);
	this->Bind(wxEVT_LEFT_DCLICK, &ccSkinFrame::OnLeftDoubleClickEvent, this);

	toolClose->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ccSkinFrame::OnBoxClose), NULL, this);
	toolMax->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ccSkinFrame::OnBoxMax), NULL, this);
	toolMaxMax->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ccSkinFrame::OnBoxMax), NULL, this);
	toolMin->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ccSkinFrame::OnBoxMin), NULL, this);
	toolMenu->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ccSkinFrame::OnBoxMenu), NULL, this);

	//wxSizeEvent event;
	//OnResize(event);
	//post a event
	this->SendSizeEvent(wxSEND_EVENT_POST);
}

ccSkinFrame::~ccSkinFrame()
{
	wxDELETE(m_cursorNS);
	wxDELETE(m_cursorWE);
	wxDELETE(m_cursorNESW);
	wxDELETE(m_cursorNWSE);

	wxDELETE(m_bitmapBack);
	//for(int n = m_lstDestroy.GetCount() - 1; n >= 0; n--)
	//{
	//	wxObject* pObj = (void*)m_lstDestroy.Item(n)->GetData();
	//	delete pObj;
	//}
}

void ccSkinFrame::SetTopMenu(wxMenu *topMenu)
{
	m_topMenu = topMenu;
	toolMenu->Show(m_topMenu != NULL);
}

void ccSkinFrame::SetDrawSkin(ccDrawSkin* ccDrawSkin)
{
	m_ccDrawSkin = ccDrawSkin;
	wxBitmap m_bitmapNormal;
	wxBitmap m_bitmapMouseon;
	wxBitmap m_bitmapPressed;
	wxBitmap m_bitmapDisabled;
	m_frameLeft = m_ccDrawSkin->GetValue(wxT("/skin/frame/leftwidth"), 2);
	m_frameTop = m_ccDrawSkin->GetValue(wxT("/skin/frame/topheight"), 16);
	m_frameRight = m_ccDrawSkin->GetValue(wxT("/skin/frame/rightwidth"), 2);
	m_frameBottom = m_ccDrawSkin->GetValue(wxT("/skin/frame/bottomheight"), 2);
	m_frameLeft = CC_MAX(m_frameLeft, 2);
	m_frameTop = CC_MAX(m_frameTop, 16);
	m_frameRight = CC_MAX(m_frameRight, 2);
	m_frameBottom = CC_MAX(m_frameBottom, 2);
	m_controloxTopside = m_ccDrawSkin->GetValue(wxT("/skin/controlbutton/top"), 0);
	m_controloxRightside = m_ccDrawSkin->GetValue(wxT("/skin/controlbutton/right"), 5);
	m_controloxSplitwidth = m_ccDrawSkin->GetValue(wxT("/skin/controlbutton/splitwidth"), 0);
	m_titleIconSize = m_ccDrawSkin->GetValue(wxT("/skin/frame/titleiconsize"), 16);

	wxSize size = this->GetSize();
	int nLocX = size.x - m_controloxRightside;
	wxSize sizeEmpty;
	m_nToolWidth = 16; //may be last valid control's wdith, and need with same width for all controlbutton
	if(m_ccDrawSkin->DrawTool(this->GetBackgroundColour(), sizeEmpty, wxT("/skin/controlbutton/close"), m_bitmapNormal, m_bitmapMouseon, m_bitmapPressed, m_bitmapDisabled))
	{
		m_nToolWidth = m_bitmapNormal.GetWidth();
		toolClose->SetSize(m_nToolWidth, m_bitmapNormal.GetHeight());
		toolClose->SetBackImage(m_bitmapNormal, m_bitmapMouseon, m_bitmapPressed, m_bitmapDisabled);
		toolClose->Move(nLocX - m_nToolWidth, m_controloxTopside);
		nLocX = nLocX - m_controloxSplitwidth - m_nToolWidth;
	}
	if(m_ccDrawSkin->DrawTool(this->GetBackgroundColour(), sizeEmpty, wxT("/skin/controlbutton/maximize"), m_bitmapNormal, m_bitmapMouseon, m_bitmapPressed, m_bitmapDisabled))
	{
		m_nToolWidth = m_bitmapNormal.GetWidth();
		toolMax->SetSize(m_nToolWidth, m_bitmapNormal.GetHeight());
		toolMax->SetBackImage(m_bitmapNormal, m_bitmapMouseon, m_bitmapPressed, m_bitmapDisabled);
		toolMax->Move(nLocX - m_nToolWidth, m_controloxTopside);

		toolMaxMax->SetSize(m_nToolWidth, m_bitmapNormal.GetHeight());
		toolMaxMax->Move(nLocX - m_nToolWidth, m_controloxTopside);
		if(m_ccDrawSkin->DrawTool(this->GetBackgroundColour(), sizeEmpty, wxT("/skin/controlbutton/maximizemax"), m_bitmapNormal, m_bitmapMouseon, m_bitmapPressed, m_bitmapDisabled))
		{
			toolMaxMax->SetBackImage(m_bitmapNormal, m_bitmapMouseon, m_bitmapPressed, m_bitmapDisabled);
		}
		else
		{
			toolMaxMax->SetBackImage(m_bitmapNormal, m_bitmapMouseon, m_bitmapPressed, m_bitmapDisabled);
		}
		nLocX = nLocX - m_controloxSplitwidth - m_nToolWidth;
	}
	if(m_ccDrawSkin->DrawTool(this->GetBackgroundColour(), sizeEmpty, wxT("/skin/controlbutton/minimize"), m_bitmapNormal, m_bitmapMouseon, m_bitmapPressed, m_bitmapDisabled))
	{
		m_nToolWidth = m_bitmapNormal.GetWidth();
		toolMin->SetSize(m_nToolWidth, m_bitmapNormal.GetHeight());
		toolMin->SetBackImage(m_bitmapNormal, m_bitmapMouseon, m_bitmapPressed, m_bitmapDisabled);
		toolMin->Move(nLocX - m_nToolWidth, m_controloxTopside);
		nLocX = nLocX - m_controloxSplitwidth - m_nToolWidth;
	}
	if(m_ccDrawSkin->DrawTool(this->GetBackgroundColour(), sizeEmpty, wxT("/skin/controlbutton/topmenu"), m_bitmapNormal, m_bitmapMouseon, m_bitmapPressed, m_bitmapDisabled))
	{
		m_nToolWidth = m_bitmapNormal.GetWidth();
		toolMenu->SetSize(m_nToolWidth, m_bitmapNormal.GetHeight());
		toolMenu->SetBackImage(m_bitmapNormal, m_bitmapMouseon, m_bitmapPressed, m_bitmapDisabled);
		toolMenu->Move(nLocX - m_nToolWidth, m_controloxTopside);
		nLocX = nLocX - m_controloxSplitwidth - m_nToolWidth;
	}

	UpdateSkin();
}

//topl, top, topr, left, right, bottoml, bottom, bottomr
//void ccSkinFrame::LoadBackImg(const wxString& sImgPath, int nTop, int nBottom, int nLeftRight)
//{
	//m_ccDrawSkin.LoadBackImg(sImgPath, nTop, nBottom, nLeftRight);
//}

void ccSkinFrame::OnBoxMin( wxCommandEvent &WXUNUSED(event) )
{
	bool newStatus = !this->IsIconized();
	this->Iconize(newStatus);
}

void ccSkinFrame::OnBoxMax( wxCommandEvent &WXUNUSED(event) )
{
	bool newStatus = !this->IsMaximized();
	this->Maximize(newStatus);
	toolMax->Show(!newStatus);
	toolMaxMax->Show(newStatus);
}

void ccSkinFrame::OnBoxClose( wxCommandEvent &WXUNUSED(event) )
{
	this->Close();
}

void ccSkinFrame::OnBoxMenu( wxCommandEvent &WXUNUSED(event) )
{
	if(m_topMenu != NULL)
	{
		wxPoint point = toolMenu->GetPosition();
		point.y += toolMenu->GetSize().y;
		PopupMenu(m_topMenu, point);
	}
}

void ccSkinFrame::OnLeftDoubleClickEvent( wxMouseEvent &WXUNUSED(event) )
{
	wxPoint mousePos = ::wxGetMousePosition();
	mousePos = ScreenToClient(mousePos);

    // What is the mode
    if(mousePos.y <= m_frameTop)
    {
		bool newStatus = !this->IsMaximized();
		this->Maximize(newStatus);
    }
}

void ccSkinFrame::OnLeaveEvent(wxMouseEvent& WXUNUSED(event))
{
	SetCursor(wxNullCursor);
}

void ccSkinFrame::OnLeftDownEvent(wxMouseEvent& WXUNUSED(event))
{
	if(m_DirectMode == -1)
	{
		m_DirectMode = 4; // Moving
	}
	CaptureMouse();
	m_oldMousePos = ::wxGetMousePosition();
	m_oldWinRect = GetScreenRect();
}

void ccSkinFrame::OnLeftUpEvent(wxMouseEvent& WXUNUSED(event))
{
	m_DirectMode = -1;
	if (HasCapture())
	{
		ReleaseMouse();
	}
}

void ccSkinFrame::OnMotionEvent(wxMouseEvent& event)
{
	if(this->IsMaximized())
	{
		return;
	}
	wxPoint mousePos = ::wxGetMousePosition();
    if (HasCapture())
    {
		if (event.Dragging() && event.LeftIsDown())
		{
			int deltax, deltay;
			deltax = (mousePos.x - m_oldMousePos.x);
			deltay = (mousePos.y - m_oldMousePos.y);
			if(deltax == 0 && deltay == 0)
			{
				wxLogDebug("=========================");
				return;
			}

			// Some variables
			wxRect winPos = GetScreenRect();
			// Move
			if(m_DirectMode == 4)
			{
				winPos.x += deltax;
				winPos.y += deltay;
				m_oldMousePos = mousePos;
			}
			else
			{
				// Resize if needed
				const wxRect curPos = winPos;
				const wxSize minSize = GetMinSize();
				//wxLogDebug("M, oldx:%d, oldy:%d, mx:%d, my:%d, ####deltax:%d, deltay:%d####, d:%d, cur w:%d, cur h;%d", m_oldMousePos.x, m_oldMousePos.y, mousePos.x, mousePos.y, deltax, deltay, m_DirectMode, curPos.width, curPos.height);

				// Adjust x/w
				if(m_DirectMode == 0 || m_DirectMode == 3 || m_DirectMode == 6)
				{
					winPos.x = mousePos.x;
					winPos.width = m_oldWinRect.width - deltax;
				}
				else if(m_DirectMode == 2 || m_DirectMode == 5 || m_DirectMode == 8)
				{
					winPos.width = m_oldWinRect.width + deltax;
				}
				if(winPos.width < minSize.x)
				{
					winPos.width = minSize.x;
					if(m_DirectMode == 2 || m_DirectMode == 5 || m_DirectMode == 8)
					{
						winPos.x = m_oldWinRect.x;
					}
					else
					{
						winPos.x = m_oldWinRect.x + m_oldWinRect.width - minSize.x;
					}
				}

				// Adjust y/h
				if(m_DirectMode == 0 || m_DirectMode == 1 || m_DirectMode == 2)
				{
					winPos.y = mousePos.y;
					winPos.height = m_oldWinRect.height - deltay;
				}
				else if(m_DirectMode == 6 || m_DirectMode == 7 || m_DirectMode == 8)
				{
					winPos.height = m_oldWinRect.height + deltay;
				}
				if(winPos.height < minSize.y)
				{
					winPos.height = minSize.y;
					if(m_DirectMode == 6 || m_DirectMode == 7 || m_DirectMode == 8)
					{
						winPos.y = m_oldWinRect.y;
					}
					else
					{
						winPos.y = m_oldWinRect.y + m_oldWinRect.height - minSize.y;
					}
				}
				//wxLogDebug("M, oldx:%d, oldy:%d, win x:%d, win y:%d, win w:%d, win h;%d", m_oldMousePos.x, m_oldMousePos.y, winPos.x, winPos.y, winPos.width, winPos.height);
			}
			// Set the size
			// The real position will get correctly set
			SetSize(winPos.x, winPos.y, winPos.width, winPos.height);
		}
	}
	else {
		mousePos = ScreenToClient(mousePos);
        wxSize size = GetSize();

        // What is the mode
        int part1 = 1;
        int part2 = 1;
        if(mousePos.x < 5)
        {
            part1 = 0;
        }
        else if(mousePos.x > size.x - 5)
        {
            part1 = 2;
        }

        if(mousePos.y < 5)
        {
            part2 = 0;
        }
        else if(mousePos.y > size.y - 5)
        {
            part2 = 2;
        }

        m_DirectMode = part1 + (3 * part2);
        // Mouse cursor
        switch(m_DirectMode)
        {
            case 0:
            case 8:
                SetCursor(*m_cursorNWSE);
                break;

            case 1:
            case 7:
                SetCursor(*m_cursorNS);
                break;

            case 2:
            case 6:
                SetCursor(*m_cursorNESW);
                break;

            case 3:
            case 5:
                SetCursor(*m_cursorWE);
                break;

            case 4:
            default:
                SetCursor(wxNullCursor);
                break;
        };
	}
}

void ccSkinFrame::OnCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
{
    m_DirectMode = -1;
    SetCursor(wxNullCursor);
}

void ccSkinFrame::AddSkinButton(ccSkinButton* imgBtn)
{
	imgBtn->SetDrawSkin(m_ccDrawSkin);
	m_lstSkinBtn.Append((wxObject*)imgBtn);
}
void ccSkinFrame::RemoveSkinButton(ccSkinButton* imgBtn)
{
	imgBtn->SetDrawSkin(NULL);
	m_lstSkinBtn.remove((wxObject*)imgBtn);
}
void ccSkinFrame::UpdateSkin()
{
	for(int n = m_lstSkinBtn.GetCount() - 1; n >= 0; n--)
	{
		ccSkinButton* imgBtn = (ccSkinButton*)m_lstSkinBtn.Item(n)->GetData();
		imgBtn->UpdateSkin();
	}
}

void ccSkinFrame::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void ccSkinFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC paintDC(this);
	GetBackBitmap(false);
    paintDC.DrawBitmap(*m_bitmapBack, 0, 0);
}

void ccSkinFrame::OnResize(wxSizeEvent& WXUNUSED(event))
{
	//_test_draw();
    const wxSize size = GetSize();
	int sw = 0;
	int sh = 0;
    if ( m_statusbar && m_statusbar->IsShown() )
	{
		m_statusbar->GetSize(&sw, &sh);
		m_statusbar->SetSize(m_frameLeft, size.y - m_frameBottom - sh, size.x - m_frameRight - m_frameLeft, sh);
	}

	if(m_client != NULL)
	{
		m_client->SetSize(m_frameLeft, m_frameTop, size.x - m_frameRight - m_frameLeft, size.y - m_frameBottom - m_frameTop - sh);
	}

	int nLocX = size.x - m_controloxRightside - m_nToolWidth;
	toolClose->Move(nLocX, m_controloxTopside);
	nLocX = nLocX - m_controloxSplitwidth - m_nToolWidth;
	toolMax->Move(nLocX, m_controloxTopside);
	toolMaxMax->Move(nLocX, m_controloxTopside);
	nLocX = nLocX - m_controloxSplitwidth - m_nToolWidth;
	toolMin->Move(nLocX, m_controloxTopside);
	nLocX = nLocX - m_controloxSplitwidth - m_nToolWidth;
	toolMenu->Move(nLocX, m_controloxTopside);

	GetBackBitmap(true);
	wxColour tColour = m_ccDrawSkin->GetTransparentColour();
	if(tColour.IsOk())
	{
		wxRegion bitmapRegion(*m_bitmapBack, tColour);
		this->SetShape(bitmapRegion);
	}

	//wxPaintEvent x;
	//OnPaint(x);
	//Refresh(false);
	this->Update();
}

#if wxUSE_STATUSBAR
void ccSkinFrame::PositionStatusBar()
{
}
#endif // wxUSE_STATUSBAR

void ccSkinFrame::GetBackBitmap(bool isReCreate)
{
	if(isReCreate || m_bitmapBack == NULL)
	{
		wxSize size = GetSize();
		if(m_bitmapBack == NULL)
		{
			m_bitmapBack = new wxBitmap(size.x, size.y);
		}
		else
		{
			m_bitmapBack->Create(size.x, size.y);
		}

		if(this->GetIcon().IsOk())
		{
			if(!m_titleIcon.IsOk() || m_titleIcon.GetHeight() != m_titleIconSize)
			{
				wxImage tmpIcon = wxBitmap(this->GetIcon()).ConvertToImage();
				m_titleIcon = wxBitmap(tmpIcon.Scale(m_titleIconSize, m_titleIconSize));
			}
		}
		m_ccDrawSkin->DrawFrame(this, *m_bitmapBack, m_titleIcon);
	}
}

ccSkinButtonBase::ccSkinButtonBase(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size)
: wxButton( parent, id, title,
          pos, size, wxNO_BORDER | wxFRAME_SHAPED | wxFULL_REPAINT_ON_RESIZE )
{
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	this->Bind(wxEVT_SIZE, &ccSkinButtonBase::OnResize, this);
	this->Bind(wxEVT_LEFT_DOWN, &ccSkinButtonBase::OnLeftDownEvent, this);
	this->Bind(wxEVT_LEFT_UP, &ccSkinButtonBase::OnLeftUpEvent, this);
	this->Bind(wxEVT_ENTER_WINDOW, &ccSkinButtonBase::OnEnterEvent, this);
	this->Bind(wxEVT_LEAVE_WINDOW, &ccSkinButtonBase::OnLeaveEvent, this);
	this->Bind(wxEVT_MOTION, &ccSkinButtonBase::OnMotionEvent, this);
	this->Bind(wxEVT_MOUSE_CAPTURE_LOST, &ccSkinButtonBase::OnCaptureLost, this);
	this->Bind(wxEVT_LEFT_DCLICK, &ccSkinButtonBase::OnLeftDoubleClickEvent, this);
	this->Bind(wxEVT_PAINT, &ccSkinButtonBase::OnPaint, this);
	//this->Bind(wxEVT_ERASE_BACKGROUND, &ccSkinButtonBase::OnEraseBackground, this);
	this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

ccSkinButtonBase::~ccSkinButtonBase()
{
}

void ccSkinButtonBase::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	if(!m_bitmapBack.IsOk())
	{
		this->doDrawBack(_CC_ccDrawSkin_NORMAL_);
	}
	if(m_bitmapBack.IsOk())
	{
		wxPaintDC paintDC(this);
		/*
		if(m_bitmapBack.GetMask() != NULL)
		{
			wxSize size = this->GetSize();
			wxWindow* win = (wxWindow*)this->GetParent();
			const wxBitmap* bitmap = win->GetBackBitmap();
			if(bitmap != NULL)
			{
				wxRect rect(0, 0, 100, 100);
				//wxImage image = bitmap->ConvertToImage();
				//wxBitmap btimap2 = bitmap->GetSubBitmap(rect);
				//wxBitmap btimap2 = wxBitmap(image.GetSubImage(rect));
				//paintDC.DrawBitmap(btimap2, 0, 0, true);
			}
			else
			{
				paintDC.SetPen(win->GetBackgroundColour());
				paintDC.SetBrush(win->GetBackgroundColour());
				paintDC.DrawRectangle(wxRect(0, 0, size.x, size.y));
				paintDC.SetBrush(wxNullBrush);
				paintDC.SetPen(wxNullPen);
			}
		}
		*/
		paintDC.DrawBitmap(m_bitmapBack, 0, 0, true);
	}
}

void ccSkinButtonBase::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void ccSkinButtonBase::OnEnterEvent(wxMouseEvent& WXUNUSED(event))
{
	//wxLogDebug("Enter, :%d, cap:%d", event.LeftDown(), HasCapture());
	this->doDrawBack(_CC_ccDrawSkin_MOUSEON_);
}

void ccSkinButtonBase::OnLeaveEvent(wxMouseEvent& WXUNUSED(event))
{
	//wxLogDebug("Leave, :%d, cap:%d", event.LeftDown(), HasCapture());
	this->doDrawBack(_CC_ccDrawSkin_NORMAL_);
}

void ccSkinButtonBase::OnLeftDoubleClickEvent(wxMouseEvent& WXUNUSED(event))
{
	//wxLogDebug("DOWN, :%d, cap:%d", event.LeftDown(), HasCapture());
}

void ccSkinButtonBase::OnLeftDownEvent(wxMouseEvent& WXUNUSED(event))
{
	//wxLogDebug("DOWN, :%d, cap:%d", event.LeftDown(), HasCapture());
	//if (!HasCapture())
	{
		CaptureMouse();
	}
	this->doDrawBack(_CC_ccDrawSkin_PRESSED_);
}

void ccSkinButtonBase::OnLeftUpEvent(wxMouseEvent& event)
{
	//wxLogDebug("UPUP, :%d, cap:%d", event.LeftDown(), HasCapture());
	if (HasCapture())
	{
		if(GetClientRect().Contains(event.GetPosition()))
		{
			wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
			GetEventHandler()->AddPendingEvent(evt);
			this->doDrawBack(_CC_ccDrawSkin_MOUSEON_);
		}
		else
		{
			this->doDrawBack(_CC_ccDrawSkin_NORMAL_);
		}
		//DoReleaseMouse();
		ReleaseMouse();
	}
}

void ccSkinButtonBase::OnMotionEvent(wxMouseEvent& event)
{
	//wxLogDebug("M, mdown:%d, HasCapture:%d", event.LeftDown(), HasCapture());
    if (HasCapture())
    {
		if (event.Dragging() && event.LeftIsDown())
		{
			if(GetClientRect().Contains(event.GetPosition()))
			{
				this->doDrawBack(_CC_ccDrawSkin_PRESSED_);
			}
			else
			{
				this->doDrawBack(_CC_ccDrawSkin_NORMAL_);
			}
		}
	}
	else
	{
		if(GetClientRect().Contains(event.GetPosition()))
		{
			this->doDrawBack(_CC_ccDrawSkin_MOUSEON_);
		}
		else
		{
			this->doDrawBack(_CC_ccDrawSkin_NORMAL_);
		}
	}
}

void ccSkinButtonBase::OnCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
{
	this->doDrawBack(_CC_ccDrawSkin_NORMAL_);
}

void ccSkinButtonBase::OnResize(wxSizeEvent& WXUNUSED(event))
{
	doOnResize();
}

void ccSkinButtonBase::doOnResize()
{
	this->doDrawBack(_CC_ccDrawSkin_NORMAL_);
}

void ccSkinButtonBase::doDrawBack(int nType)
{
	if(!m_bitmapBack.IsOk())
	{
		const wxSize size = this->GetSize();
		m_bitmapBack = wxBitmap(size.x, size.y);
	}
	m_ccDrawSkin->DrawButton(this, m_bitmapBack, nType);
	this->Refresh();
}

void ccSkinButtonBase::UpdateSkin()
{
	doDrawBack(_CC_ccDrawSkin_NORMAL_);
}

//////////////////////////////////////////////////////////////////////////////// ccSkinButton
ccSkinButton::ccSkinButton(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size)
: ccSkinButtonBase( parent, id, title, pos, size )
{
}

ccSkinButton::~ccSkinButton()
{
}

void ccSkinButton::doDrawBack(int nType)
{
	ccSkinButtonBase::doDrawBack(nType);
	m_bitmapBack.SaveFile(wxT("c:\\temp\\a1.png"), wxBITMAP_TYPE_PNG);
}

//////////////////////////////////////////////////////////////////////////////// ccSkinTool
ccSkinTool::ccSkinTool(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size)
: ccSkinButtonBase( parent, id, title, pos, size )
{
}

ccSkinTool::~ccSkinTool()
{
}

void ccSkinTool::doDrawBack(int nType)
{
	if(m_bitmapNormal.IsOk())
	{
		if(nType == _CC_ccDrawSkin_MOUSEON_)
		{
			m_bitmapBack = m_bitmapMouseon;
		}
		else if(nType == _CC_ccDrawSkin_PRESSED_)
		{
			m_bitmapBack = m_bitmapPressed;
		}
		else if(nType == _CC_ccDrawSkin_DISABLED_)
		{
			m_bitmapBack = m_bitmapDisabled;
		}
		else
		{
			m_bitmapBack = m_bitmapNormal;
		}
		this->Refresh();
	}
}

void ccSkinTool::SetBackImage(const wxBitmap& bitmapNormal, const wxBitmap& bitmapMouseon, const wxBitmap& bitmapPressed, const wxBitmap& bitmapDisabled)
{
	m_bitmapNormal = wxBitmap(bitmapNormal);
	m_bitmapMouseon = wxBitmap(bitmapMouseon);
	m_bitmapPressed = wxBitmap(bitmapPressed);
	m_bitmapDisabled = wxBitmap(bitmapDisabled);
	doOnResize();
}

void ccSkinTool::doOnResize()
{
	if(m_bitmapNormal.IsOk())
	{
		wxSize size = this->GetSize();
		if(m_bitmapNormal.GetWidth() != size.x || m_bitmapNormal.GetHeight() != size.y)
		{
			wxImage img = wxImage(m_bitmapNormal.ConvertToImage()).Scale(size.x, size.y);
			m_bitmapNormal = wxBitmap(img);
		}
		if(m_bitmapMouseon.GetWidth() != size.x || m_bitmapMouseon.GetHeight() != size.y)
		{
			wxImage img = wxImage(m_bitmapMouseon.ConvertToImage()).Scale(size.x, size.y);
			m_bitmapMouseon = wxBitmap(img);
		}
		if(m_bitmapPressed.GetWidth() != size.x || m_bitmapPressed.GetHeight() != size.y)
		{
			wxImage img = wxImage(m_bitmapPressed.ConvertToImage()).Scale(size.x, size.y);
			m_bitmapPressed = wxBitmap(img);
		}
		if(m_bitmapDisabled.GetWidth() != size.x || m_bitmapDisabled.GetHeight() != size.y)
		{
			wxImage img = wxImage(m_bitmapDisabled.ConvertToImage()).Scale(size.x, size.y);
			m_bitmapDisabled = wxBitmap(img);
		}
		m_bitmapBack = m_bitmapNormal;
		//not need ccSkinButtonBase::doOnResize()
		//ccSkinButtonBase::doOnResize();

		//next will ocuse memory leak
		//wxPaintEvent x;
		//OnPaint(x);
		this->Refresh(true);
	}
}