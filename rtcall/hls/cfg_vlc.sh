#--disable-nls 
#--disable-rpath
./configure \
	--prefix=/usr/local/vlc \
	--disable-lua --disable-postproc --disable-xcb \
	--disable-alsa --disable-libgcrypt \
	--disable-sdl \
	--disable-qt4 \
	--disable-dbus \
	--disable-dbus-control \
	--enable-twolame --enable-faad --enable-a52 --enable-mad --enable-vorbis \
	--enable-pvr \
	--enable-omxil \
	--enable-iomx \
	--enable-dvbpsi \
	--enable-ogg --enable-mux_ogg --enable-x264 --enable-theora --enable-mkv \
	--enable-avcodec --enable-avformat --enable-swscale \
	--enable-live555 \
	--enable-debug \
	--enable-gprof \
	--enable-run-as-root 

