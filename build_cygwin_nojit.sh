# P-UAE
#
# 2006-2011 Mustafa TUFAN (aka GnoStiC/BRONX)
#
#
#

base=" --with-sdl --with-sdl-gfx --with-sdl-sound --enable-drvsnd "
cd32=" --enable-cd32 "
a600=" --enable-gayle "
#scsi=" --enable-scsi-device --enable-ncr --enable-a2091 "
scsi=""
other=" --with-caps --enable-amax --disable-jit"
#
#
./bootstrap.sh
./configure $base $cd32 $a600 $scsi $other CFLAGS="-D_WIN32 -mno-cygwin -mwindows" CPPFLAGS="-D_WIN32 -mno-cygwin -mwindows" LDFLAGS="-D_WIN32 -mno-cygwin -mwindows"
make clean
make
