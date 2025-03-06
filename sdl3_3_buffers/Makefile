compile: build copy-dir run


# https://gcc.gnu.org/onlinedocs/libstdc++/manual/using.html
# -lstdc++exp
# Linking to libstdc++exp.a is required for use of experimental C++ library 
# features. This currently provides support for the C++23 types defined in the 
# <stacktrace> header, the Filesystem library extensions defined in the 
# <experimental/filesystem> header, and the Contracts extensions enabled by -fcontracts.

# Copy command for Windows or Linux
#ifeq ($(OS),Windows_NT)
#   COPY_CMD = xcopy /E /I /Y
#else
#    COPY_CMD = cp -r
#endif

build:	
	g++ -std=c++23 ./src/main.cpp  -lstdc++exp \
    -o ./out/game.exe \
	-l SDL3 \
	-I C:\Libraries\SDL3-3.2.6\x86_64-w64-mingw32\include \
	-I C:\Libraries\glm-master\glm \
	-L C:\Libraries\SDL3-3.2.6\x86_64-w64-mingw32\lib

copy-dir:
	xcopy Content out\Content /E /I /Y

run:
	./out/game.exe
