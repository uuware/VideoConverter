/////////////////////////////////////////////////////////////////////////////
// Name:        dragimag.cpp
// Purpose:     wxDragImage sample
// Author:      Julian Smart
// Modified by:
// Created:     28/2/2000
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/log.h>
#include "wx/image.h"
#include "wx/graphics.h"

// Under Windows, change this to 1
// to use wxGenericDragImage

#define wxUSE_GENERIC_DRAGIMAGE 1

#if wxUSE_GENERIC_DRAGIMAGE
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#else
#include "wx/dragimag.h"
#endif

#include "ccutils.h"
#include "ccctl.h"
#include "main.h"
#include "ffmpegutils.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
    #include "dragicon.xpm"
#endif



//检查输入文件信息
//Check input file information
BOOL TaskinfoDlg_CheckInputMedia(wxString url, wxImage& frame)
{
	wxString input_protocol, input_format, wxh, decoder_name,
		decoder_type, bitrate, extention, framerate, timelong, decoder_name_au, sample_rate_au, channels_au;
	float framerate_temp, timelong_temp, bitrate_temp;

	int ret;
	int video_stream = -1;
	int audio_stream = -1;
	AVFormatContext	*pFormatCtx = NULL;
	AVCodecContext	*pCodecCtx = NULL, *pCodecCtx_au = NULL;
	AVCodec			*pCodec = NULL, *pCodec_au = NULL;

	AVFrame *m_frame;
	m_frame = av_frame_alloc();
	if (!m_frame)
	{
		return FALSE;
	}

	av_register_all();
	//avformat_network_init();
	pFormatCtx = avformat_alloc_context();

	if((ret = avformat_open_input(&pFormatCtx, url.mb_str(), NULL, NULL)) < 0)
	{
		MessageBox(NULL, _T("Couldn't open input stream."), _T(""), 0);
		return FALSE;
	}

	if((ret = avformat_find_stream_info(pFormatCtx, NULL)) < 0)
	{
		MessageBox(NULL, _T("Couldn't find stream information."), _T(""), 0);
		avformat_close_input(&pFormatCtx);
		return FALSE;
	}

	for(unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_stream = i;
		}
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_stream = i;
		}
	}

	if(audio_stream == -1 && video_stream == -1)
	{
		MessageBox(NULL, _T("Didn't find a video/audio stream."), _T(""), 0);
		avformat_close_input(&pFormatCtx);
		return FALSE;
	}

	//视频解码参数，有视频的时候设置
	if(video_stream != -1)
	{
		pCodecCtx = pFormatCtx->streams[video_stream]->codec;
		pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
		if(pCodec == NULL)
		{
			MessageBox(NULL, _T("Video codec not found."), _T(""), 0);
			avformat_close_input(&pFormatCtx);
			return FALSE;
		}
		if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
		{
			MessageBox(NULL, _T("Could not open video codec."), _T(""), 0);
			avformat_close_input(&pFormatCtx);
			return FALSE;
		}

		wxh.Format(_T("%d x %d"), pCodecCtx->width, pCodecCtx->height);
		decoder_name = wxString(pCodecCtx->codec->long_name);

		//帧率显示还有问题
		framerate_temp = (pFormatCtx->streams[video_stream]->r_frame_rate.num)/(pFormatCtx->streams[video_stream]->r_frame_rate.den);
		framerate.Format(_T("%5.2ffps"), framerate_temp);

		//FIX:帧率不正常，有可能是音频里附带的图片，不对视频进行转码
		if(framerate_temp >= 500)
		{
			video_stream = -1;
		}

		int m_width = pCodecCtx->width;
		int m_height = pCodecCtx->height;
		int frameFinished;
		AVPacket packet;
		while (av_read_frame(pFormatCtx, &packet) >=0) {
			// is this a packet from the video stream?
			if (packet.stream_index == video_stream) {
				  // decode video frame
				  avcodec_decode_video2(pCodecCtx, m_frame, &frameFinished, &packet);
				  if (frameFinished) {
						SwsContext* imgConvertCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
									m_width, m_height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
						if (imgConvertCtx != NULL)
						{
							frame = wxImage(m_width, m_height);
							uint8_t *rgbSrc[3] = { frame.GetData(), NULL, NULL };
							int rgbStride[3] = { 3 * m_width, 0, 0 };
							sws_scale(imgConvertCtx, m_frame->data, m_frame->linesize, 0, pCodecCtx->height, rgbSrc, rgbStride);

							sws_freeContext(imgConvertCtx);
							break;
						}
				  }
			}
			// free the packet that was allocated by av_read_frame
			av_free_packet(&packet);
		}
	}

	//音频解码参数，有音频的时候设置
	if(audio_stream != -1)
	{
		pCodecCtx_au = pFormatCtx->streams[audio_stream]->codec;
		pCodec_au = avcodec_find_decoder(pCodecCtx_au->codec_id);
		if(pCodec_au == NULL)
		{
			MessageBox(NULL, _T("Audio codec not found."), _T(""), 0);
			if(pCodecCtx != NULL)
			{
				avcodec_close(pCodecCtx);
			}
			avformat_close_input(&pFormatCtx);
			return FALSE;
		}
		if(avcodec_open2(pCodecCtx_au, pCodec_au, NULL) < 0)
		{
			MessageBox(NULL, _T("Could not open audio codec."), _T(""), 0);
			if(pCodecCtx != NULL)
			{
				avcodec_close(pCodecCtx);
			}
			avformat_close_input(&pFormatCtx);
			return FALSE;
		}

		decoder_name_au.Format(_T("%s"), pCodecCtx_au->codec->long_name);
		sample_rate_au.Format(_T("%d"), pCodecCtx_au->sample_rate);
		channels_au.Format(_T("%d"), pCodecCtx_au->channels);
	}

	//显示成以k为单位
	bitrate_temp = ((float)(pFormatCtx->bit_rate))/1000;
	bitrate.Format(_T("%5.2fkbps") ,bitrate_temp);
	//duration是以微秒为单位
	timelong_temp = (pFormatCtx->duration)/1000000;
	//转换成hh:mm:ss形式
	int tns, thh, tmm, tss;
	tns  = (pFormatCtx->duration)/1000000;
	thh  = tns / 3600;
	tmm  = (tns % 3600) / 60;
	tss  = (tns % 60);
	timelong.Format(_T("%02d:%02d:%02d"), thh, tmm, tss);
	//输入文件的封装格式------
	input_format.Format("%s", pFormatCtx->iformat->long_name);

	if(video_stream != -1)
	{
		avcodec_close(pCodecCtx);
	}
	if(audio_stream != -1)
	{
		avcodec_close(pCodecCtx_au);
	}
	avformat_close_input(&pFormatCtx);
	return TRUE;
}

