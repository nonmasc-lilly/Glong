#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef GLONG_H

#define LG_REPORT_ERROR "\033[90;40mERROR"
#define LG_REPORT_WARNING "\033[93;40mWARNING"
#define LG_REPORT_INFO "\033[32;40mINFO"
#define LG_FREPORT(KIND, FORMAT, ...)\
    fprintf(stderr, "%s:: " FORMAT "\033[37;40m", KIND, __VA_ARGS__)
#define LG_REPORT(KIND, STRING)\
    fprintf(stderr, "%s:: " STRING "\033[37;40m", KIND)
#define LG_FMAKE_COLOR(red, green, blue, alpha)\
    (LG_MAKE_COLOR(\
        (unsigned char)((red)   * 0xFF),\
        (unsigned char)((green) * 0xFF),\
        (unsigned char)((blue)  * 0xFF),\
        (unsigned char)((alpha) * 0xFF)))
#define LG_MAKE_COLOR(red, green, blue, alpha)\
    ((unsigned int)((red)   & 0xFF)       |\
    ((unsigned int)((green) & 0xFF) << 8) |\
    ((unsigned int)((blue)  & 0xFF) << 16)|\
    ((unsigned int)((alpha) & 0xFF) << 24))
#define LG_RED_FROM_COLOR(color)   (color & 0xFF)
#define LG_GREEN_FROM_COLOR(color) ((color >> 8) & 0xFF)
#define LG_BLUE_FROM_COLOR(color)  ((color >> 16) & 0xFF)
#define LG_ALPHA_FROM_COLOR(color) ((color >> 24) & 0xFF)
#define LG_COLOR_BREAKDOWN_F(color, color_func) ((float)(color_func(color)) / 255.0)
#define LG_VERTEX_STRIDE 8 * sizeof(float)

typedef unsigned int LG_COLOR;
typedef unsigned int LG_GRAPHICS_OBJECT;

enum LG_type_name {
    LG_VT_INT,   LG_VT_UINT,  LG_VT_FLOAT,
    LG_VT_IVEC2, LG_VT_IVEC3, LG_VT_IVEC4, 
    LG_VT_UVEC2, LG_VT_UVEC3, LG_VT_UVEC4,
    LG_VT_VEC2,  LG_VT_VEC3,  LG_VT_VEC4,
};

union value {
    int i;
    unsigned int u;
    float f;
    int *iv;
    unsigned int *uv;
    float *fv;
};

struct LG_renderer {
    bool active, is_current;
    unsigned int swap_interval;
    LG_COLOR color;
    double deltatime, last_time;
    GLFWwindow *window;
};

struct LG_renderer LG_create_renderer(unsigned int width, unsigned int height,
    const char *name, LG_COLOR color, unsigned int swap_interval);
void LG_destroy_renderer(struct LG_renderer *render);
bool LG_is_renderer_window_closed(struct LG_renderer render);
void LG_poll();
struct LG_renderer *LG_renderer_make_current(struct LG_renderer *render);
#define LG_get_current_renderer(render) LG_renderer_make_current(render)
void LG_renderer_refresh(struct LG_renderer render);
void LG_terminate();

enum LG_draw_type {
    LG_DT_STREAM  =  GL_STREAM_DRAW,
    LG_DT_STATIC  =  GL_STATIC_DRAW,
    LG_DT_DYNAMIC = GL_DYNAMIC_DRAW
};

struct LG_render_object {
    LG_GRAPHICS_OBJECT vao, vbo, ebo, shader, texture;
    unsigned int parent_renderers_len, index_num;
    struct LG_renderer **parent_renderers;
};

/*
*   VERTEX FORMAT:
*       <position [vec3]>, <color [vec3]>, <uv [vec2]>
*   while these must all exist, they do not need to be used
*   to be a valid vertex, unused values should be zero
*/
LG_GRAPHICS_OBJECT LG_compile_shader(const char *vertex_src,
    const char *fragment_src);
void LG_discard_shader(LG_GRAPHICS_OBJECT shader);

struct LG_render_object LG_create_render_object(
    const float *vertices, unsigned int vertices_len,
    const unsigned int *indices, unsigned int indices_len,
    enum LG_draw_type draw_type);
void LG_render_object_add_shader(struct LG_render_object *render_object,
    LG_GRAPHICS_OBJECT shader);
void LG_render_object_add_parent(struct LG_render_object *render_object,
    struct LG_renderer *parent);
void LG_render_object_add_texture(struct LG_render_object *render_object,
    LG_GRAPHICS_OBJECT texture);
void LG_destroy_render_object(struct LG_render_object *render_object);

void LG_render_object_draw(struct LG_render_object *render_object);
void LG_render_object_prepare_shader_parameter(
    struct LG_render_object *render_object, const char *name, union value var,
    enum LG_type_name type);

