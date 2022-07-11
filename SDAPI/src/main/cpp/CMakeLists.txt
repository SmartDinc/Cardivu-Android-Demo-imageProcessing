cmake_minimum_required(VERSION 3.18.1)

set(pathPROJECT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)
set(pathOPENCV ${pathPROJECT}/opencv)
set(pathLIBOPENCV_JAVA ${pathOPENCV}/native/libs/${ANDROID_ABI}/libopencv_java4.so)

#SMU_pupil file
file(GLOB Library_SOURCES "src/main/cpp/SMU_pupil/*.cpp")
file(GLOB Library_HEADERS "src/main/cpp/SMU_pupil/*.h")
file(GLOB Library_SOURCES_C "src/main/cpp/SMU_pupil/*.c")

include_directories(${pathOPENCV}/native/jni/include)
include_directories(${pathPROJECT}/src/main/cpp/SMU_pupil)

add_library(
        native-lib

        SHARED

        native-lib.cpp
        )


add_library(
        dsp-lib

        SHARED

        dsp-lib.cpp

        # files related to stft
        fft/stft.cpp
        fft/real_double_fft.cpp
        fft/real_double_fft_mixed.cpp
        fft/stft.h
        fft/real_double_fft.h
        fft/real_double_fft_mixed.h

        # files related to iir filter
        iir/biquad.cpp
        iir/butterworth.cpp
        iir/cascade.cpp
        iir/design.cpp
        iir/filter.cpp
        iir/param.cpp
        iir/pole_filter.cpp
        iir/root_finder.cpp
        iir/biquad.h
        iir/butterworth.h
        iir/cascade.h
        iir/common.h
        iir/design.h
        iir/dsp.h
        iir/filter.h
        iir/layout.h
        iir/math_supplement.h
        iir/params.h
        iir/pole_filter.h
        iir/root_finder.h
        iir/smoothed_filter.h
        iir/state.h
        iir/types.h
        iir/utilities.h
        )

add_library( lib_opencv SHARED IMPORTED )

set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${pathLIBOPENCV_JAVA})

find_library(
        log-lib

        log)

target_link_libraries(
        native-lib

        dsp-lib

        lib_opencv

        ${log-lib})
