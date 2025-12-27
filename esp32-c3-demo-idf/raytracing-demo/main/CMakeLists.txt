idf_component_register(
    SRCS "main.cpp" "raytracer.cpp"
    INCLUDE_DIRS "."
    REQUIRES
        arduino
        nvs_flash
)
