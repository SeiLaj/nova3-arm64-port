// Stubs vazios para GLES - removem dependência
extern "C" {
    void glShaderSource(unsigned int, int, const char**, const int*) {}
    void glCompileShader(unsigned int) {}
    unsigned int glCreateShader(unsigned int) { return 0; }
    unsigned int glCreateProgram() { return 0; }
    void glAttachShader(unsigned int, unsigned int) {}
    void glLinkProgram(unsigned int) {}
    void glGetProgramiv(unsigned int, unsigned int, int*) {}
    void glUseProgram(unsigned int) {}
    int glGetUniformLocation(unsigned int, const char*) { return 0; }
    void glUniform4f(int, float, float, float, float) {}
    int glGetAttribLocation(unsigned int, const char*) { return 0; }
    void glEnableVertexAttribArray(unsigned int) {}
    void glDisableVertexAttribArray(unsigned int) {}
    void glVertexAttribPointer(unsigned int, int, unsigned int, unsigned char, int, const void*) {}
    void glDrawArrays(unsigned int, int, int) {}
}
