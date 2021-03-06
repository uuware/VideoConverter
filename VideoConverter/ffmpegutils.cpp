/////////////////////////////////////////////////////////////////////////////
// Name:        ffmpeg.cpp
// Purpose:     FFMPEG Media Decoder
// Author:      Alex Thuering
// Created:     21.07.2007
// RCS-ID:      $Id: mediadec_ffmpeg.cpp,v 1.32 2015/09/21 13:23:51 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <ffmpegutils.h>
#include <wx/wx.h>
#include <errno.h>

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

void wxFfmpegDecoder::GetFormatList(wxArrayString& formatIn, wxArrayString& formatOut, wxArrayString& formatAudio, wxArrayString& formatVideo)
{
	Init();

	AVCodec *first_c = NULL;
	AVInputFormat *first_if = NULL;
	AVOutputFormat *first_of = NULL;

	first_if = av_iformat_next(NULL);
	first_of = av_oformat_next(NULL);
	first_c = av_codec_next(NULL);

	AVInputFormat *if_temp = first_if;
	AVOutputFormat *of_temp = first_of;
	AVCodec *c_temp = first_c;

	//InputFormat
	while(if_temp != NULL)
	{
		wxString name(if_temp->name);
		formatIn.Add(name);

		if_temp = if_temp->next;
	}

	//OutputFormat
	while(of_temp!=NULL)
	{
		wxString name(of_temp->name);
		formatOut.Add(name);

		of_temp = of_temp->next;
	}

	//Codec
	while(c_temp!=NULL)
	{
		wxString name(c_temp->name);
		switch(c_temp->type){
		case AVMEDIA_TYPE_VIDEO:
			{
			formatVideo.Add(name);
			break;
			}
		case AVMEDIA_TYPE_AUDIO:
			{
			formatAudio.Add(name);
			break;
			}
		default:
			{
			break;
			}
		}

		c_temp = c_temp->next;
	}
}

wxFfmpegDecoder::wxFfmpegDecoder(): m_formatCtx(NULL), m_videoStream(-1), m_audioStream(-1), m_codecCtx(NULL), m_codecCtx_au(NULL), m_frame(NULL),
		m_width(0), m_height(0)
{
	Init();
}

wxFfmpegDecoder::~wxFfmpegDecoder()
{
    Close();
}

void wxFfmpegDecoder::Init()
{
	static int flg = 0;
	if(flg == 0)
	{
		flg = 1;
		av_register_all();
	}
}

void PrintError(const wxString& msg, int err)
{
	char errbuf[128];
	const char *errbuf_ptr = errbuf;
	
	if (av_strerror(err, errbuf, sizeof(errbuf)) < 0)
	{
		errbuf_ptr = strerror(AVUNERROR(err));
	}
	wxString errorStr(errbuf_ptr, wxConvUTF8);
	wxLogError(msg + wxT(": ") + errorStr);
}