int mainYY(wxString sPath, wxString sDesPath)
{
	wxArrayString arrIn;
	wxArrayString arrOut;
	wxArrayString arrA;
	wxArrayString arrV;
	wxFfmpegDecoder::GetFormatList(arrIn, arrOut, arrA, arrV);
	for(int n = 0; n < arrIn.size(); n++)
	{
		wxString item = arrIn[n];
		wxLogDebug("arrIn:%s", item);
	}
	for(int n = 0; n < arrOut.size(); n++)
	{
		wxString item = arrOut[n];
		wxLogDebug("arrOut:%s", item);
	}
	for(int n = 0; n < arrA.size(); n++)
	{
		wxString item = arrA[n];
		wxLogDebug("arrA:%s", item);
	}
	for(int n = 0; n < arrV.size(); n++)
	{
		wxString item = arrV[n];
		wxLogDebug("arrV:%s", item);
	}

	avcodec_register_all();
#if CONFIG_AVDEVICE
    avdevice_register_all();
#endif
    avfilter_register_all();
    av_register_all();
    avformat_network_init();
	return 0;
}


/**
* setup the filter graph, init the ifilter(buffersrc_ctx) and ofilter(buffersink_ctx).
* AVFilterContext* buffersrc_ctx, to where put decoded frame
* AVFilterContext* buffersink_ctx, from where get filtered frame
*/
int configure_filtergraph(
    /*input*/
    AVStream* ist, AVStream* ost, AVCodec* enc,
    /*output*/
    AVFilterContext*& buffersrc_ctx, AVFilterContext*& buffersink_ctx)
{
    int ret = 0;
    
    AVFilterGraph* graph = avfilter_graph_alloc();
    assert(graph);
    
    // inputs/outputs build by avfilter_graph_parse2
    AVFilterInOut* inputs = NULL;
    AVFilterInOut* outputs = NULL;
    // init filter graph
    if (true) {
        // init simple filters
        const char* anull_filters_desc = "anull";
        // ost->sws_flags
        graph->scale_sws_opts = av_strdup("flags=0x4");
        av_opt_set(graph, "aresample_swr_opts", "", 0);
        graph->resample_lavr_opts = av_strdup("");
        // build filter graph
        ret = avfilter_graph_parse2(graph, anull_filters_desc, &inputs, &outputs);
        assert(ret >= 0);
        // simple filter must have only one input and output.
        assert(inputs && !inputs->next);
        assert(outputs && !outputs->next);
    }
    // config input filter
    if (true) {
        // first_filter is "anull"
        AVFilterContext* first_filter = inputs->filter_ctx;
        int pad_idx = inputs->pad_idx;
        
        // get abuffer audio filter
        AVFilter* abuffersrc = avfilter_get_by_name("abuffer");
        // init abuffer audio filter
        char args[512]; memset(args, 0, sizeof(args));
        // time_base=1/44100:sample_rate=44100:sample_fmt=fltp:channel_layout=0x3
        _snprintf(args, sizeof(args), 
            "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%I64x",
            1, ist->codec->sample_rate, ist->codec->sample_rate,
            av_get_sample_fmt_name(ist->codec->sample_fmt), ist->codec->channel_layout);
        ret = avfilter_graph_create_filter(&buffersrc_ctx, abuffersrc, "abuffer-filter", args, NULL, graph);
        assert(ret >= 0);
        // link src "abuffer" to dst "anull"
        // the data flow: abuffer ===> anull
        ret = avfilter_link(buffersrc_ctx, 0, first_filter, pad_idx);
        assert(ret >= 0);
        
        avfilter_inout_free(&inputs);
    }
    // config output filter
    if (true) {
        // last_filter is "anull"
        AVFilterContext* last_filter = outputs->filter_ctx;
        int pad_idx = outputs->pad_idx;
        
        // init ffabuffersink audio filter
        // link it later.
        AVABufferSinkParams* params = av_abuffersink_params_alloc();
        params->all_channel_counts = 1;
        AVFilter* abuffersink = avfilter_get_by_name("ffabuffersink");
        ret = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, "abuffersink-filter", NULL, params, graph);
        assert(ret >= 0);
        av_free(params);
        
        // init the encoder context channel_layout.
        if (ost->codec->channels && !ost->codec->channel_layout) {
            ost->codec->channel_layout = av_get_default_channel_layout(ost->codec->channels);
            
            const char* sample_fmts = av_get_sample_fmt_name(*enc->sample_fmts);
            char args[512]; memset(args, 0, sizeof(args));
            _snprintf(args, sizeof(args), 
                "sample_fmts=%s:sample_rates=%d:channel_layouts=0x%I64x:",
                sample_fmts, ost->codec->sample_rate, ost->codec->channel_layout);
            
            AVFilterContext* aformat_ctx = NULL;
            AVFilter* aformat = avfilter_get_by_name("aformat");
            ret = avfilter_graph_create_filter(&aformat_ctx, aformat, "aformat-filter", args, NULL, graph);
            assert(ret >= 0);
            
            // the data flow: anull ===> aformat
            ret = avfilter_link(last_filter, pad_idx, aformat_ctx, 0);
            assert(ret >= 0);
            // now, "aformat" is the last filter
            last_filter = aformat_ctx;
            pad_idx = 0;
        }
        
        // link the abuffersink to the last filer
        // the data flow: aformat ===> abuffersink
        // full data flow: anull ===> aformat ===> abuffersink
        ret = avfilter_link(last_filter, pad_idx, buffersink_ctx, 0);
        assert(ret >= 0);
        
        avfilter_inout_free(&outputs);
    }
    ret = avfilter_graph_config(graph, NULL);
    assert(ret >= 0);
    
    return ret;
}




static AVFormatContext *ifmt_ctx;
static AVFormatContext *ofmt_ctx;
typedef struct FilteringContext {
	AVFilterContext *buffersink_ctx;
	AVFilterContext *buffersrc_ctx;
	AVFilterGraph *filter_graph;
} FilteringContext;
static FilteringContext *filter_ctx;

static int open_input_file(const char *filename)
{
	int ret;
	unsigned int i;

	ifmt_ctx = NULL;
	if ((ret = avformat_open_input(&ifmt_ctx, filename, NULL, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
		return ret;
	}

	if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
		return ret;
	}

	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		AVStream *stream;
		AVCodecContext *codec_ctx;
		stream = ifmt_ctx->streams[i];
		codec_ctx = stream->codec;
		/* Reencode video & audio and remux subtitles etc. */
		if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
			|| codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			/* Open decoder */
			ret = avcodec_open2(codec_ctx,
				avcodec_find_decoder(codec_ctx->codec_id), NULL);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
				return ret;
			}
		}
	}

	av_dump_format(ifmt_ctx, 0, filename, 0);
	return 0;
}

