mkdir ./m4
cp /usr/share/aclocal/pkg.m4 ./m4/

export PREFIX=/opt/webrtc2sip
./autogen.sh 
mkdir oldbld
cd oldbld && ../configure --prefix=$PREFIX
make && make install
cp -f ../config.xml $PREFIX/sbin/config.xml
