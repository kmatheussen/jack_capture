#include <sndfile.h>

static int setformat_base(char *soundfile_format){
  return(
    (!strcasecmp("WAV",soundfile_format)) ? SF_FORMAT_WAV:
    (!strcasecmp("AIFF",soundfile_format)) ? SF_FORMAT_AIFF:
    (!strcasecmp("AU",soundfile_format)) ? SF_FORMAT_AU:
    (!strcasecmp("RAW",soundfile_format)) ? SF_FORMAT_RAW:
    (!strcasecmp("PAF",soundfile_format)) ? SF_FORMAT_PAF:
    (!strcasecmp("SVX",soundfile_format)) ? SF_FORMAT_SVX:
    (!strcasecmp("NIST",soundfile_format)) ? SF_FORMAT_NIST:
    (!strcasecmp("VOC",soundfile_format)) ? SF_FORMAT_VOC:
    (!strcasecmp("IRCAM",soundfile_format)) ? SF_FORMAT_IRCAM:
    (!strcasecmp("W64",soundfile_format)) ? SF_FORMAT_W64:
    (!strcasecmp("MAT4",soundfile_format)) ? SF_FORMAT_MAT4:
    (!strcasecmp("MAT5",soundfile_format)) ? SF_FORMAT_MAT5:
    (!strcasecmp("PVF",soundfile_format)) ? SF_FORMAT_PVF:
    (!strcasecmp("XI",soundfile_format)) ? SF_FORMAT_XI:
    (!strcasecmp("HTK",soundfile_format)) ? SF_FORMAT_HTK:
    (!strcasecmp("SDS",soundfile_format)) ? SF_FORMAT_SDS:
    (!strcasecmp("AVR",soundfile_format)) ? SF_FORMAT_AVR:
    (!strcasecmp("WAVEX",soundfile_format)) ? SF_FORMAT_WAVEX:
    (!strcasecmp("SD2",soundfile_format)) ? SF_FORMAT_SD2:
    (!strcasecmp("FLAC",soundfile_format)) ? SF_FORMAT_FLAC:
    (!strcasecmp("CAF",soundfile_format)) ? SF_FORMAT_CAF:
    (!strcasecmp("WVE",soundfile_format)) ? SF_FORMAT_WVE:
    (!strcasecmp("OGG",soundfile_format)) ? SF_FORMAT_OGG:
    (!strcasecmp("MPC2K",soundfile_format)) ? SF_FORMAT_MPC2K:
    (!strcasecmp("RF64",soundfile_format)) ? SF_FORMAT_RF64:
    -1);
}

int getformat(char *soundfile_format){
    return setformat_base(soundfile_format);
}

void print_all_formats(void){

  if(setformat_base("WAV")!=-1)
    printf("wav ");
  if(setformat_base("AIFF")!=-1)
    printf("aiff ");
  if(setformat_base("AU")!=-1)
    printf("au ");
  if(setformat_base("RAW")!=-1)
    printf("raw ");
  if(setformat_base("PAF")!=-1)
    printf("paf ");
  if(setformat_base("SVX")!=-1)
    printf("svx ");
  if(setformat_base("NIST")!=-1)
    printf("nist ");
  if(setformat_base("VOC")!=-1)
    printf("voc ");
  if(setformat_base("IRCAM")!=-1)
    printf("ircam ");
  if(setformat_base("W64")!=-1)
    printf("w64 ");
  if(setformat_base("MAT4")!=-1)
    printf("mat4 ");
  if(setformat_base("MAT5")!=-1)
    printf("mat5 ");
  if(setformat_base("PVF")!=-1)
    printf("pvf ");
  if(setformat_base("XI")!=-1)
    printf("xi ");
  if(setformat_base("HTK")!=-1)
    printf("htk ");
  if(setformat_base("SDS")!=-1)
    printf("sds ");
  if(setformat_base("AVR")!=-1)
    printf("avr ");
  if(setformat_base("WAVEX")!=-1)
    printf("wavex ");
  if(setformat_base("SD2")!=-1)
    printf("sd2 ");
  if(setformat_base("FLAC")!=-1)
    printf("flac ");
  if(setformat_base("CAF")!=-1)
    printf("caf ");
  if(setformat_base("WVE")!=-1)
    printf("wve ");
  if(setformat_base("OGG")!=-1)
    printf("ogg ");
  if(setformat_base("MPC2K")!=-1)
    printf("mpc2k ");
  if(setformat_base("RF64")!=-1)
    printf("rf64 ");
  if(setformat_base("MP3")!=-1)
    printf("mp3 ");
  if(setformat_base("MP2")!=-1)
    printf("mp2 ");
  if(setformat_base("SPEEX")!=-1)
    printf("speex ");
  if(setformat_base("WMA")!=-1)
    printf("wma ");
  if(setformat_base("AAC")!=-1)
    printf("aac ");
  if(setformat_base("VQF")!=-1)
    printf("vqf ");
  if(setformat_base("RA")!=-1)
    printf("ra ");
  if(setformat_base("ALAC")!=-1)
    printf("alac ");
  if(setformat_base("AIFC")!=-1)
    printf("aifc ");
   printf("\n");
}