static int open_output_file(const char *filename)
{
	AVStream *out_stream;
	AVStream *in_stream;
	AVCodecContext *dec_ctx, *enc_ctx;
	AVCodec *encoder;
	int ret;
	unsigned int i;

	ofmt_ctx = NULL;
	avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, filename);
	if (!ofmt_ctx) {
		av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
		return AVERROR_UNKNOWN;
	}

	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		out_stream = avformat_new_stream(ofmt_ctx, NULL);
		if (!out_stream) {
			av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
			return AVERROR_UNKNOWN;
		}

		in_stream = ifmt_ctx->streams[i];
		dec_ctx = in_stream->codec;
		enc_ctx = out_stream->codec;

		if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) 
		{
			encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
			if (!encoder) {
				av_log(NULL, AV_LOG_FATAL, "Neccessary encoder not found\n");
				return AVERROR_INVALIDDATA;
			}

			enc_ctx->height = dec_ctx->height;
			enc_ctx->width = dec_ctx->width;
			enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
				
			enc_ctx->pix_fmt = encoder->pix_fmts[0];
				
			enc_ctx->time_base = dec_ctx->time_base;

			enc_ctx->me_range = 16;
			enc_ctx->max_qdiff = 4;
			enc_ctx->qmin = 10;
			enc_ctx->qmax = 51;
			enc_ctx->qcompress = 0.6;
			enc_ctx->refs = 3;
			enc_ctx->bit_rate = 500000;
			
			ret = avcodec_open2(enc_ctx, encoder, NULL);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
				return ret;
			}
		}
		else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
			av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
			return AVERROR_INVALIDDATA;
		}
		else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			encoder = avcodec_find_encoder(AV_CODEC_ID_PCM_ALAW);
			enc_ctx->sample_rate = dec_ctx->sample_rate;
			enc_ctx->channel_layout = dec_ctx->channel_layout;
			enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
			enc_ctx->sample_fmt = encoder->sample_fmts[0];
			AVRational ar = { 1, enc_ctx->sample_rate };
			enc_ctx->time_base = ar;

			ret = avcodec_open2(enc_ctx, encoder, NULL);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
				return ret;
			}
		}
		else {
			ret = avcodec_copy_context(ofmt_ctx->streams[i]->codec,
				ifmt_ctx->streams[i]->codec);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Copying stream context failed\n");
				return ret;
			}
		}

		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			enc_ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;

	}
	av_dump_format(ofmt_ctx, 0, filename, 1);

	if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
		ret = avio_open(&ofmt_ctx->pb, filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'", filename);
			return ret;
		}
	}

	/* init muxer, write output file header */
	ret = avformat_write_header(ofmt_ctx, NULL);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
		return ret;
	}

	return 0;
}

static int init_filter(FilteringContext* fctx, AVCodecContext *dec_ctx,
	AVCodecContext *enc_ctx, const char *filter_spec)
{
	char args[512];
	int ret = 0;
	AVFilter *buffersrc = NULL;
	AVFilter *buffersink = NULL;
	AVFilterContext *buffersrc_ctx = NULL;
	AVFilterContext *buffersink_ctx = NULL;
	AVFilterInOut *outputs = avfilter_inout_alloc();
	AVFilterInOut *inputs = avfilter_inout_alloc();
	AVFilterGraph *filter_graph = avfilter_graph_alloc();

	if (!outputs || !inputs || !filter_graph) {
		ret = AVERROR(ENOMEM);
		goto end;
	}

	if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
		buffersrc = avfilter_get_by_name("buffer");
		buffersink = avfilter_get_by_name("buffersink");
		if (!buffersrc || !buffersink) {
			av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
			ret = AVERROR_UNKNOWN;
			goto end;
		}

		_snprintf(args, sizeof(args),
			"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
			dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
			dec_ctx->time_base.num, dec_ctx->time_base.den,
			dec_ctx->sample_aspect_ratio.num,
			dec_ctx->sample_aspect_ratio.den);

		ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
			args, NULL, filter_graph);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
			goto end;
		}

		ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
			NULL, NULL, filter_graph);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
			goto end;
		}

		ret = av_opt_set_bin(buffersink_ctx, "pix_fmts",
			(uint8_t*)&enc_ctx->pix_fmt, sizeof(enc_ctx->pix_fmt),
			AV_OPT_SEARCH_CHILDREN);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
			goto end;
		}
	}
	else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
		buffersrc = avfilter_get_by_name("abuffer");
		buffersink = avfilter_get_by_name("abuffersink");
		if (!buffersrc || !buffersink) {
			av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
			ret = AVERROR_UNKNOWN;
			goto end;
		}

		if (!dec_ctx->channel_layout)
			dec_ctx->channel_layout = av_get_default_channel_layout(dec_ctx->channels);
		_snprintf(args, sizeof(args),
			"time_base=%u/%u:sample_rate=%u:sample_fmt=%s:channel_layout=0x%I64x",
			dec_ctx->time_base.num, dec_ctx->time_base.den, dec_ctx->sample_rate,
			av_get_sample_fmt_name(dec_ctx->sample_fmt),
			dec_ctx->channel_layout);

		ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
			args, NULL, filter_graph);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
			goto end;
		}

		ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
			NULL, NULL, filter_graph);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
			goto end;
		}

		ret = av_opt_set_bin(buffersink_ctx, "sample_fmts",
			(uint8_t*)&enc_ctx->sample_fmt, sizeof(enc_ctx->sample_fmt),
			AV_OPT_SEARCH_CHILDREN);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
			goto end;
		}

		ret = av_opt_set_bin(buffersink_ctx, "channel_layouts",
			(uint8_t*)&enc_ctx->channel_layout,
			sizeof(enc_ctx->channel_layout), AV_OPT_SEARCH_CHILDREN);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
			goto end;
		}

		ret = av_opt_set_bin(buffersink_ctx, "sample_rates",
			(uint8_t*)&enc_ctx->sample_rate, sizeof(enc_ctx->sample_rate),
			AV_OPT_SEARCH_CHILDREN);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
			goto end;
		}
	}
	else {
		ret = AVERROR_UNKNOWN;
		goto end;
	}

	/* Endpoints for the filter graph. */
	outputs->name = av_strdup("in");
	outputs->filter_ctx = buffersrc_ctx;
	outputs->pad_idx = 0;
	outputs->next = NULL;

	inputs->name = av_strdup("out");
	inputs->filter_ctx = buffersink_ctx;
	inputs->pad_idx = 0;
	inputs->next = NULL;

	if (!outputs->name || !inputs->name) {
		ret = AVERROR(ENOMEM);
		goto end;
	}

	if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_spec,
		&inputs, &outputs, NULL)) < 0)
	{
		wxLogDebug("avfilter_graph_parse_ptr:%d", ret);
		goto end;
	}

	if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
	{
		wxLogDebug("avfilter_graph_config:%d", ret);
		if((ret = avfilter_graph_parse2(filter_graph, filter_spec, &inputs, &outputs)) < 0)
		{
			wxLogDebug("avfilter_graph_parse2:%d", ret);
			goto end;
		}

		if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
		{
			wxLogDebug("avfilter_graph_config:%d", ret);
			goto end;
		}
	}

	/* Fill FilteringContext */
	fctx->buffersrc_ctx = buffersrc_ctx;
	fctx->buffersink_ctx = buffersink_ctx;
	fctx->filter_graph = filter_graph;

end:
	avfilter_inout_free(&inputs);
	avfilter_inout_free(&outputs);

	return ret;
}

static int init_filters(void)
{
	const char *filter_spec;
	unsigned int i;
	int ret;
	filter_ctx = (FilteringContext*)av_malloc_array(ifmt_ctx->nb_streams, sizeof(*filter_ctx));
	if (!filter_ctx)
		return AVERROR(ENOMEM);

	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		filter_ctx[i].buffersrc_ctx = NULL;
		filter_ctx[i].buffersink_ctx = NULL;
		filter_ctx[i].filter_graph = NULL;
		if (!(ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO
			|| ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO))
			continue;


		if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			filter_spec = "null"; /* passthrough (dummy) filter for video */
		else
			filter_spec = "anull"; /* passthrough (dummy) filter for audio */
		ret = init_filter(&filter_ctx[i], ifmt_ctx->streams[i]->codec,
			ofmt_ctx->streams[i]->codec, filter_spec);
		if (ret)
			return ret;
	}
	return 0;
}