bool wxFfmpegDecoder::Load(const wxString& fileName)
{
	Close();
	if ((fileName.Find(wxT(':')) <= 0 || fileName.Find(wxT(':')) >= 8
			|| fileName.GetChar(fileName.Find(wxT(':')) - 1) == wxT('\\'))
			&& !wxFileExists(fileName))
	{
		wxLogError(wxT("%s: no such file or directory"), fileName.c_str());
		return false;
	}
//#ifdef __WXMSW__
//	int err = avformat_open_input(&m_formatCtx, fileName.utf8_str(), NULL, NULL);
//#else
	int err = avformat_open_input(&m_formatCtx, fileName.mb_str(), NULL, NULL);
//#endif
	if (err != 0)
	{
		PrintError(fileName, err);
		return false;
	}
	// Retrieve stream information
	if (avformat_find_stream_info(m_formatCtx, NULL) < 0)
	{
		wxLogError(wxT("%s: could not find codec parameters"), fileName.c_str());
		return false;
	}

	for(unsigned int i = 0; i < m_formatCtx->nb_streams; i++)
	{
		if(m_formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_videoStream = i;
		}
		if(m_formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			m_audioStream = i;
		}
	}
	if(m_videoStream == -1 && m_audioStream == -1)
	{
		wxLogError(wxT("%s: could not find a video/audio stream"), fileName.c_str());
		Close();
		return false;
	}

	if(m_videoStream != -1)
	{
		// get a pointer to the codec context for the video stream 
		m_codecCtx = m_formatCtx->streams[m_videoStream]->codec;
		// find and open the decoder for the video stream 
		AVCodec* codec = avcodec_find_decoder(m_codecCtx->codec_id);
		if (!codec || avcodec_open2(m_codecCtx, codec, NULL) < 0)
		{
			wxLogError(wxT("%s: could not avcodec_find_decoder"), fileName.c_str());
			m_codecCtx = NULL; //not need close m_codecCtx
			Close();
			return false;
		}
	}

	if(m_audioStream != -1)
	{
		m_codecCtx_au = m_formatCtx->streams[m_audioStream]->codec;
		AVCodec* pCodec_au = avcodec_find_decoder(m_codecCtx_au->codec_id);
		if(!pCodec_au || avcodec_open2(m_codecCtx_au, pCodec_au, NULL) < 0)
		{
			wxLogError(wxT("%s: could not open audio codec"), fileName.c_str());
			m_codecCtx_au = NULL; //not need close m_codecCtx_au
			Close();
			return false;
		}
		//decoder_name_au.Format(_T("%s"), m_codecCtx_au->codec->long_name);
		//sample_rate_au.Format(_T("%d"), m_codecCtx_au->sample_rate);
		//channels_au.Format(_T("%d"), m_codecCtx_au->channels);
	}
	return true;
}

bool wxFfmpegDecoder::BeginDecode(int width, int height)
{
    if (!m_formatCtx || !m_codecCtx)
	{
        return false;
	}
    // video size
	if (width < 1 || height < 1)
	{
		m_width = m_codecCtx->width;
		m_height = m_codecCtx->height;
	}
	else
	{
		int w = m_codecCtx->width * height / m_codecCtx->height;
		int h = m_codecCtx->height * width / m_codecCtx->width;
		m_width = w < width ? w : width;
		m_height = w < width ? height : h;
	}
    // allocate video frame
    m_frame = av_frame_alloc();
    if (!m_frame)
	{
        avcodec_close(m_codecCtx);
        m_codecCtx = NULL;
        return false;
    }
    return true;
}

void wxFfmpegDecoder::Close()
{
	EndDecode();
	if (m_formatCtx)
	{
		avformat_close_input(&m_formatCtx);
		m_formatCtx = NULL;
	}
}

void wxFfmpegDecoder::EndDecode()
{
	if(m_frame != NULL)
	{
		av_frame_free(&m_frame);
		m_frame = NULL;
	}
    if (m_codecCtx != NULL)
	{
		avcodec_close(m_codecCtx); // close the codec
		m_codecCtx = NULL;
	}
    if (m_codecCtx_au != NULL)
	{
		avcodec_close(m_codecCtx_au); // close the codec
		m_codecCtx_au = NULL;
	}
}

unsigned int wxFfmpegDecoder::GetStreamCount()
{
	return m_formatCtx != NULL ? m_formatCtx->nb_streams : 0;
}

wxSize wxFfmpegDecoder::GetVideoSize()
{
    return m_codecCtx ? wxSize(m_codecCtx->width, m_codecCtx->height) : wxSize();
}

AVStream* wxFfmpegDecoder::GetVideoStream()
{
	if (m_formatCtx == NULL || m_videoStream == -1)
	{
		return NULL;
	}
	return m_formatCtx->streams[m_videoStream];
}

float wxFfmpegDecoder::GetFrameAspectRatio()
{
	AVStream *st = GetVideoStream();
	if (st == NULL)
	{
		return -1;
	}
	float frame_aspect_ratio = 1;
	AVCodecContext *enc = st->codec;
	if (st->sample_aspect_ratio.num)
	{
		frame_aspect_ratio = av_q2d(st->sample_aspect_ratio);
	}
	else if (enc->sample_aspect_ratio.num)
	{
		frame_aspect_ratio = av_q2d(enc->sample_aspect_ratio);
	}
	frame_aspect_ratio *= (float) enc->width / enc->height;
	return frame_aspect_ratio;
}

