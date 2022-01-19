INC_PATH=-I/opt/ffmpeg/build/include
LIB_PATH=-L/opt/ffmpeg/build/lib -lavcodec -lavformat -lavutil 

.SUFFIXES: .cpp .c

main:main.cpp
	g++ main.cpp -o main  ${INC_PATH} ${LIB_PATH}

clean:
	@rm -rf main 
	@rm -rf *.flv
	@rm -rf *.o
	@echo Done cleaning

