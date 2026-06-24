cmake_minimum_required(VERSION 3.22)

project(AudioLimiter VERSION 0.1.0)

# Statically link the MSVC runtime so the .exe runs without the VC++ redistributable.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(C:/Users/bruno/JUCE JUCE-build)

juce_add_gui_app(AudioLimiter
    PRODUCT_NAME "Clamp"
)

target_sources(AudioLimiter
    PRIVATE
        src/main.cpp
        src/MainComponent.cpp
        src/AudioEngine.cpp
        src/CompressorProcessor.cpp
)

target_link_libraries(AudioLimiter
    PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_utils
)