#include "inc/main.h"
#include <time.h>
#include <stdlib.h>

#define PW 0xFF,0xFF,0xFF,0xFF,
#define PB 0,0,0,0,
unsigned char texture0[20*40] = {
    PW PW PW PW PW
    PW PB PB PB PW
    PW PB PB PB PW
    PW PB PB PB PW
    PW PB PB PB PW
    PW PB PB PB PW
    PW PB PB PB PW
    PW PB PB PB PW
    PW PB PB PB PW
    PW PW PW PW PW
};
unsigned char texture1[20*40] = {
    PB PB PW PB PB
    PB PW PW PB PB
    PB PB PW PB PB
    PB PB PW PB PB
    PB PB PW PB PB
    PB PB PW PB PB
    PB PB PW PB PB
    PB PB PW PB PB
    PB PB PW PB PB
    PW PW PW PW PW
};
#undef PW
#undef PB

static const float vertices[] = {
     1.0,  1.0, 0,  1,1,1,  1,1,
     1.0, -1.0, 0,  1,1,1,  1,0,
    -1.0, -1.0, 0,  1,1,1,  0,0,
    -1.0,  1.0, 0,  1,1,1,  0,1
};
static const unsigned int indices[] = {
    0, 1, 3, 1, 2, 3
};

const char *vscoreshader =
    "#version 330 core\n"
    "layout (location = 0) in vec3 layposition;\n"
    "layout (location = 1) in vec3 laycolor;\n"
    "layout (location = 2) in vec2 layuv;\n"
    "out vec3 ocol;\n"
    "out vec2 tcrd;\n"
    "uniform vec2 translation;\n"
    "uniform vec2 scale;\n"
    "void main() {\n"
    "   gl_Position = vec4(vec3(translation, 0)+"
    "       (layposition*vec3(scale,0)), 1.0);\n"
    "   ocol = laycolor;\n"
    "   tcrd = layuv;\n"
    "}\n",
*fscoreshader =
    "#version 330 core\n"
    "out vec4 fcol;\n"
    "in vec3 ocol;\n"
    "in vec2 tcrd;\n"
    "uniform sampler2D tex;\n"
    "void main() {\n"
    "   fcol = texture(tex, vec2(tcrd.x, 1-tcrd.y))*vec4(ocol, 1.0);\n"
    "}\n";

#define SCRSZ {0.03, 0.03}
#define SCRPS {-0.6, 0.7}
void score1(int increment, bool render, bool deinit, struct LG_renderer *renderer) {
    static int  score       = 0;
    static bool initialized = false;
    static LG_GRAPHICS_OBJECT tex0, tex1, shader;
    static struct LG_render_object ro[8];
    static float pos[2] = SCRPS;
    static float siz[2] = SCRSZ;
    union value v;
    unsigned int i;
    if(!initialized) {
        tex0 = LG_create_texture(texture0, 5, 10, LG_FT_NEAREST);
        tex1 = LG_create_texture(texture1, 5, 10, LG_FT_NEAREST);
        shader = LG_compile_shader(vscoreshader, fscoreshader);
        for(i=0; i<8; i++) {
            ro[i] = LG_create_render_object(vertices, 32, indices, 6, LG_DT_STREAM);
            LG_render_object_add_shader(&ro[i], shader);
            LG_render_object_add_parent(&ro[i],renderer);
        }
        initialized = true;
    }
    score+=increment;
    pos[0] = -0.6;
    if(render) {
        for(i=0; i < 8; i++) {
            LG_render_object_add_texture(&ro[i], ((score >> 7-i) & 1) ? tex1 : tex0);
            v.fv = pos;
            LG_render_object_prepare_shader_parameter(
                &ro[i], "translation", v, LG_VT_VEC2);
            v.fv = siz;
            LG_render_object_prepare_shader_parameter(
                &ro[i], "scale", v, LG_VT_VEC2);
            LG_render_object_draw(&ro[i]);
            pos[0] += siz[0]*2;
        }
    }
    if(deinit) {
        for(i=0; i<8; i++) {
            LG_destroy_render_object(&ro[i]);
        }
        LG_discard_shader(shader);
        LG_destroy_texture(tex0);
        LG_destroy_texture(tex1);
        initialized = false;
    }
    
}

