idf_component_register(
    SRCS "main.cpp" "marble_game.cpp"
    INCLUDE_DIRS "."
    REQUIRES
        arduino
        nvs_flash
)
