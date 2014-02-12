./autogen.sh

mkdir oldbld
cd oldbld && ../configure --with-ssl --with-srtp --with-speexdsp
make && make install
