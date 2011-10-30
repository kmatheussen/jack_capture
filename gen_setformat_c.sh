#This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


ai="
WAV
AIFF
AU
RAW
PAF
SVX
NIST
VOC
IRCAM
W64
MAT4
MAT5
PVF
XI
HTK
SDS
AVR
WAVEX
SD2
FLAC
CAF
WVE
OGG
MPC2K
RF64
MP3
MP2
SPEEX
WMA
AAC
VQF
RA
ALAC
AIFC
"

echo "static int setformat_base(char *soundfile_format){"
echo "  return("

for a in $ai;do
    echo "#include <sndfile.h>" >temp.c
    echo "main(){return SF_FORMAT_"$a";}" >>temp.c
    echo >>temp.c
    if gcc temp.c 2>/dev/null; then
	echo "    (!strcasecmp(\""$a"\",soundfile_format)) ? SF_FORMAT_"$a":"
    fi
done
echo "    -1);"
echo "}"
echo
echo "int getformat(char *soundfile_format){"
echo "    return setformat_base(soundfile_format);"
echo "}"
echo
echo "void print_all_formats(void){"
#echo "  printf(\"Supported formats: \\n\");"
echo
for a in $ai;do
    echo "  if(setformat_base(\""$a"\")!=-1)"
    echo "    printf(\""`echo $a|tr '[:upper:]' '[:lower:]'`" \");"
done
echo '   printf("\n");'
echo "}"