static int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame) {
	int ret;
	int got_frame_local;
	AVPacket enc_pkt;
	int(*enc_func)(AVCodecContext *, AVPacket *, const AVFrame *, int *) =
		(ifmt_ctx->streams[stream_index]->codec->codec_type ==
		AVMEDIA_TYPE_VIDEO) ? avcodec_encode_video2 : avcodec_encode_audio2;

	if (!got_frame)
		got_frame = &got_frame_local;

	av_log(NULL, AV_LOG_INFO, "Encoding frame\n");
	/* encode filtered frame */
	enc_pkt.data = NULL;
	enc_pkt.size = 0;
	av_init_packet(&enc_pkt);
	ret = enc_func(ofmt_ctx->streams[stream_index]->codec, &enc_pkt,
		filt_frame, got_frame);
	av_frame_free(&filt_frame);
	if (ret < 0)
		return ret;
	if (!(*got_frame))
		return 0;

	/* prepare packet for muxing */
	enc_pkt.stream_index = stream_index;
	av_packet_rescale_ts(&enc_pkt,
		ofmt_ctx->streams[stream_index]->codec->time_base,
		ofmt_ctx->streams[stream_index]->time_base);

	av_log(NULL, AV_LOG_DEBUG, "Muxing frame\n");
	/* mux encoded frame */
	ret = av_interleaved_write_frame(ofmt_ctx, &enc_pkt);
	return ret;
}

static int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index)
{
	int ret;
	AVFrame *filt_frame;

	av_log(NULL, AV_LOG_INFO, "Pushing decoded frame to filters\n");
	/* push the decoded frame into the filtergraph */
	ret = av_buffersrc_add_frame_flags(filter_ctx[stream_index].buffersrc_ctx,
		frame, 0);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
		return ret;
	}

	/* pull filtered frames from the filtergraph */
	while (1) {
		filt_frame = av_frame_alloc();
		if (!filt_frame) {
			ret = AVERROR(ENOMEM);
			break;
		}
		av_log(NULL, AV_LOG_INFO, "Pulling filtered frame from filters\n");
		ret = av_buffersink_get_frame(filter_ctx[stream_index].buffersink_ctx,
			filt_frame);
		if (ret < 0) {
			/* if no more frames for output - returns AVERROR(EAGAIN)
			* if flushed and no more frames for output - returns AVERROR_EOF
			* rewrite retcode to 0 to show it as normal procedure completion
			*/
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				ret = 0;
			av_frame_free(&filt_frame);
			break;
		}

		filt_frame->pict_type = AV_PICTURE_TYPE_NONE;
		ret = encode_write_frame(filt_frame, stream_index, NULL);
		if (ret < 0)
			break;
	}

	return ret;
}

static int flush_encoder(unsigned int stream_index)
{
	int ret;
	int got_frame;

	if (!(ofmt_ctx->streams[stream_index]->codec->codec->capabilities &
		CODEC_CAP_DELAY))
		return 0;

	while (1) {
		av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
		ret = encode_write_frame(NULL, stream_index, &got_frame);
		if (ret < 0)
			break;
		if (!got_frame)
			return 0;
	}
	return ret;
}

int mainXX(wxString sPath, wxString sDesPath)
{
	int ret;
	AVPacket packet; //= { .data = NULL, .size = 0 };
	packet.data = NULL;
	packet.size = 0;
	AVFrame *frame = NULL;
	enum AVMediaType type;
	unsigned int stream_index;
	unsigned int i;
	int got_frame;
	int(*dec_func)(AVCodecContext *, AVFrame *, int *, const AVPacket *);
	
	av_register_all();
	avfilter_register_all();

	wxString sPathName = ccUtils::GetFilename(sPath);
	wxString sDesName = ccUtils::GetComparePath(sDesPath, sPathName);
	sDesName += ".avi";
	if ((ret = open_input_file(sPath.mb_str())) < 0)
		goto end;
	if ((ret = open_output_file(sDesName.mb_str())) < 0)
		goto end;
	if ((ret = init_filters()) < 0)
		goto end;

	/* read all packets */
	while (1) {
		if ((ret = av_read_frame(ifmt_ctx, &packet)) < 0)
			break;
		
		stream_index = packet.stream_index;
		type = ifmt_ctx->streams[packet.stream_index]->codec->codec_type;
		
		av_log(NULL, AV_LOG_DEBUG, "Demuxer gave frame of stream_index %u\n",
			stream_index);

		if (filter_ctx[stream_index].filter_graph) {
			av_log(NULL, AV_LOG_DEBUG, "Going to reencode&filter the frame\n");
			frame = av_frame_alloc();
			if (!frame) {
				ret = AVERROR(ENOMEM);
				break;
			}
			av_packet_rescale_ts(&packet,
				ifmt_ctx->streams[stream_index]->time_base,
				ifmt_ctx->streams[stream_index]->codec->time_base);
			dec_func = (type == AVMEDIA_TYPE_VIDEO) ? avcodec_decode_video2 :
				avcodec_decode_audio4;
			ret = dec_func(ifmt_ctx->streams[stream_index]->codec, frame,
				&got_frame, &packet);
			if (ret < 0) {
				av_frame_free(&frame);
				av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
				break;
			}

			if (got_frame) {
				frame->pts = av_frame_get_best_effort_timestamp(frame);
				ret = filter_encode_write_frame(frame, stream_index);
				av_frame_free(&frame);
				if (ret < 0)
					goto end;
			}
			else {
				av_frame_free(&frame);
			}
		}
		else {
			/* remux this frame without reencoding */
			av_packet_rescale_ts(&packet,
				ifmt_ctx->streams[stream_index]->time_base,
				ofmt_ctx->streams[stream_index]->time_base);

			ret = av_interleaved_write_frame(ofmt_ctx, &packet);
			if (ret < 0)
				goto end;
		}
		av_free_packet(&packet);
	}

	/* flush filters and encoders */
	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		/* flush filter */
		if (!filter_ctx[i].filter_graph)
			continue;
		ret = filter_encode_write_frame(NULL, i);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Flushing filter failed\n");
			goto end;
		}

		/* flush encoder */
		ret = flush_encoder(i);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Flushing encoder failed\n");
			goto end;
		}
	}

	av_write_trailer(ofmt_ctx);
end:
	if(packet.data != NULL)
	{
		av_free_packet(&packet);
	}
	if(frame != NULL)
	{
		av_frame_free(&frame);
	}
	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		avcodec_close(ifmt_ctx->streams[i]->codec);
		if (ofmt_ctx && ofmt_ctx->nb_streams > i && ofmt_ctx->streams[i] && ofmt_ctx->streams[i]->codec)
			avcodec_close(ofmt_ctx->streams[i]->codec);
		if (filter_ctx && filter_ctx[i].filter_graph)
			avfilter_graph_free(&filter_ctx[i].filter_graph);
	}
	av_free(filter_ctx);
	avformat_close_input(&ifmt_ctx);
	if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
		avio_closep(&ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);

	//if (ret < 0)
	//	av_log(NULL, AV_LOG_ERROR, "Error occurred: %s\n"); //av_err2str(ret));

	return ret ? 1 : 0;
}






