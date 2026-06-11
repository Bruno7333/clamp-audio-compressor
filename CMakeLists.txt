cmake_minimum_required(VERSION 3.22)

project(AudioLimiter VERSION 0.1.0)

add_subdirectory(C:/Users/bruno/JUCE JUCE-build)

juce_add_gui_app(AudioLimiter
    PRODUCT_NAME "Audio Limiter"
)

target_sources(AudioLimiter
    PRIVATE
        src/main.cpp
        src/MainComponent.cpp
)

target_link_libraries(AudioLimiter
    PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_utils
)