enum LG_key_type {
    LG_KT_SPACE=32,
    LG_KT_APOSTROPHE=39,
    LG_KT_COMMA=44,
    LG_KT_MINUS=45,
    LG_KT_PERIOD=46,
    LG_KT_SLASH=47,
    LG_KT_0=48,
    LG_KT_1=49,
    LG_KT_2=50,
    LG_KT_3=51,
    LG_KT_4=52,
    LG_KT_5=53,
    LG_KT_6=54,
    LG_KT_7=55,
    LG_KT_8=56,
    LG_KT_9=57,
    LG_KT_SEMICOLON=59,
    LG_KT_EQUAL=61,
    LG_KT_A=65,
    LG_KT_B=66,
    LG_KT_C=67,
    LG_KT_D=68,
    LG_KT_E=69,
    LG_KT_F=70,
    LG_KT_G=71,
    LG_KT_H=72,
    LG_KT_I=73,
    LG_KT_J=74,
    LG_KT_K=75,
    LG_KT_L=76,
    LG_KT_M=77,
    LG_KT_N=78,
    LG_KT_O=79,
    LG_KT_P=80,
    LG_KT_Q=81,
    LG_KT_R=82,
    LG_KT_S=83,
    LG_KT_T=84,
    LG_KT_U=85,
    LG_KT_V=86,
    LG_KT_W=87,
    LG_KT_X=88,
    LG_KT_Y=89,
    LG_KT_Z=90,
    LG_KT_LEFT_BRACKET=91,
    LG_KT_BACKSLASH=92,
    LG_KT_RIGHT_BRACKET=93,
    LG_KT_GRAVE_ACCENT=96,
    LG_KT_ESCAPE=256,
    LG_KT_ENTER=257,
    LG_KT_TAB=258,
    LG_KT_BACKSPACE=259,
    LG_KT_INSERT=260,
    LG_KT_DELETE=261,
    LG_KT_RIGHT=262,
    LG_KT_LEFT=263,
    LG_KT_DOWN=264,
    LG_KT_UP=265,
    LG_KT_PAGE_UP=266,
    LG_KT_PAGE_DOWN=267,
    LG_KT_HOME=268,
    LG_KT_END=269,
    LG_KT_CAPS_LOCK=280,
    LG_KT_SCROLL_LOCK=281,
    LG_KT_NUM_LOCK=282,
    LG_KT_PRINT_SCREEN=283,
    LG_KT_PAUSE=284,
    LG_KT_F1 =290,
    LG_KT_F2 =291,
    LG_KT_F3 =292,
    LG_KT_F4 =293,
    LG_KT_F5 =294,
    LG_KT_F6 =295,
    LG_KT_F7 =296,
    LG_KT_F8 =297,
    LG_KT_F9 =298,
    LG_KT_F10=299,
    LG_KT_F11=300,
    LG_KT_F12=301,
    LG_KT_F13=302,
    LG_KT_F14=303,
    LG_KT_F15=304,
    LG_KT_F16=305,
    LG_KT_F17=306,
    LG_KT_F18=307,
    LG_KT_F19=308,
    LG_KT_F20=309,
    LG_KT_F21=310,
    LG_KT_F22=311,
    LG_KT_F23=312,
    LG_KT_F24=313,
    LG_KT_F25=314,
    LG_KT_KP0=320,
    LG_KT_KP1=321,
    LG_KT_KP2=322,
    LG_KT_KP3=323,
    LG_KT_KP4=324,
    LG_KT_KP5=325,
    LG_KT_KP6=326,
    LG_KT_KP7=327,
    LG_KT_KP8=328,
    LG_KT_KP9=329,
    LG_KT_KP_DECIMAL=330,
    LG_KT_KP_DIVIDE=331,
    LG_KT_KP_MULTIPLY=332,
    LG_KT_KP_SUBTRACT=333,
    LG_KT_KP_ADD=334,
    LG_KT_KP_ENTER=335,
    LG_KT_KP_EQUAL=336,
    LG_KT_LEFT_SHIFT=340,
    LG_KT_LEFT_CONTROL=341,
    LG_KT_LEFT_ALT=342,
    LG_KT_LEFT_SUPER=343,
    LG_KT_RIGHT_SHIFT=344,
    LG_KT_RIGHT_CONTROL=345,
    LG_KT_RIGHT_ALT=346,
    LG_KT_RIGHT_SUPER=347,
    LG_KT_MENU=348
};

enum LG_key_state {
    LG_KS_PRESSED=GLFW_PRESS,
    LG_KS_RELEASED=GLFW_RELEASE,
    LG_KS_REPEATED=GLFW_REPEAT
};

enum LG_key_state LG_get_key_state(enum LG_key_type key);

double LG_deltatime();

enum LG_filter_type {
    LG_FT_LINEAR=GL_LINEAR,
    LG_FT_NEAREST=GL_NEAREST
};

LG_GRAPHICS_OBJECT LG_create_texture(const unsigned char *texture, unsigned int width,
    unsigned int height, enum LG_filter_type filter_type);

void LG_destroy_texture(LG_GRAPHICS_OBJECT texture);

#endif