//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)

#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)

int thread_exit=0;
int thread_pause=0;

int sfp_refresh_thread(void *opaque){
	thread_exit=0;
	thread_pause=0;

	while (!thread_exit) {
		if(!thread_pause){
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);
		}
		SDL_Delay(40);
	}
	thread_exit=0;
	thread_pause=0;
	//Break
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);

	return 0;
}


int mainX(void* handle, const wxString& docPath)
{

	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame,*pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int ret, got_picture;

	//------------SDL----------------
	int screen_w,screen_h;
	SDL_Window *screen; 
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Rect sdlRect;
	SDL_Thread *video_tid;
	SDL_Event event;

	struct SwsContext *img_convert_ctx;

	//char filepath[] = (char*)docPath;

	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();

	if(avformat_open_input(&pFormatCtx, docPath.mb_str(), NULL, NULL)!=0){
		printf("Couldn't open input stream.\n");
		(void)wxMessageBox( wxT("Couldn't open input stream."), wxT("Demo"), wxICON_INFORMATION | wxOK );
		return -1;
	}
	if(avformat_find_stream_info(pFormatCtx, NULL)<0){
		printf("Couldn't find stream information.\n");
		return -1;
	}
	videoindex=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++) 
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
			videoindex=i;
			break;
		}
	if(videoindex==-1){
		printf("Didn't find a video stream.\n");
		return -1;
	}
	pCodecCtx=pFormatCtx->streams[videoindex]->codec;
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL){
		printf("Codec not found.\n");
		return -1;
	}
	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
		printf("Could not open codec.\n");
		return -1;
	}
	pFrame=av_frame_alloc();
	pFrameYUV=av_frame_alloc();
	out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

	//Output Info-----------------------------
	printf("---------------- File Information ---------------\n");
	//av_dump_format(pFormatCtx,0,filepath,0);
	printf("-------------------------------------------------\n");
	
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 
	

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {  
		printf( "Could not initialize SDL - %s\n", SDL_GetError()); 
		return -1;
	} 
	//SDL 2.0 Support for multiple windows
	screen_w = pCodecCtx->width*2;
	screen_h = pCodecCtx->height*2;
	screen = SDL_CreateWindowFrom(handle);
	//screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	//	screen_w, screen_h,SDL_WINDOW_OPENGL);

	if(!screen) {  
		printf("SDL: could not create window - exiting:%s\n",SDL_GetError());  
		return -1;
	}
	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);  
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,pCodecCtx->width,pCodecCtx->height);  

	sdlRect.x=0;
	sdlRect.y=0;
	sdlRect.w=screen_w;
	sdlRect.h=screen_h;

	packet=(AVPacket *)av_malloc(sizeof(AVPacket));

	video_tid = SDL_CreateThread(sfp_refresh_thread,NULL,NULL);
	//------------SDL End------------
	//Event Loop
	
	for (;;) {
		//Wait
		SDL_WaitEvent(&event);
		if(event.type==SFM_REFRESH_EVENT){
			//------------------------------
			if(av_read_frame(pFormatCtx, packet)>=0){
				if(packet->stream_index==videoindex){
					ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
					if(ret < 0){
						printf("Decode Error.\n");
						return -1;
					}
					if(got_picture){
						sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
						//SDL---------------------------
						SDL_UpdateTexture( sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0] );  
						SDL_RenderClear( sdlRenderer );  
						//SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );  
						SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, NULL);  
						SDL_RenderPresent( sdlRenderer );  
						//SDL End-----------------------
					}
				}
				av_free_packet(packet);
			}else{
				//Exit Thread
				thread_exit=1;
			}
		}else if(event.type==SDL_KEYDOWN){
			//Pause
			if(event.key.keysym.sym==SDLK_SPACE)
				thread_pause=!thread_pause;
		}else if(event.type==SDL_QUIT){
			thread_exit=1;
		}else if(event.type==SFM_BREAK_EVENT){
			break;
		}

	}

	sws_freeContext(img_convert_ctx);

	SDL_Quit();
	//--------------
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}




// main program

// VRow
VRow::VRow( const wxString& path, VList *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
	: wxPanel( parent, id, pos, size, wxNO_BORDER )
{
	m_flag = 0;
	SetBackgroundColour(wxColour("#cdcfd2"));
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	m_owner = parent;
	m_vpath = path;

    wxStaticBox *sb = new wxStaticBox( this, wxID_ANY, m_vpath,
                           wxDefaultPosition, wxDefaultSize );
	this->Bind(wxEVT_PAINT, &VRow::OnPaint, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, &VRow::OnEraseBackground, this);
	this->Bind(wxEVT_ENTER_WINDOW, &VRow::OnEnterEvent, this);
	this->Bind(wxEVT_LEAVE_WINDOW, &VRow::OnLeaveEvent, this);
	this->Bind(wxEVT_LEFT_DOWN, &VRow::OnLeftClickEvent, this);
	this->Bind(wxEVT_LEFT_UP, &VRow::OnLeftClickEvent, this);
}

VRow::~VRow()
{
}

void VRow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	bool isMouseon = CC_BIT_IS(m_flag, CC_VROW_FLAG_MOUSEON);
	bool isSelected = IsSelected();
	wxSize size = this->GetSize();
	wxRect rect(2, 2, size.x - 4, size.y - 4);
	wxPaintDC paintDC(this);
    PrepareDC(paintDC);

	paintDC.SetBackground(this->GetParent()->GetBackgroundColour());
	paintDC.Clear();

	if(isSelected || isMouseon)
	{
		paintDC.SetPen(wxPen(*wxBLUE, 4));
		paintDC.SetBrush(*wxWHITE);
	}
	else
	{
		paintDC.SetPen(*wxWHITE);
		paintDC.SetBrush(this->GetBackgroundColour());
	}

	paintDC.DrawRoundedRectangle(rect, 5);
	paintDC.SetBrush(wxNullBrush);
	paintDC.SetPen(wxNullPen);
}

void VRow::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void VRow::OnEnterEvent(wxMouseEvent& event)
{
	CC_BIT_SET(m_flag, CC_VROW_FLAG_MOUSEON, true);
	this->Refresh();
}

void VRow::OnLeaveEvent(wxMouseEvent& event)
{
	CC_BIT_SET(m_flag, CC_VROW_FLAG_MOUSEON, false);
	this->Refresh();
}

void VRow::OnLeftClickEvent( wxMouseEvent &event )
{
	if(event.LeftDown())
	{
		bool isSelected = this->IsSelected();
		SetSelected(!isSelected);
		this->Refresh();
		m_owner->SetSelected(this);
	}
	m_owner->SetFocus();
}

