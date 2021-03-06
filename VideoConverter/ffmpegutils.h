/////////////////////////////////////////////////////////////////////////////
// Name:        ccSkinFrame.h
// Purpose:     wxDragImage sample
// Author:      Julian Smart
// Modified by:
// Created:     28/2/2000
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FFMPEG_CTL_
#define _WX_FFMPEG_CTL_

#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
//#define inline _inline
//#ifndef INT64_C
//#define INT64_C(c) (c ## LL)
//#define UINT64_C(c) (c ## ULL)
//#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif
	#include "libavformat/avformat.h"
	#include <libavcodec/avcodec.h>
	//#include "libavcodec/mathops.h"
	#include <libavdevice/avdevice.h>
	#include <libswscale/swscale.h>
	#include <libavfilter/avfilter.h>
	#include "libavfilter/avfiltergraph.h"
	#include "libavfilter/buffersrc.h"
	#include "libavfilter/buffersink.h"
	#include "libavutil/avutil.h"
	#include "libavutil/opt.h"
	#include "libavutil/channel_layout.h"
	#include "libavutil/parseutils.h"
	#include "libavutil/samplefmt.h"
	#include "libavutil/fifo.h"
	//#include "libavutil/internal.h"
	#include "libavutil/intreadwrite.h"
	#include "libavutil/dict.h"
	#include "libavutil/mathematics.h"
	#include "libavutil/pixdesc.h"
	#include "libavutil/avstring.h"
	//#include "libavutil/libm.h"
	#include "libavutil/imgutils.h"
	//#include "libavutil/timestamp.h"
	#include "libavutil/bprint.h"
	#include "libavutil/time.h"
	#include "libavutil/threadmessage.h"
	#include "libavutil/pixfmt.h"
	//#include "libavformat/os_support.h"
#ifdef __cplusplus
}
#endif

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")










#include <wx/string.h>
#include <wx/image.h>
struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVStream;

enum StreamType {
	stUNKNOWN = 0,
	stVIDEO,
	stAUDIO,
	stSUBTITLE
};

class wxFfmpegDecoder {
public:
	wxFfmpegDecoder();
	virtual ~wxFfmpegDecoder();
	static void Init();
	
	static void GetFormatList(wxArrayString& formatIn, wxArrayString& formatOut, wxArrayString& formatAudio, wxArrayString& formatVideo);
	virtual bool Load(const wxString& fileName);
	virtual void Close();
	
	unsigned int GetStreamCount();
	StreamType GetStreamType(unsigned int streamIndex);
	/** Returns codec name (e.g. h264, MPEG4) */
	wxString GetCodecName(unsigned int streamIndex);
	/** Returns codec tag (fourcc, e.g. DIVX or XVID) */
	wxString GetCodecTag(unsigned int streamIndex);
	int GetChannelNumber(unsigned int streamIndex);
	int GetSampleRate(unsigned int streamIndex);
	int GetBitrate(unsigned int streamIndex);
	wxString GetBitrateString(unsigned int streamIndex);
	
	double GetDuration();
	wxString GetDurationString();
	bool SetPosition(double pos, bool keyFrame = true);
	double GetPosition();
	
	virtual wxSize GetVideoSize();
	virtual float GetFrameAspectRatio();
	virtual float GetFps();
	
	virtual bool BeginDecode(int width = -1, int height = -1);
	virtual bool GetNextFrame(wxImage& frame);
	virtual void EndDecode();
	
	/** Returns a comma separated list of short names for the format. */
	wxString GetFormatName();
	/** Returns time base for video codec (tbc). */
	float GetCodecTimeBase();
	
private:
	int m_videoStream;
	int m_audioStream;
	AVFormatContext* m_formatCtx;
	AVCodecContext* m_codecCtx;
	AVCodecContext* m_codecCtx_au;
	AVFrame* m_frame;
	int m_width;
	int m_height;
	AVStream* GetVideoStream();
};










#endif // _WX_FFMPEG_CTL_
