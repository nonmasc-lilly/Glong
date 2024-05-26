#include "inc/main.h"

struct LG_renderer LG_create_renderer(unsigned int width, unsigned int height,
        const char *name, LG_COLOR color, unsigned int swap_interval) {
    struct LG_renderer ret;
    static bool glfw_initiated = false;
    static bool glad_initiated = false;
    if(!glfw_initiated) {
        glfwInit();
        glfw_initiated = true;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,                        3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,                        3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE,                           GLFW_FALSE);
    ret.window = glfwCreateWindow(width, height, name, NULL, NULL);
    if(ret.window == NULL) {
        LG_REPORT(LG_REPORT_ERROR, "Could not create window\n");
        memset(&ret, 0, sizeof(ret));
        return ret;
    }
    glfwMakeContextCurrent(ret.window);
    if(!glad_initiated && !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LG_REPORT(LG_REPORT_ERROR, "Could not initialize glad\n");
        memset(&ret, 0, sizeof(ret));
        return ret;
    }
    glad_initiated = true;
    ret.active = true;
    ret.is_current = false;
    ret.color = color;
    ret.swap_interval = swap_interval;
    return ret;
}

void LG_destroy_renderer(struct LG_renderer *renderer) {
    if(!renderer->active) return;
    renderer->active = false;
    glfwDestroyWindow(renderer->window);
    renderer->window = NULL;
}

bool LG_is_renderer_window_closed(struct LG_renderer render) {
    if(!render.active) return true;
    return !glfwWindowShouldClose(render.window);
}

struct LG_renderer *LG_renderer_make_current(struct LG_renderer *render) {
    static struct LG_renderer *current = NULL;
    if(!render) return current;
    if(current) {
        if(render == current) return render;
        if(current->active) current->is_current = false;
    }
    glfwMakeContextCurrent(render->window);
    glClearColor(
        LG_COLOR_BREAKDOWN_F(render->color, LG_RED_FROM_COLOR),
        LG_COLOR_BREAKDOWN_F(render->color, LG_GREEN_FROM_COLOR),
        LG_COLOR_BREAKDOWN_F(render->color, LG_BLUE_FROM_COLOR),
        LG_COLOR_BREAKDOWN_F(render->color, LG_ALPHA_FROM_COLOR)
    );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    render->is_current = true;
    current = render;
    glfwSwapInterval(render->swap_interval);
    render->last_time = glfwGetTime();
    render->deltatime = 0;
    return render;
}

void LG_poll() {
    glfwPollEvents();
}

void LG_renderer_refresh(struct LG_renderer render) {
    if(!render.is_current) {
        LG_REPORT(
            LG_REPORT_WARNING,
            "attempting to refresh non-current renderer, "
            "deltatime will not be available.\n"
        );
    } else LG_deltatime();
    glfwSwapBuffers(render.window);
    glClear(GL_COLOR_BUFFER_BIT);
}

void LG_terminate() {
    glfwTerminate();
}

LG_GRAPHICS_OBJECT LG_compile_shader(const char *vertex_src,
        const char *fragment_src) {
    LG_GRAPHICS_OBJECT vs, fs, ret;
    unsigned int vlen, flen;
    int success;
    char infolog[1024];
    vlen = strlen(vertex_src);
    flen = strlen(fragment_src);
    vs = glCreateShader(GL_VERTEX_SHADER);
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vs, 1, &vertex_src,   &vlen);
    glShaderSource(fs, 1, &fragment_src, &flen);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vs, 1024, NULL, infolog);
        LG_FREPORT(
            LG_REPORT_ERROR,
            "vertex shader could not compile:: %s\n",
            infolog
        );
        return 0;
    }
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fs, 1024, NULL, infolog);
        LG_FREPORT(
            LG_REPORT_ERROR,
            "fragment shader could not compile:: %s\n",
            infolog
        );
        return 0;
    }
    ret = glCreateProgram();
    glAttachShader(ret, vs);
    glAttachShader(ret, fs);
    glLinkProgram(ret);
    glGetProgramiv(ret, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(ret, 1024, NULL, infolog);
        LG_FREPORT(
            LG_REPORT_ERROR,
            "could not link shaders:: %s\n",
            infolog
        );
        return 0;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return ret;
}

void LG_discard_shader(LG_GRAPHICS_OBJECT shader) {
    glDeleteProgram(shader);
}

struct LG_render_object LG_create_render_object(
        const float *vertices, unsigned int vertices_len,
        const unsigned int *indices, unsigned int indices_len,
        enum LG_draw_type draw_type) {
    struct LG_render_object ret;
    glCreateVertexArrays(1, &ret.vao);
    glCreateBuffers(1, &ret.vbo);
    glCreateBuffers(1, &ret.ebo);
    ret.shader               =    0;
    ret.texture              =    0;
    ret.parent_renderers_len =    0;
    ret.parent_renderers     = NULL;
    ret.index_num            = indices_len;
    glBindVertexArray(ret.vao);
    glBindBuffer(GL_ARRAY_BUFFER, ret.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret.ebo);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices_len*sizeof(float),
        vertices,
        draw_type
    );
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices_len*sizeof(unsigned int),
        indices,
        draw_type
    );
    /* position */
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        LG_VERTEX_STRIDE,
        (void*)0
    );
    /* color */
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        LG_VERTEX_STRIDE,
        (void*)(3*sizeof(float))
    );
    /* uv */
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        LG_VERTEX_STRIDE,
        (void*)(6*sizeof(float))
    );
    return ret;
}

