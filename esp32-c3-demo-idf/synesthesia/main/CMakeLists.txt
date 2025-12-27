idf_component_register(
    SRCS "main.cpp" "synesthesia.cpp"
    INCLUDE_DIRS "."
    REQUIRES
        arduino
        nvs_flash
        driver
)
