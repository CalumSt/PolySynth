cmake_minimum_required(VERSION 3.22)

# Always use the newest C++ standard on green-field projects if possible.
set(CMAKE_CXX_STANDARD 23)
if (APPLE)
    # On Mac, we need to wait for a new JUCE version that fixes the compilation issue
    set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Change me!
# This is the internal name of the project and the name of JUCE's shared code target
# Note: This cannot have spaces (it may be 2024, but you can't have it all!)
# Worry not, JUCE's PRODUCT_NAME can have spaces (and is what DAWs display)
set(PROJECT_NAME "JX11")

# From Eyal Amir: https://forum.juce.com/t/cmake-and-intellisense-for-windows-vscode/62183
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(COMPANY_NAME "QUILTS")


project(${PROJECT_NAME} VERSION 0.1)

add_subdirectory(Libs/JUCE)

# add_subdirectory(Libs/melatonin_inspector)

juce_add_plugin(${PROJECT_NAME}
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME ${COMPANY_NAME}                # Specify the name of the plugin's author
    IS_SYNTH TRUE                               # Is this a synth or an effect?
    NEEDS_MIDI_INPUT TRUE                       # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    # COPY_PLUGIN_AFTER_BUILD TRUE               # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Qlts               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Aran                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "JX11"                         # The name of the final executable, which can differ from the target name
)

add_subdirectory(Libs/foleys_gui_magic/modules)

juce_generate_juce_header(${PROJECT_NAME})

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS 
"${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h")
target_sources("${PROJECT_NAME}" PRIVATE ${SourceFiles})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        foleys_gui_magic
        juce::juce_audio_basics
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_recommended_warning_flags
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        FOLEYS_ENABLE_BINARY_DATA=0
        FOLEYS_SHOW_GUI_EDITOR_PALLETTE=1
        FOLEYS_SAVE_EDITED_GUI_IN_PLUGIN_STATE=0)

## Testing

add_subdirectory(Tests)
add_subdirectory(Libs/googletest)
enable_testing()