// VList
VList::VList( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
	: wxScrolledWindow( parent, id, pos, size, wxVSCROLL )
{
	this->SetScrollRate(-1, 10);
	SetBackgroundColour(parent->GetBackgroundColour());
	//SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	m_owner = parent;
	m_selectedRow = NULL;

	this->Bind(wxEVT_SIZE, &VList::OnResize, this);
}

VList::~VList()
{
}

void VList::AddRow(VRow* vRow)
{
	m_List.Append(vRow);
	ReLayout();
}

void VList::RemoveRow(VRow* vRow)
{
	m_List.remove(vRow);
	ReLayout();
	this->RemoveChild(vRow);
	delete vRow;
}

void VList::RemoveRowAll()
{
	wxList::iterator iter;
	for (iter = m_List.begin(); iter != m_List.end(); ++iter)
	{
		VRow* vRow = (VRow*)*iter;
		this->RemoveChild(vRow);
		delete vRow;
	}
	m_List.clear();
}

int VList::GetRowCount()
{
	return m_List.size();
}

wxString VList::GetPath(int index)
{
	if(index < 0 || index > GetRowCount())
	{
		return wxEmptyString;
	}
	VRow* vRow = (VRow*)m_List[index];
	return vRow->GetPath();
}

void VList::ReLayout()
{
	bool hasBarY = this->HasScrollbar(wxVERTICAL);
	int barW = hasBarY ? ccUtils::GetScrollBarWidth() : 0;
	int nTop = 3;
	wxSize size = this->GetSize();
	// let's iterate over the list in STL syntax
	wxList::iterator iter;
	for (iter = m_List.begin(); iter != m_List.end(); ++iter)
	{
		VRow* vRow = (VRow*)*iter;
		wxSize size2 = vRow->GetSize();
		vRow->SetSize(1, nTop, size.x - barW - 2, size2.y);
		nTop += size2.y;
	}
	nTop = CC_MAX(size.y, nTop - 3);
	this->SetVirtualSize(-1, nTop);
	//SetScrollbars(1, 1, size.x - barW, nTop, 0, 0);
}

void VList::OnResize(wxSizeEvent& event)
{
	ReLayout();
}

void VList::SetSelected(VRow* vRow)
{
	wxList::iterator iter;
	for (iter = m_List.begin(); iter != m_List.end(); ++iter)
	{
		VRow* vRow2 = (VRow*)*iter;
		if(vRow2 != vRow)
		{
			vRow2->SetSelected(false);
		}
	}
	if(!vRow->IsSelected())
	{
		vRow->SetSelected(true);
	}
	m_selectedRow = vRow;
}

// VPlay
VPlay::VPlay( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
	: wxPanel( parent, id, pos, size, wxNO_BORDER )
{
	SetBackgroundColour(parent->GetBackgroundColour());
	//SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	m_owner = parent;
	m_play = NULL;

	this->Bind(wxEVT_SIZE, &VPlay::OnResize, this);
}

VPlay::~VPlay()
{
}

void VPlay::OnResize(wxSizeEvent& event)
{
	if(m_play != NULL)
	{
		wxSize size = this->GetSize();
		m_play->SetSize(0, size.y/4, size.x, size.y/2);
	}
}

wxPanel* VPlay::GetPlayPanel(bool isReset)
{
	if(isReset || m_play == NULL)
	{
		if(m_play != NULL)
		{
			this->RemoveChild(m_play);
			delete m_play;
			m_play = NULL;
		}
		m_play = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
		//post a event
		this->SendSizeEvent(wxSEND_EVENT_POST);
	}
	return m_play;
}

// MainFrame
MainFrame::MainFrame()
: ccSkinFrame( (wxFrame *)NULL, wxID_ANY, wxT("视频转换软件"),
          wxDefaultPosition, wxSize(900,600) )
{
    SetIcon(wxICON(mainicon));
	m_skin = new ccDrawSkin();
	this->AddAutoDestroy(m_skin);

	m_skin->LoadSkin(wxT(".\\skins\\skin01.xml"));
	this->SetccDrawSkin(m_skin);

    wxMenu *file_menu = new wxMenu();
	this->AddAutoDestroy(file_menu);

	itemSkin1 = new wxMenuItem(file_menu, wxNewId(), wxT("切换到皮肤&1"), wxT("换肤"), wxITEM_CHECK);
	itemSkin2 = new wxMenuItem(file_menu, wxNewId(), wxT("切换到皮肤&2"), wxT("换肤"), wxITEM_CHECK);
	itemSkin3 = new wxMenuItem(file_menu, wxNewId(), wxT("切换到皮肤&3"), wxT("换肤"), wxITEM_CHECK);
	itemAbout = new wxMenuItem(file_menu, wxNewId(), wxT("&About"));
	itemExit = new wxMenuItem(file_menu, wxNewId(), wxT("E&xit"));
    file_menu->Append( itemSkin1 );
    file_menu->Append( itemSkin2 );
    file_menu->Append( itemSkin3 );
    file_menu->AppendSeparator();
    file_menu->Append(itemAbout);
    file_menu->Append( itemExit );
	file_menu->Bind( wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnMenuSkin, this );
	itemSkin1->Check(true);

	this->SetTopMenu(file_menu);
    //SetMenuBar( menu_bar );

#if wxUSE_STATUSBAR
    //CreateStatusBar(2);
	//MainStatusBar* statbar = new MainStatusBar(this, wxSTB_DEFAULT_STYLE);
	//this->SetStatusBar(statbar);
    //int widths[] = { -1, 100 };
    //SetStatusWidths( 2, widths );
	//SetStatusText(wxT("准备就绪。"));
	//m_frameStatusBar->SetBackgroundColour(this->GetBackgroundColour());
#endif // wxUSE_STATUSBAR

	ccStatusBar* statbar = new ccStatusBar(this);
	this->SetStatusPanel(statbar);
	new ccLabel(statbar, wxID_ANY, wxT("准备就绪。"), wxPoint(3, 2), wxSize(80, 18));
	m_chkShutdown = new ccCheckbox( statbar, wxID_ANY, wxT("转换完成后关闭电脑"), wxPoint(200, 2), wxSize(140, 18) );

	int nBtnTop = 3;
	int nBtnLeft = 5;
	m_btnAddFile = new ccSkinButton(m_client, wxNewId(), wxT("添加文件"), wxPoint(nBtnLeft, nBtnTop), wxSize(120, 30));
	this->AddSkinButton(m_btnAddFile);
	m_btnAddFile->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);
	nBtnLeft += m_btnAddFile->GetSize().x + 5;

	m_btnPlay = new ccSkinButton(m_client, wxNewId(), wxT("播放"), wxPoint(nBtnLeft, nBtnTop), wxSize(120, 30));
	this->AddSkinButton(m_btnPlay);
	m_btnPlay->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);
	nBtnLeft += m_btnPlay->GetSize().x + 5;

	m_btnEdit = new ccSkinButton(m_client, wxNewId(), wxT("编辑"), wxPoint(nBtnLeft, nBtnTop), wxSize(120, 30));
	this->AddSkinButton(m_btnEdit);
	m_btnEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);
	nBtnLeft += m_btnEdit->GetSize().x + 5;

	m_btnRemove = new ccSkinButton(m_client, wxNewId(), wxT("移除"), wxPoint(nBtnLeft, nBtnTop), wxSize(120, 30));
	this->AddSkinButton(m_btnRemove);
	m_btnRemove->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);
	nBtnLeft += m_btnRemove->GetSize().x + 5;

	m_btnRemoveall = new ccSkinButton(m_client, wxNewId(), wxT("移除所有"), wxPoint(nBtnLeft, nBtnTop), wxSize(120, 30));
	this->AddSkinButton(m_btnRemoveall);
	m_btnRemoveall->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);

	m_btnFormat = new ccSkinButton(m_client, wxNewId(), wxT("设置"), wxPoint(nBtnLeft, nBtnTop), wxSize(80, 22));
	this->AddSkinButton(m_btnFormat);
	m_btnFormat->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);

	m_btnOutpath = new ccSkinButton(m_client, wxNewId(), wxT("打开"), wxPoint(nBtnLeft, nBtnTop), wxSize(80, 22));
	this->AddSkinButton(m_btnOutpath);
	m_btnOutpath->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);

	m_btnStart = new ccSkinButton(m_client, wxNewId(), wxT("开始转换"), wxPoint(nBtnLeft, nBtnTop), wxSize(155, 46));
	this->AddSkinButton(m_btnStart);
	m_btnStart->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);

	m_vList = new VList(m_client, wxNewId(), wxDefaultPosition, wxDefaultSize);
	m_vList->SetBackgroundColour( wxColour("#485464") );

	m_vPlay = new VPlay(m_client, wxNewId(), wxDefaultPosition, wxDefaultSize);
	m_vPlay->SetBackgroundColour( *wxWHITE );

	m_labFormat = new ccLabel(m_client, wxID_ANY, wxT("输出格式："), wxDefaultPosition, wxSize(80, 22));
	m_labOutpath = new ccLabel(m_client, wxID_ANY, wxT("输出目录："), wxDefaultPosition, wxSize(80, 22));

	m_txtFormat = new wxTextCtrl( m_client, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
	m_txtFormat->SetBackgroundColour(wxColour("#4e5a6c"));
	m_txtOutpath = new wxTextCtrl( m_client, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
	m_txtOutpath->SetBackgroundColour(wxColour("#4e5a6c"));

	wxButton* btn2 = new wxButton(m_vPlay, wxID_ANY, "Close", wxPoint(10, 1));
	btn2->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnQuit), NULL, this);

	ccSkinButton* btn = new ccSkinButton(m_vPlay, wxID_ANY, "Sample", wxPoint(10, 40), wxSize(100, 22));
	btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnQuit), NULL, this);
	this->AddSkinButton(btn);

	//for trigger resize, must after set ccDrawSkin
	SetMinSize(wxSize(900, 600));
	CentreOnScreen();
}