float wxFfmpegDecoder::GetFps()
{
	float result = -1;
	AVStream *st = GetVideoStream();
	if (st == NULL)
	{
		return result;
	}
	if (st->avg_frame_rate.num && st->avg_frame_rate.den)
	{
		if (st->avg_frame_rate.num > st->avg_frame_rate.den)
		{
			result = ((float) st->avg_frame_rate.num) / st->avg_frame_rate.den;
		}
		else
		{
			result = ((float) st->avg_frame_rate.den) / st->avg_frame_rate.num;
		}
	}
	/*
	//帧率显示还有问题
	framerate_temp = (pFormatCtx->streams[video_stream]->r_frame_rate.num)/(pFormatCtx->streams[video_stream]->r_frame_rate.den);
	framerate.Format(_T("%5.2ffps"), framerate_temp);

	//FIX:帧率不正常，有可能是音频里附带的图片，不对视频进行转码
	if(framerate_temp >= 500)
	{
		video_stream = -1;
	}
	*/
	return result;
}

StreamType wxFfmpegDecoder::GetStreamType(unsigned int streamIndex)
{
	if (m_formatCtx == NULL || streamIndex < 0 || streamIndex >= m_formatCtx->nb_streams)
	{
		return stUNKNOWN;
	}
	switch (m_formatCtx->streams[streamIndex]->codec->codec_type)
	{
	case AVMEDIA_TYPE_VIDEO:
		return stVIDEO;
	case AVMEDIA_TYPE_AUDIO:
		return stAUDIO;
	case AVMEDIA_TYPE_SUBTITLE:
		return stSUBTITLE;
	default:
		break;
	}
	return stUNKNOWN;
}

wxString wxFfmpegDecoder::GetCodecName(unsigned int streamIndex)
{
	if (m_formatCtx == NULL || streamIndex < 0 || streamIndex >= m_formatCtx->nb_streams)
	{
		return wxEmptyString;
	}
	char buf[256];
	avcodec_string(buf, sizeof(buf), m_formatCtx->streams[streamIndex]->codec, false);
	wxString name = wxString(buf, wxConvLocal).BeforeFirst(wxT(','));
	return name.Index(wxT(':')) > 0 ? name.AfterFirst(wxT(':')).Trim(false) : name;
}

int wxFfmpegDecoder::GetChannelNumber(unsigned int streamIndex)
{
	if (m_formatCtx == NULL || streamIndex < 0 || streamIndex >= m_formatCtx->nb_streams)
	{
		return -1;
	}
	return m_formatCtx->streams[streamIndex]->codec->channels;
}

int wxFfmpegDecoder::GetSampleRate(unsigned int streamIndex)
{
	if (m_formatCtx == NULL || streamIndex < 0 || streamIndex >= m_formatCtx->nb_streams)
	{
		return -1;
	}
	return m_formatCtx->streams[streamIndex]->codec->sample_rate;
}

int wxFfmpegDecoder::GetBitrate(unsigned int streamIndex)
{
	if (m_formatCtx == NULL || streamIndex < 0 || streamIndex >= m_formatCtx->nb_streams)
	{
		return -1;
	}
	return m_formatCtx->streams[streamIndex]->codec->bit_rate;
}

wxString wxFfmpegDecoder::GetBitrateString(unsigned int streamIndex)
{
	wxString bitrate;
	int bit = GetBitrate(streamIndex);
	if(bit == -1)
	{
		bitrate= wxString(_T("-1kbps"));
	}
	else
	{
		float bitrate_temp = ((float)bit)/1000;
		bitrate.Format(_T("%5.2fkbps"), bitrate_temp);
	}
	return bitrate;
}

double wxFfmpegDecoder::GetDuration()
{
	return m_formatCtx != NULL && m_formatCtx->duration != (int64_t)AV_NOPTS_VALUE ?
			((double)m_formatCtx->duration) / AV_TIME_BASE : -1;
}

wxString wxFfmpegDecoder::GetDurationString()
{
	wxString timelong;
	double dur = GetDuration();
	if(dur < 0)
	{
		timelong.Format(_T("%02d:%02d:%02d"), 0, 0, 0);
	}
	else
	{
		//duration turn to hh:mm:ss
		int tns, thh, tmm, tss;
		tns = (int)dur;
		thh  = tns / 3600;
		tmm  = (tns % 3600) / 60;
		tss  = (tns % 60);
		timelong.Format(_T("%02d:%02d:%02d"), thh, tmm, tss);
	}
	return timelong;
}

