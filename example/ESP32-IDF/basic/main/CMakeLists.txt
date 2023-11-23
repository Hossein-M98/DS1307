idf_component_register(
  SRCS "main.c" "./main.c" "../../../../src/DS1307.c" "../../../../port/ESP32-IDF/DS1307_platform.c"
  INCLUDE_DIRS "../../../../src/include" "../../../../port/ESP32-IDF"
  )