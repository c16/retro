idf_component_register(
    SRCS "main.cpp" "audio.cpp" "game.cpp" "config.cpp"
    INCLUDE_DIRS "."
    REQUIRES
        arduino
        bt
        nvs_flash
)
