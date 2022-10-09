#ifndef PATHTRACER_RENDERSETTINGS_H
#define PATHTRACER_RENDERSETTINGS_H

// RENDER SETTINGS
#define MAIN_RESOLUTION_W 1920
#define MAIN_RESOLUTION_H 1080
#define RESOLUTION_DIVIDER 2 

#define LIGHT_SAMPLES 2    // 8 for HQ
#define INDIRECT_SAMPLES 4 // 256 for high quality
#define INDIRECT_DEPTH 2    // 1 for direct lighting only, 2 for HQ

#define RAY_TRACING_THRESHOLD 0.000001f
#define INDIRECT_RAY_BIAS 0.00001f
#define SHADOW_BIAS 0.00001f

// CONSTANTS
#define ACCELERATOR_DEPTH_LIMIT 50
#define ACCELERATOR_MIN_FACES_COUNT 2

#define WORLD_MAX_DISTANCE 9999999.f
#define STATIC_FRAME 0

#define DEFAULT_ALBEDO 0.18f

#define LIGHT_MESH_ID -2
#define LIGHT_SHADER_ID -2

#define RESOLUTION_W (MAIN_RESOLUTION_W / RESOLUTION_DIVIDER)
#define RESOLUTION_H (MAIN_RESOLUTION_H / RESOLUTION_DIVIDER)

#define DEBUG_PIXEL false 
#define DEBUG_PIXEL_X 291 
#define DEBUG_PIXEL_Y 470 

#endif // PATHTRACER_RENDERSETTINGS_H
