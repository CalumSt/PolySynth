cmake_minimum_required(VERSION 3.22)
# Set up the testing project
project(TESTING)
# allow testing from this directory
enable_testing()

# set up sources

set(This Tests)
# --------------------------------------------------------------------------
# ADD TEST FILES HERE
set(SOURCES
    ADSREnvelope_test.cpp
    Oscillator_test.cpp
    Voice_test.cpp
    Synth_test.cpp
)
# --------------------------------------------------------------------------


add_executable(${PROJECT_NAME} ${SOURCES})
#add_executable(${This} ADSREnvelope_test.cpp)

# --------------------------------------------------------------------------
# ADD DEPENDENCIES HERE
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Libs/googletest/googletest/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Source
        ${CMAKE_CURRENT_SOURCE_DIR}/../Libs/JUCE/modules
    )    
# --------------------------------------------------------------------------
    
target_link_libraries(${PROJECT_NAME} 
PRIVATE
    juce::juce_audio_utils
PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags    
    JX11
    GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})