MainFrame::~MainFrame()
{
}

void _GetSelFilter(wxString& strfilter)
{
	strfilter.Append(_T("Common media formats|*.avi;*.wmv;*.wmp;*.wm;*.asf;*.rm;*.ram;*.rmvb;*.ra;*.mpg;*.mpeg;*.mpe;*.m1v;*.m2v;*.mpv2;"));
	strfilter.Append(_T("*.mp2v;*.dat;*.mp4;*.m4v;*.m4p;*.vob;*.ac3;*.dts;*.mov;*.qt;*.mr;*.3gp;*.3gpp;*.3g2;*.3gp2;*.swf;*.ogg;*.wma;*.wav;"));
	strfilter.Append(_T("*.mid;*.midi;*.mpa;*.mp2;*.mp3;*.m1a;*.m2a;*.m4a;*.aac;*.mkv;*.ogm;*.m4b;*.tp;*.ts;*.tpr;*.pva;*.pss;*.wv;*.m2ts;*.evo;"));
	strfilter.Append(_T("*.rpm;*.realpix;*.rt;*.smi;*.smil;*.scm;*.aif;*.aiff;*.aifc;*.amr;*.amv;*.au;*.acc;*.dsa;*.dsm;*.dsv;*.dss;*.pmp;*.smk;*.flic|"));
	strfilter.Append(_T("Windows Media Video(*.avi;*wmv;*wmp;*wm;*asf)|*.avi;*.wmv;*.wmp;*.wm;*.asf|"));
	strfilter.Append(_T("Windows Media Audio(*.wma;*wav;*aif;*aifc;*aiff;*mid;*midi;*rmi)|*.wma;*.wav;*.aif;*.aifc;*.aiff;*.mid;*.midi;*.rmi|"));
	strfilter.Append(_T("Real(*.rm;*ram;*rmvb;*rpm;*ra;*rt;*rp;*smi;*smil;*.scm)|*.rm;*.ram;*.rmvb;*.rpm;*.ra;*.rt;*.rp;*.smi;*.smil;*.scm|"));
	strfilter.Append(_T("MPEG Video(*.mpg;*mpeg;*mpe;*m1v;*m2v;*mpv2;*mp2v;*dat;*mp4;*m4v;*m4p;*m4b;*ts;*tp;*tpr;*pva;*pss;*.wv;)|"));
	strfilter.Append(_T("*.mpg;*.mpeg;*.mpe;*.m1v;*.m2v;*.mpv2;*.mp2v;*.dat;*.mp4;*.m4v;*.m4p;*.m4b;*.ts;*.tp;*.tpr;*.pva;*.pss;*.wv;|"));
	strfilter.Append(_T("MPEG Audio(*.mpa;*mp2;*m1a;*m2a;*m4a;*aac;*.m2ts;*.evo)|*.mpa;*.mp2;*.m1a;*.m2a;*.m4a;*.aac;*.m2ts;*.evo|"));
	strfilter.Append(_T("DVD(*.vob;*ifo;*ac3;*dts)|*.vob;*.ifo;*.ac3;*.dts|MP3(*.mp3)|*.mp3|CD Tracks(*.cda)|*.cda|"));
	strfilter.Append(_T("Quicktime(*.mov;*qt;*mr;*3gp;*3gpp;*3g2;*3gp2)|*.mov;*.qt;*.mr;*.3gp;*.3gpp;*.3g2;*.3gp2|"));
	strfilter.Append(_T("Flash Files(*.flv;*swf;*.f4v)|*.flv;*.swf;*.f4v|Playlist(*.smpl;*.asx;*m3u;*pls;*wvx;*wax;*wmx;*mpcpl)|*.smpl;*.asx;*.m3u;*.pls;*.wvx;*.wax;*.wmx;*.mpcpl|"));
	strfilter.Append(_T("Others(*.ivf;*au;*snd;*ogm;*ogg;*fli;*flc;*flic;*d2v;*mkv;*pmp;*mka;*smk;*bik;*ratdvd;*roq;*drc;*dsm;*dsv;*dsa;*dss;*mpc;*divx;*vp6;*.ape;*.flac;*.tta;*.csf)"));
	strfilter.Append(_T("|*.ivf;*.au;*.snd;*.ogm;*.ogg;*.fli;*.flc;*.flic;*.d2v;*.mkv;*.pmp;*.mka;*.smk;*.bik;*.ratdvd;*.roq;*.drc;*.dsm;*.dsv;*.dsa;*.dss;*.mpc;*.divx;*.vp6;*.ape;*.amr;*.flac;*.tta;*.csf|"));
	strfilter.Append(_T("All Files(*.*)|*.*||"));
}

