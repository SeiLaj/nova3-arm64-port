#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static const char* vertex_shader =
    "attribute vec4 a_position;\n"
    "void main() {\n"
    "  gl_Position = a_position;\n"
    "}\n";

static const char* fragment_shader =
    "precision mediump float;\n"
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "  gl_FragColor = u_color;\n"
    "}\n";

static GLuint g_program = 0;
static GLuint g_color_loc = 0;
static bool g_initialized = false;

bool init_renderer() {
    if (g_initialized) return true;
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, nullptr);
    glCompileShader(vs);
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, nullptr);
    glCompileShader(fs);
    
    g_program = glCreateProgram();
    glAttachShader(g_program, vs);
    glAttachShader(g_program, fs);
    glLinkProgram(g_program);
    
    GLint status;
    glGetProgramiv(g_program, GL_LINK_STATUS, &status);
    if (!status) {
        LOGE("Failed to link shader program");
        return false;
    }
    
    g_color_loc = glGetUniformLocation(g_program, "u_color");
    g_initialized = true;
    LOGI("Renderer initialized");
    return true;
}

void render_square() {
    if (!g_initialized) return;
    
    glUseProgram(g_program);
    glUniform4f(g_color_loc, 1.0f, 0.0f, 0.0f, 1.0f); // Vermelho
    
    // Quadrado: -0.5 a 0.5
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
    };
    
    GLint pos_loc = glGetAttribLocation(g_program, "a_position");
    glEnableVertexAttribArray(pos_loc);
    glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(pos_loc);
}
