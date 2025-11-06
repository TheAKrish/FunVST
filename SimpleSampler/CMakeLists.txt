# SimpleSampler - WAV Sampler VST Plugin
# CMake build configuration for cross-platform compilation

cmake_minimum_required(VERSION 3.15)

# Project definition
project(SimpleSampler VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE to the build
add_subdirectory(../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Define the plugin target
juce_add_plugin(SimpleSampler
    # Basic plugin information
    COMPANY_NAME "YourName"                     # Change this to your name/company
    PLUGIN_MANUFACTURER_CODE Ynam               # 4-character unique manufacturer code
    PLUGIN_CODE Ssmp                            # 4-character unique plugin code

    # Plugin formats to build
    FORMATS AU VST3                             # AU for macOS, VST3 for all platforms

    # Plugin characteristics
    IS_SYNTH TRUE                               # This is a synthesizer/instrument
    NEEDS_MIDI_INPUT TRUE                       # Accepts MIDI input
    NEEDS_MIDI_OUTPUT FALSE                     # Does not produce MIDI output
    IS_MIDI_EFFECT FALSE                        # Not a MIDI effect
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE            # UI should receive keyboard input

    # Plugin identification
    PRODUCT_NAME "SimpleSampler"                # Name displayed in DAW

    # Copy to standard plugin locations after build
    COPY_PLUGIN_AFTER_BUILD TRUE

    # Bundle identifier for macOS
    BUNDLE_ID com.yourname.SimpleSampler        # Change this to your domain
)

# Add source files
target_sources(SimpleSampler
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

# Set compile definitions
target_compile_definitions(SimpleSampler
    PUBLIC
        # JUCE configuration
        JUCE_WEB_BROWSER=0                      # Disable web browser (not needed)
        JUCE_USE_CURL=0                         # Disable curl (not needed)
        JUCE_VST3_CAN_REPLACE_VST2=0            # VST3 settings

        # Plugin settings
        JucePlugin_Name="SimpleSampler"
        JucePlugin_Desc="Simple WAV Sampler with Virtual Keyboard"
        JucePlugin_VersionString="1.0.0"
)

# Link JUCE modules
target_link_libraries(SimpleSampler
    PRIVATE
        # JUCE core modules
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra

        # JUCE DSP module for reverb
        juce::juce_dsp

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Platform-specific settings
if(APPLE)
    # macOS specific settings
    set_target_properties(SimpleSampler PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
    )
endif()

# Print build information
message(STATUS "")
message(STATUS "SimpleSampler Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Plugin Formats: AU, VST3")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "")
