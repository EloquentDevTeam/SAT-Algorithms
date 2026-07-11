add_library(memutils STATIC memutils.cpp
        common.h)
target_sources(memutils PUBLIC FILE_SET HEADERS FILES memutils.h)