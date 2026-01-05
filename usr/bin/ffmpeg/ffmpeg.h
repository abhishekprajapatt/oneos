#ifndef FFMPEG_H
#define FFMPEG_H

#include <stdint.h>

typedef struct
{
    const char *input_file;
    const char *output_file;
    const char *codec;
    uint32_t bitrate;
    const char *resolution;
} ffmpeg_encode_options_t;

typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t fps;
    uint32_t bitrate;
    const char *codec;
    uint64_t duration;
} ffmpeg_media_info_t;

int ffmpeg_init(void);
int ffmpeg_encode(const char *input, const char *output);
int ffmpeg_encode_with_options(ffmpeg_encode_options_t *options);
int ffmpeg_get_media_info(const char *file_path, ffmpeg_media_info_t *info);
int ffmpeg_extract_audio(const char *video_file, const char *audio_file);
int ffmpeg_add_subtitles(const char *video_file, const char *subtitle_file, const char *output_file);

#endif
