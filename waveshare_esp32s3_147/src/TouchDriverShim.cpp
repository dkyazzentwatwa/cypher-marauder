// Arduino CLI compiles sketch `src/` recursively, but not this project's
// top-level `drivers/` directory. Keep the requested driver layout and include
// the local Waveshare touch implementation here so it becomes part of the build.
#include "../include/BoardConfig.h"
#if ENABLE_TOUCH
#include "../drivers/esp_lcd_touch_axs5106l/esp_lcd_touch_axs5106l.cpp"
#endif
