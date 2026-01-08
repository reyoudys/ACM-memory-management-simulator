all:
	g++ -std=c++17 \
	-Iinclude \
	src/main.cpp \
	src/memory.cpp \
	src/buddy/buddy.cpp \
	src/cache/cache.cpp \
	-o memsim