void LG_destroy_render_object(struct LG_render_object *render_object) {
    glDeleteVertexArrays(1, &render_object->vao);
    glDeleteBuffers(1, &render_object->vbo);
    glDeleteBuffers(1, &render_object->ebo);
    if(render_object->parent_renderers) free(render_object->parent_renderers);
    memset(render_object, 0, sizeof(*render_object));
}

void LG_render_object_add_texture(struct LG_render_object *render_object,
        LG_GRAPHICS_OBJECT texture) {
    render_object->texture = texture;
}

void LG_render_object_add_shader(struct LG_render_object *render_object,
        LG_GRAPHICS_OBJECT shader) {
    render_object->shader = shader;
}

void LG_render_object_add_parent(struct LG_render_object *render_object,
        struct LG_renderer *parent) {
    if(!render_object->parent_renderers)
        render_object->parent_renderers = malloc(sizeof(parent));
    else
        render_object->parent_renderers = realloc(
            render_object->parent_renderers,
            sizeof(parent)*render_object->parent_renderers_len
        );
    render_object->parent_renderers[render_object->parent_renderers_len] = parent;
    render_object->parent_renderers_len++;
}

void LG_render_object_draw(struct LG_render_object *render_object) {
    unsigned int i;
    struct LG_renderer *current_renderer;
    bool current;
    if(!render_object->shader) {
        LG_REPORT(LG_REPORT_ERROR, "cannot draw render object without shader\n");
        return;
    }
    if(render_object->texture) {
        glBindTexture(GL_TEXTURE_2D, render_object->texture);
        glActiveTexture(GL_TEXTURE0);
    }
    current_renderer = LG_renderer_make_current(NULL);
    for(i=0; i<render_object->parent_renderers_len; i++) {
        if(render_object->parent_renderers[i] == current_renderer) {
            current = true;
            break;
        }
    }
    if(!current) {
        LG_REPORT(
            LG_REPORT_WARNING,
            "drawing render_object that is not parented actively\n"
        );
    }
    
    glBindVertexArray(render_object->vao);
    glUseProgram(render_object->shader);
    glDrawElements(
        GL_TRIANGLES,
        render_object->index_num,
        GL_UNSIGNED_INT,
        (void*)0
    );
    if(render_object->texture) {
    }
}

void LG_render_object_prepare_shader_parameter(
        struct LG_render_object *render_object, const char *name, union value var,
        enum LG_type_name type) {
    int uniform_location;
    if(!render_object->shader) {
        LG_REPORT(LG_REPORT_ERROR, "cant set parameter of nonexistant shader\n");
        return;
    }
    glUseProgram(render_object->shader);
    uniform_location = glGetUniformLocation(render_object->shader, name);
    switch(type) {
    case LG_VT_INT:
        glUniform1i(uniform_location, var.i);
    case LG_VT_IVEC2:
        glUniform2i(uniform_location, var.iv[0], var.iv[1]);
        break;
    case LG_VT_IVEC3:
        glUniform3i(uniform_location, var.iv[0], var.iv[1], var.iv[2]);
        break;
    case LG_VT_IVEC4:
        glUniform4i(uniform_location, var.iv[0], var.iv[1], var.iv[2], var.iv[3]);
        break;
    case LG_VT_UINT:
        glUniform1ui(uniform_location, var.u);
    case LG_VT_UVEC2:
        glUniform2ui(uniform_location, var.uv[0], var.uv[1]);
        break;
    case LG_VT_UVEC3:
        glUniform3ui(uniform_location, var.uv[0], var.uv[1], var.uv[2]);
        break;
    case LG_VT_UVEC4:
        glUniform4ui(uniform_location, var.uv[0], var.uv[1], var.uv[2], var.uv[3]);
        break;
    case LG_VT_FLOAT:
        glUniform1f(uniform_location, var.f);
    case LG_VT_VEC2:
        glUniform2f(uniform_location, var.fv[0], var.fv[1]);
        break;
    case LG_VT_VEC3:
        glUniform3f(uniform_location, var.fv[0], var.fv[1], var.fv[2]);
        break;
    case LG_VT_VEC4:
        glUniform4f(uniform_location, var.fv[0], var.fv[1], var.fv[2], var.fv[3]);
        break;
    }
}

enum LG_key_state LG_get_key_state(enum LG_key_type key) {
    return glfwGetKey(LG_renderer_make_current(NULL)->window, key);
}

double LG_deltatime() {
    struct LG_renderer *cur;
    cur = LG_renderer_make_current(NULL);
    cur->deltatime = glfwGetTime() - cur->last_time;
    cur->last_time = glfwGetTime();
    return cur->deltatime;
}

LG_GRAPHICS_OBJECT LG_create_texture(const unsigned char *texture, unsigned int width,
        unsigned int height, enum LG_filter_type filter_type) {
    LG_GRAPHICS_OBJECT ret;
    glGenTextures(1, &ret);
    glBindTexture(GL_TEXTURE_2D, ret);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        texture
    );
    return ret;
}

void LG_destroy_texture(LG_GRAPHICS_OBJECT texture) {
    glDeleteTextures(1, &texture);
}