void MainFrame::OnClick( wxCommandEvent &event )
{
	int nId = event.GetId();
	if(nId == m_btnAddFile->GetId())
	{
		wxString filter;
		_GetSelFilter(filter);
		wxFileDialog* OpenDialog = new wxFileDialog(
			this, _("Choose a file to open"), wxEmptyString, wxEmptyString, filter, wxFD_OPEN, wxDefaultPosition);
		// Creates a "open file" dialog with 4 file types
		if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
		{
			wxImage frame;
			wxString docPath = OpenDialog->GetPath();
			wxFfmpegDecoder ffmpegDecode;
			if(!ffmpegDecode.Load(docPath))
			{
				return;
			}

			ffmpegDecode.GetNextFrame(frame);
			if(frame.IsOk())
			{
				frame.SaveFile(_T("c:\\temp\\aaaa.png"));
			}
			VRow* row = new VRow(docPath, m_vList, wxNewId(), wxPoint(0, 0), wxSize(500, 78));
			m_vList->AddRow(row);
		}

		// Clean up after ourselves
		OpenDialog->Destroy();
	}
	else if(nId == m_btnPlay->GetId())
	{
		int cnt = m_vList->GetRowCount();
		if(cnt < 1)
		{
			(void)wxMessageBox( wxT("必须先添加文件才可以开始播放。"), wxT("错误提示"), wxICON_STOP | wxOK );
			return;
		}
		wxString sPath = m_vList->GetPath(0);
		if(!ccUtils::fExist(sPath))
		{
			(void)wxMessageBox( wxT("选择的文件不存在，请重新选择。"), wxT("错误提示"), wxICON_STOP | wxOK );
			return;
		}

		void* handle = (void *)m_vPlay->GetPlayPanel(true)->GetHandle();
		mainX(handle, sPath);
	}
	else if(nId == m_btnEdit->GetId())
	{
	}
	else if(nId == m_btnRemove->GetId())
	{
	}
	else if(nId == m_btnRemoveall->GetId())
	{
		m_vList->RemoveRowAll();
	}
	else if(nId == m_btnStart->GetId())
	{
		int cnt = m_vList->GetRowCount();
		if(cnt < 1)
		{
			(void)wxMessageBox( wxT("必须先添加文件才可以开始转换。"), wxT("错误提示"), wxICON_STOP | wxOK );
			return;
		}

		wxString sDesPath = m_txtOutpath->GetValue();
		if(sDesPath.IsEmpty())
		{
			(void)wxMessageBox( wxT("必须先选择输出目录才可以开始转换。"), wxT("错误提示"), wxICON_STOP | wxOK );
			return;
		}
		if(!ccUtils::dExist(sDesPath))
		{
			ccUtils::dCreate(sDesPath);
			if(!ccUtils::dExist(sDesPath))
			{
				(void)wxMessageBox( wxT("设置的输出目录创建失败，请选择正确的输出路径。"), wxT("错误提示"), wxICON_STOP | wxOK );
				return;
			}
		}
		for(int n = 0; n < cnt; n++)
		{
			wxString sPath = m_vList->GetPath(n);
			if(!ccUtils::fExist(sPath))
			{
				(void)wxMessageBox( wxT("选择的文件不存在，请重新选择。"), wxT("错误提示"), wxICON_STOP | wxOK );
				return;
			}
			//turn_one(sPath, sDesPath);
			mainXX(sPath, sDesPath);
			mainYY(sPath, sDesPath);
		}
	}
}

int MainFrame::turn_one(const wxString sPath, const wxString sDesPath)
{
	return 0;
}

void MainFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
    Close( true );
}

void MainFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
    (void)wxMessageBox( wxT("wxDragImage demo\n")
        wxT("Julian Smart (c) 2000"),
        wxT("About wxDragImage Demo"),
        wxICON_INFORMATION | wxOK );
}

void MainFrame::OnMenuSkin( wxCommandEvent &event )
{
	int nid = event.GetId();
	if(nid == itemSkin1->GetId())
	{
		itemSkin2->Check(false);
		itemSkin3->Check(false);

		itemSkin1->Check(true);
		m_skin->LoadSkin(wxT(".\\skins\\skin01.xml"));
		this->SetccDrawSkin(m_skin);
		this->SendSizeEvent(wxSEND_EVENT_POST);
	}
	else if(nid == itemSkin2->GetId())
	{
		itemSkin1->Check(false);
		itemSkin3->Check(false);

		itemSkin2->Check(true);
		m_skin->LoadSkin(wxT(".\\skins\\skin08.xml"));
		this->SetccDrawSkin(m_skin);
		this->SendSizeEvent(wxSEND_EVENT_POST);
	}
	else if(nid == itemSkin3->GetId())
	{
		itemSkin1->Check(false);
		itemSkin2->Check(false);

		itemSkin3->Check(true);
		m_skin->LoadSkin(wxT(".\\skins\\skin09.xml"));
		this->SetccDrawSkin(m_skin);
		this->SendSizeEvent(wxSEND_EVENT_POST);
	}
	else if(nid == itemAbout->GetId())
	{
		this->OnAbout(event);
	}
	else if(nid == itemExit->GetId())
	{
		this->OnQuit(event);
	}
	this->Refresh(true);
}

void MainFrame::OnResize(wxSizeEvent& event)
{
	ccSkinFrame::OnResize(event);

    const wxSize size = m_client->GetSize();
	int nLeft = size.x * 0.7;
	m_vList->SetSize(0, m_btnAddFile->GetSize().y + 6, nLeft, size.y - 100);
	m_vPlay->SetSize(nLeft, m_btnAddFile->GetSize().y + 6, size.x - nLeft, size.y - 100);

	m_labFormat->SetPosition(wxPoint(5, size.y - 55));
	m_labOutpath->SetPosition(wxPoint(5, size.y - 30));
	m_txtFormat->SetSize(90, size.y - 55, nLeft - 90, 22);
	m_txtOutpath->SetSize(90, size.y - 30, nLeft - 90, 22);

	m_btnFormat->SetPosition(wxPoint(nLeft + 5, size.y - 55));
	m_btnOutpath->SetPosition(wxPoint(nLeft + 5, size.y - 30));
	m_btnStart->SetPosition(wxPoint(nLeft + 5 + 90, size.y - 55));
}

IMPLEMENT_APP(MainApp)

//-----------------------------------------------------------------------------
// MainApp
//-----------------------------------------------------------------------------
MainApp::MainApp()
{
}

bool MainApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif

    MainFrame *frame = new MainFrame();

#if 0
    // Under Motif or GTK, this demonstrates that
    // wxScreenDC only gets the root window content.
    // We need to be able to copy the overall content
    // for full-screen dragging to work.
    int w, h;
    wxDisplaySize(& w, & h);
    wxBitmap bitmap(w, h);

    wxScreenDC dc;
    wxMemoryDC memDC;
    memDC.SelectObject(bitmap);
    memDC.Blit(0, 0, w, h, & dc, 0, 0);
    memDC.SelectObject(wxNullBitmap);
    m_background = bitmap;
#endif

    frame->Show( true );

    return true;
}

int MainApp::OnExit()
{
    return 0;
}