void score2(int increment, bool render, bool deinit, struct LG_renderer *renderer) {
    static int  score       = 0;
    static bool initialized = false;
    static LG_GRAPHICS_OBJECT tex0, tex1, shader;
    static struct LG_render_object ro[8];
    static float pos[2] = SCRPS;
    static float siz[2] = SCRSZ;
    union value v;
    unsigned int i;
    if(!initialized) {
        tex0 = LG_create_texture(texture0, 5, 10, LG_FT_NEAREST);
        tex1 = LG_create_texture(texture1, 5, 10, LG_FT_NEAREST);
        shader = LG_compile_shader(vscoreshader, fscoreshader);
        for(i=0; i<8; i++) {
            ro[i] = LG_create_render_object(vertices, 32, indices, 6, LG_DT_STREAM);
            LG_render_object_add_shader(&ro[i], shader);
            LG_render_object_add_parent(&ro[i],renderer);
        }
        initialized = true;
    }
    score+=increment;
    pos[0] = 0.6-siz[0]*16;
    if(render) {
        for(i=0; i < 8; i++) {
            LG_render_object_add_texture(&ro[i], ((score >> 7-i) & 1) ? tex1 : tex0);
            v.fv = pos;
            LG_render_object_prepare_shader_parameter(
                &ro[i], "translation", v, LG_VT_VEC2);
            v.fv = siz;
            LG_render_object_prepare_shader_parameter(
                &ro[i], "scale", v, LG_VT_VEC2);
            LG_render_object_draw(&ro[i]);
            pos[0] += siz[0]*2;
        }
    }
    if(deinit) {
        for(i=0; i<8; i++) {
            LG_destroy_render_object(&ro[i]);
        }
        LG_discard_shader(shader);
        LG_destroy_texture(tex0);
        LG_destroy_texture(tex1);
        initialized = false;
    }
    
}


static const char *vertex_src =
    "#version 330 core\n"
    "layout (location=0) in vec3 layoutPosition;\n"
    "layout (location=1) in vec3 layoutColor;\n"
    "out vec4 outVertexColor;\n"
    "uniform vec2 translation;\n"
    "uniform vec2 scale;\n"
    "void main() {\n"
    "   gl_Position = vec4(vec3(translation, 0)+"
    "       (layoutPosition*vec3(scale,0)), 1.0);\n"
    "   outVertexColor = vec4(layoutColor, 1.0);\n"
    "}",
*fragment_src =
    "#version 330 core\n"
    "out vec4 outFragmentColor;\n"
    "in vec4 outVertexColor;\n"
    "void main() {\n"
    "   outFragmentColor = outVertexColor;\n"
    "}";

struct game_object {
    struct LG_render_object ro;
    float px, py, sx, sy;
};

void go_shader_init(struct game_object *go) {
    float p[2] = {go->px, go->py};
    float s[2] = {go->sx, go->sy};
    union value v;
    v.fv = p;
    LG_render_object_prepare_shader_parameter(
        &go->ro,
        "translation",
        v,
        LG_VT_VEC2
    );
    v.fv = s;
    LG_render_object_prepare_shader_parameter(
        &go->ro,
        "scale",
        v,
        LG_VT_VEC2
    );   
}

