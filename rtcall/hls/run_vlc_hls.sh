export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/vlc/lib
export PATH=$PATH:/usr/local/vlc/bin

URL="http://livecdn.ahtv.cn/channels/1501/500.flv/live?1339768336791"

#
# for transcode module
# threads=1,width=320,height=180,fps=15,vcodec=h264,vb=192
# profile=baseline,level=30,preset=faster,tune=zerolatency
# aud,ref=2,qp=35,vbv-maxrate=192,vbv-bufsize=64,frames=10240000,intra-refresh,weightp=1
# keyint=12,min-keyint=5,deblock=-1:-2,subme=5,bpyramid=none,bitrate=192
VENC_OPT="width=320,height=180,fps=12,vcodec=h264,vb=192"
VENC_OPT1="profile=baseline,level=30"
#VENC_OPT1="profile=main"
VENC_OPT2="aud,ref=1" #,vbv-maxrate=192,vbv-bufsize=96"
AENC_OPT="acodec=aac,ab=24"

#
# for access module
INDEX="/var/www/streaming/mystream.m3u8"
INDEX_URL="http://www.uskee.org/streaming/media/test-########.ts"
ACCESS="livehttp{seglen=10,extsegs=10,numsegs=5,index=$INDEX,index-url=$INDEX_URL}"

#
# for ts mux module
# use-key-frames=true,pcr=50,dts-delay=3000
MUX="ts{use-key-frames}"

#
# for dst module
DST="/var/www/streaming/media/test-########.ts"

#
# for vlc --sout
SOUT="#transcode{$VENC_OPT,venc=x264{$VENC_OPT1,$VENC_OPT2},$AENC_OPT}:std{access=$ACCESS,mux=$MUX,dst=$DST}"

#exit 0

#
# run
mkdir -p /var/www/streaming/media/
rm -f /var/www/streaming/media/*.ts
vlc -I dummy $URL --sout=$SOUT --http-reconnect --verbose 1 vlc://quit
