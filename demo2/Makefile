FFMPEG_INC_PATH=-I/opt/ffmpeg/build/include
FFMPEG_LIBS=-L/opt/ffmpeg/build/lib -lavcodec -lavformat -lavutil 

OPENCV_INC_PATH=-I/usr/local/include/opencv4
OPENCV_LIBS=-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml

INC_PATH=${FFMPEG_INC_PATH} ${OPENCV_INC_PATH}
LIB_PATH=${FFMPEG_LIBS} ${OPENCV_LIBS}
.SUFFIXES: .cpp .c

main:main.cpp
	g++ main.cpp -o main  ${INC_PATH} ${LIB_PATH}

info:
	@echo "INC_PATH:"${INC_PATH}
	@echo "LIB_PATH:"${LIB_PATH}


clean:
	@rm -rf main 
	@rm -rf *.flv
	@rm -rf *.o
	@echo Done cleaning

