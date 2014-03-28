
if [ $# != 4 ]; then
	echo "usage: $0 url width height cid"
	exit 0
fi

URL="http://livecdn.ahtv.cn/channels/1501/500.flv/live?1339768336791"
#URL=$1
SIZE="$2x$3"
NAME="snapshot_$4.jpg"

#exit 0
#ffmpeg -i $URL $NAME -ss 00:00:01 -r 1 -vframes 1 -an -vcodec mjpeg -s ${WIDTH}x${HEIGHT} 2>/dev/null &
ffmpeg -i $URL -y -f mjpeg -an -t 0.1 -s 180x90 $NAME

exit 0