bool wxFfmpegDecoder::SetPosition(double pos, bool keyFrame) {
    if (m_formatCtx == NULL)
        return false;
	if (!m_frame && !BeginDecode())
		return false;
    int64_t timestamp = (int64_t) (pos * AV_TIME_BASE);
    if (m_formatCtx->start_time != (int64_t)AV_NOPTS_VALUE)
        timestamp += m_formatCtx->start_time;
    avcodec_flush_buffers(m_codecCtx);
    bool res = av_seek_frame(m_formatCtx, -1, timestamp,
    		keyFrame ? AVSEEK_FLAG_BACKWARD : AVSEEK_FLAG_ANY|AVSEEK_FLAG_BACKWARD) >= 0;
    avcodec_flush_buffers(m_codecCtx);
    return res;
}

double wxFfmpegDecoder::GetPosition() {
	AVStream *st = GetVideoStream();
	if (st == NULL)
		return -1;
	int64_t timestamp = st->cur_dts;
	if (timestamp == (int64_t)AV_NOPTS_VALUE)
		return -1;
	timestamp = av_rescale(timestamp, AV_TIME_BASE * (int64_t)st->time_base.num, st->time_base.den);
	if (m_formatCtx->start_time != (int64_t)AV_NOPTS_VALUE)
		timestamp -= m_formatCtx->start_time;
	return ((double)timestamp)/AV_TIME_BASE;
}

bool wxFfmpegDecoder::GetNextFrame(wxImage& frame) {
	if (!m_frame && !BeginDecode())
	{
		return false;
	}

	int frameFinished;
	AVPacket packet;
	while (av_read_frame(m_formatCtx, &packet) >=0)
	{
		// is this a packet from the video stream?
		if (packet.stream_index == m_videoStream)
		{
			// decode video frame
			avcodec_decode_video2(m_codecCtx, m_frame, &frameFinished, &packet);
			if (frameFinished)
			{
				SwsContext* imgConvertCtx = sws_getContext(m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
						m_width, m_height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
				if (imgConvertCtx == NULL)
				{
					av_free_packet(&packet);
					return false;
				}

				frame = wxImage(m_width, m_height);
				uint8_t *rgbSrc[3] = { frame.GetData(), NULL, NULL };
				int rgbStride[3] = { 3 * m_width, 0, 0 };
				sws_scale(imgConvertCtx, m_frame->data, m_frame->linesize, 0, m_codecCtx->height, rgbSrc, rgbStride);
				av_free_packet(&packet);
				sws_freeContext(imgConvertCtx);
				return true;
			}
		}
		// free the packet that was allocated by av_read_frame
		av_free_packet(&packet);
	}
	return false;
}

/** Returns a comma separated list of short names for the format. */
wxString wxFfmpegDecoder::GetFormatName()
{
	if (m_formatCtx == NULL || m_formatCtx->iformat == NULL || m_formatCtx->iformat->name == NULL)
		return wxT("");
	wxString name = wxString(m_formatCtx->iformat->name, wxConvLocal);
	if (name.Find(wxT("mp4")) >= 0)
		return wxT("mp4");
	return name;
}

/** Returns video codec tag (fourcc) */
wxString wxFfmpegDecoder::GetCodecTag(unsigned int streamIndex)
{
	if (m_formatCtx == NULL || streamIndex < 0 || streamIndex >= m_formatCtx->nb_streams)
	{
		return wxEmptyString;
	}
	AVStream *st = m_formatCtx->streams[streamIndex];
	if (st->codec == NULL || st->codec->codec_tag == 0)
	{
		return wxEmptyString;
	}
	char buf[32];
	if (av_get_codec_tag_string(buf, sizeof(buf), st->codec->codec_tag) <= 0)
	{
		return wxEmptyString;
	}
	return wxString(buf, wxConvLocal);
}

/** Returns time base for video codec (tbc). */
float wxFfmpegDecoder::GetCodecTimeBase()
{
	if (m_codecCtx == NULL || !m_codecCtx->time_base.den || !m_codecCtx->time_base.den)
		return -1;
	return 1 / av_q2d(m_codecCtx->time_base);
}