#define PADSPEED 2
#define PADWIDTH 0.02
#define PADHEIGHT 0.1
#define BALLWIDTH 0.02
#define BALLHEIGHT 0.02
#define BALLSPEED 1
#define MINBALLSPEED 1
int main(int argc, char **argv) {
    struct LG_renderer renderer;
    struct game_object pad1, pad2, ball;
    float ball_velocity_x, ball_velocity_y;
    float p1v, p2v;
    p1v = 0;
    p2v = 0;
    LG_GRAPHICS_OBJECT shader;
    srand(time(NULL));
    renderer = LG_create_renderer(
        800,
        800,
        "GLONG",
        0,
        1
    );
    pad1.px = -.7;
    pad1.py = 0;
    pad1.sx = PADWIDTH;
    pad1.sy = PADHEIGHT;
    pad2.px = .7;
    pad2.py = 0;
    pad2.sx = PADWIDTH;
    pad2.sy = PADHEIGHT;
    ball.px = 0;
    ball.py = 0;
    ball.sx = BALLWIDTH;
    ball.sy = BALLHEIGHT;
    ball_velocity_x = (rand()%(int)(BALLSPEED*1000))/1000.0 - 0.5*BALLSPEED;
    ball_velocity_y = (rand()%(int)(BALLSPEED*1000))/1000.0 - 0.5*BALLSPEED;
    if(ball_velocity_x > 0 && ball_velocity_x < MINBALLSPEED)
        ball_velocity_x=MINBALLSPEED;
    if(ball_velocity_x < 0 && ball_velocity_x > -MINBALLSPEED)
        ball_velocity_x=-MINBALLSPEED;
    if(ball_velocity_y > 0 && ball_velocity_y < MINBALLSPEED)
        ball_velocity_y=MINBALLSPEED;
    if(ball_velocity_y < 0 && ball_velocity_y > -MINBALLSPEED)
        ball_velocity_y=-MINBALLSPEED;


    LG_renderer_make_current(&renderer);
    pad1.ro = LG_create_render_object(vertices, 32, indices, 6, LG_DT_STREAM);
    pad2.ro = LG_create_render_object(vertices, 32, indices, 6, LG_DT_STREAM);
    ball.ro = LG_create_render_object(vertices, 32, indices, 6, LG_DT_STREAM);
    shader   = LG_compile_shader(vertex_src, fragment_src); 
    score1(0, false, false, &renderer);
    score2(0, false, false, &renderer);
    LG_render_object_add_shader(&pad1.ro, shader);
    LG_render_object_add_shader(&pad2.ro, shader);
    LG_render_object_add_shader(&ball.ro, shader);
    LG_render_object_add_parent(&pad1.ro, &renderer);
    LG_render_object_add_parent(&pad2.ro, &renderer);
    LG_render_object_add_parent(&ball.ro, &renderer);
    while(LG_is_renderer_window_closed(renderer)) {
        LG_poll();
        LG_renderer_refresh(renderer);
        go_shader_init(&pad2);
        LG_render_object_draw(&pad2.ro);
        go_shader_init(&pad1); 
        LG_render_object_draw(&pad1.ro);
        go_shader_init(&ball);
        LG_render_object_draw(&ball.ro);
        score1(0, true, false, NULL);
        score2(0, true, false, NULL);
        if(LG_get_key_state(LG_KT_W) == LG_KS_PRESSED) {
            p1v = PADSPEED*pad1.ro.parent_renderers[0]->deltatime;
            pad1.py += p1v;
            if(pad1.py > 1-pad1.sy) pad1.py = 1-pad1.sy;
        }
        if(LG_get_key_state(LG_KT_S) == LG_KS_PRESSED) {
            p1v = PADSPEED*pad1.ro.parent_renderers[0]->deltatime;
            pad1.py -= p1v;
            if(pad1.py < -1+pad1.sy) pad1.py = -1+pad1.sy;
        }
        if(LG_get_key_state(LG_KT_UP) == LG_KS_PRESSED) {
            p2v = PADSPEED*pad2.ro.parent_renderers[0]->deltatime;
            pad2.py += p2v;
            if(pad2.py > 1-pad1.sy) pad2.py = 1-pad2.sy;
        }
        if(LG_get_key_state(LG_KT_DOWN) == LG_KS_PRESSED) {
            p2v = PADSPEED*pad2.ro.parent_renderers[0]->deltatime;
            pad2.py -= p2v;
            if(pad2.py < -1+pad2.sy) pad2.py = -1+pad2.sy;
        }
        ball.px += ball_velocity_x*ball.ro.parent_renderers[0]->deltatime;
        ball.py += ball_velocity_y*ball.ro.parent_renderers[0]->deltatime;
        if(ball.px > 0.7-PADWIDTH && ball.px < 0.7+PADWIDTH &&
                ball.py > pad2.py-pad2.sy && ball.py < pad2.py+pad2.sy) {
            ball.px = 0.7-PADWIDTH;
            ball_velocity_x = -(ball_velocity_x+p2v/2);
            ball_velocity_y += p2v;
        }
        if(ball.px < -0.7+PADWIDTH && ball.px > -0.7-PADWIDTH &&
                ball.py > pad1.py-pad1.sy && ball.py < pad1.py+pad1.sy) {
            ball.px = -0.7+PADWIDTH;
            ball_velocity_x = -(ball_velocity_x+p1v/2);
            ball_velocity_y += p1v;
        }
        if(ball.py > 1-BALLWIDTH) {
            ball.py = 1-BALLWIDTH;
            ball_velocity_y = -ball_velocity_y;
        }
        if(ball.py < -1+BALLWIDTH) {
            ball.py = -1+BALLWIDTH;
            ball_velocity_y = -ball_velocity_y;
        }
        if(ball.px > 1 || ball.px < -1) {
            if(ball.px > 1) score1(1, false, false, NULL);
            if(ball.px < -1) score2(1, false, false, NULL);
            ball.px = ball.py = 0;
            ball_velocity_x = (rand()%(int)(BALLSPEED*1000))/1000.0 - 0.5*BALLSPEED;
            ball_velocity_y = (rand()%(int)(BALLSPEED*1000))/1000.0 - 0.5*BALLSPEED;
            if(ball_velocity_x > 0 && ball_velocity_x < MINBALLSPEED)
                ball_velocity_x=MINBALLSPEED;
            if(ball_velocity_x < 0 && ball_velocity_x > -MINBALLSPEED)
                ball_velocity_x=-MINBALLSPEED;
            if(ball_velocity_y > 0 && ball_velocity_y < MINBALLSPEED)
                ball_velocity_y=MINBALLSPEED;
            if(ball_velocity_y < 0 && ball_velocity_y > -MINBALLSPEED)
                ball_velocity_y=-MINBALLSPEED;
            pad1.px = -0.7;
            pad1.py = 0;
            pad2.px = 0.7;
            pad2.py = 0;
        }
    }
    score1(0, false, true, NULL);
    score2(0, false, true, NULL);
    LG_destroy_render_object(&pad1.ro);
    LG_destroy_render_object(&pad2.ro);
    LG_destroy_render_object(&ball.ro);
    LG_discard_shader(shader);
    LG_destroy_renderer(&renderer);
    LG_terminate();
    return 0;
}
