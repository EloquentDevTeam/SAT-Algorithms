add_library(memutils STATIC memutils.cpp)
target_sources(memutils PUBLIC FILE_SET HEADERS FILES memutils.h)