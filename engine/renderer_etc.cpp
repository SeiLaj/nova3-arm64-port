#include <android/log.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define GL_TEXTURE_2D 0x0DE1
#define GL_ETC1_RGB8_OES 0x8D64
#define GL_UNSIGNED_BYTE 0x1401
#define GL_RGBA 0x1908
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_LINEAR 0x2601
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_TRIANGLE_STRIP 0x0005
#define GL_FLOAT 0x1406
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30

static void* g_gles;
static bool g_ok;
static unsigned g_prog, g_tex;
static int g_w, g_h;
static bool g_compressed;

static void (*pClearColor)(float,float,float,float);
static void (*pClear)(unsigned);
static unsigned (*pCreateShader)(unsigned);
static void (*pShaderSource)(unsigned,int,const char**,const int*);
static void (*pCompileShader)(unsigned);
static unsigned (*pCreateProgram)(void);
static void (*pAttachShader)(unsigned,unsigned);
static void (*pLinkProgram)(unsigned);
static void (*pUseProgram)(unsigned);
static int (*pGetUniformLocation)(unsigned,const char*);
static void (*pUniform1i)(int,int);
static int (*pGetAttribLocation)(unsigned,const char*);
static void (*pEnableVertexAttribArray)(unsigned);
static void (*pDisableVertexAttribArray)(unsigned);
static void (*pVertexAttribPointer)(unsigned,int,unsigned,unsigned char,int,const void*);
static void (*pDrawArrays)(unsigned,int,int);
static void (*pGenTextures)(int,unsigned*);
static void (*pBindTexture)(unsigned,unsigned);
static void (*pTexImage2D)(unsigned,int,int,int,int,int,unsigned,unsigned,const void*);
static void (*pCompressedTexImage2D)(unsigned,int,unsigned,int,int,int,int,const void*);
static void (*pTexParameteri)(unsigned,unsigned,int);
static unsigned (*pGetError)(void);

static const char* VS =
  "attribute vec4 a_pos;\nattribute vec2 a_uv;\nvarying vec2 v_uv;\n"
  "void main(){ gl_Position=a_pos; v_uv=a_uv; }\n";
static const char* FS =
  "precision mediump float;\nvarying vec2 v_uv;\nuniform sampler2D u_tex;\n"
  "void main(){ gl_FragColor=texture2D(u_tex,v_uv); }\n";

static bool load_gles() {
  const char* libs[] = {"libGLESv2.so","/system/lib64/libGLESv2.so",nullptr};
  for (int i=0; libs[i]; i++)
    if ((g_gles=dlopen(libs[i], RTLD_NOW))) break;
  if (!g_gles) return false;
  pClearColor=(void(*)(float,float,float,float))dlsym(g_gles,"glClearColor");
  pClear=(void(*)(unsigned))dlsym(g_gles,"glClear");
  pCreateShader=(unsigned(*)(unsigned))dlsym(g_gles,"glCreateShader");
  pShaderSource=(void(*)(unsigned,int,const char**,const int*))dlsym(g_gles,"glShaderSource");
  pCompileShader=(void(*)(unsigned))dlsym(g_gles,"glCompileShader");
  pCreateProgram=(unsigned(*)(void))dlsym(g_gles,"glCreateProgram");
  pAttachShader=(void(*)(unsigned,unsigned))dlsym(g_gles,"glAttachShader");
  pLinkProgram=(void(*)(unsigned))dlsym(g_gles,"glLinkProgram");
  pUseProgram=(void(*)(unsigned))dlsym(g_gles,"glUseProgram");
  pGetUniformLocation=(int(*)(unsigned,const char*))dlsym(g_gles,"glGetUniformLocation");
  pUniform1i=(void(*)(int,int))dlsym(g_gles,"glUniform1i");
  pGetAttribLocation=(int(*)(unsigned,const char*))dlsym(g_gles,"glGetAttribLocation");
  pEnableVertexAttribArray=(void(*)(unsigned))dlsym(g_gles,"glEnableVertexAttribArray");
  pDisableVertexAttribArray=(void(*)(unsigned))dlsym(g_gles,"glDisableVertexAttribArray");
  pVertexAttribPointer=(void(*)(unsigned,int,unsigned,unsigned char,int,const void*))dlsym(g_gles,"glVertexAttribPointer");
  pDrawArrays=(void(*)(unsigned,int,int))dlsym(g_gles,"glDrawArrays");
  pGenTextures=(void(*)(int,unsigned*))dlsym(g_gles,"glGenTextures");
  pBindTexture=(void(*)(unsigned,unsigned))dlsym(g_gles,"glBindTexture");
  pTexImage2D=(void(*)(unsigned,int,int,int,int,int,unsigned,unsigned,const void*))dlsym(g_gles,"glTexImage2D");
  pCompressedTexImage2D=(void(*)(unsigned,int,unsigned,int,int,int,int,const void*))dlsym(g_gles,"glCompressedTexImage2D");
  pTexParameteri=(void(*)(unsigned,unsigned,int))dlsym(g_gles,"glTexParameteri");
  pGetError=(unsigned(*)(void))dlsym(g_gles,"glGetError");
  return pClear && pCompressedTexImage2D;
}

static bool load_etc(const char* path) {
  FILE* f = fopen(path, "rb");
  if (!f) { LOGE("open fail %s", path); return false; }
  fseek(f, 0, SEEK_END);
  long sz = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (sz < 52) { fclose(f); return false; }
  unsigned char* buf = (unsigned char*)malloc((size_t)sz);
  if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { free(buf); fclose(f); return false; }
  fclose(f);

  uint32_t hdr, w, h, fmt, aux, plen;
  memcpy(&hdr, buf+0, 4);
  memcpy(&w,   buf+4, 4);
  memcpy(&h,   buf+8, 4);
  memcpy(&fmt, buf+12, 4);
  memcpy(&aux, buf+16, 4);
  memcpy(&plen,buf+20, 4);
  if (hdr != 52 || w == 0 || h == 0 || hdr + plen > (uint32_t)sz) {
    LOGE("bad etc header");
    free(buf);
    return false;
  }

  uint32_t level0 = (w/4)*(h/4)*8;
  if (level0 > plen) level0 = plen;

  pGenTextures(1, &g_tex);
  pBindTexture(GL_TEXTURE_2D, g_tex);
  while (pGetError()) {}
  pCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES,
                        (int)w, (int)h, 0, (int)level0, buf + hdr);
  unsigned err = pGetError();
  if (err) {
    LOGE("CompressedTex err=0x%x fallback", err);
    int cw=64, ch=64;
    unsigned char* px = (unsigned char*)malloc((size_t)(cw*ch*4));
    for (int y=0;y<ch;y++) for (int x=0;x<cw;x++) {
      int i=(y*cw+x)*4, c=((x/8)+(y/8))&1;
      px[i]=c?255:40; px[i+1]=c?40:255; px[i+2]=c?200:40; px[i+3]=255;
    }
    pTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cw, ch, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
    free(px);
    g_compressed = false;
    g_w = cw; g_h = ch;
  } else {
    g_compressed = true;
    g_w = (int)w; g_h = (int)h;
    LOGI("ETC1 OK %ux%u level0=%u fmt=%u", w, h, level0, fmt);
  }
  pTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  pTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  pTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  pTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  free(buf);
  return true;
}

static bool init_prog() {
  unsigned vs = pCreateShader(GL_VERTEX_SHADER);
  unsigned fs = pCreateShader(GL_FRAGMENT_SHADER);
  pShaderSource(vs, 1, &VS, nullptr); pCompileShader(vs);
  pShaderSource(fs, 1, &FS, nullptr); pCompileShader(fs);
  g_prog = pCreateProgram();
  pAttachShader(g_prog, vs); pAttachShader(g_prog, fs); pLinkProgram(g_prog);
  return g_prog != 0;
}

extern "C" bool init_renderer() {
  if (g_ok) return true;
  if (!load_gles()) { LOGE("GLES fail"); return false; }
  if (!init_prog()) return false;
  const char* paths[] = {
    "/data/data/com.gameloft.android.ANMP.GloftN3HM/files/SF_white_standard.etc",
    "/storage/emulated/0/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/SF_white_standard.etc",
    "/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/SF_white_standard.etc",
    nullptr
  };
  for (int i=0; paths[i]; i++)
    if (load_etc(paths[i])) break;
  if (!g_tex) {
    /* fallback_checker: sem ficheiro → xadrez 64x64 */
    LOGI("no ETC file — using checkerboard");
    int cw=64, ch=64;
    unsigned char* px = (unsigned char*)malloc((size_t)(cw*ch*4));
    if (px && pGenTextures && pTexImage2D) {
      for (int y=0;y<ch;y++) for (int x=0;x<cw;x++) {
        int i=(y*cw+x)*4, c=((x/8)+(y/8))&1;
        px[i]=c?255:40; px[i+1]=c?40:255; px[i+2]=c?200:40; px[i+3]=255;
      }
      pGenTextures(1, &g_tex);
      pBindTexture(GL_TEXTURE_2D, g_tex);
      pTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cw, ch, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
      pTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      pTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      g_w=cw; g_h=ch; g_compressed=false;
    }
    free(px);
  }

    g_ok = true;
  LOGI("Renderer ready tex=%u %dx%d compressed=%d", g_tex, g_w, g_h, (int)g_compressed);
  return true;
}

extern "C" void render_textured_quad() {
  if (!g_ok) init_renderer();
  if (!g_tex) return;
  pClearColor(0.05f, 0.05f, 0.08f, 1);
  pClear(GL_COLOR_BUFFER_BIT);
  pUseProgram(g_prog);
  pUniform1i(pGetUniformLocation(g_prog, "u_tex"), 0);
  pBindTexture(GL_TEXTURE_2D, g_tex);
  float v[] = {
    -0.9f,-0.9f,0, 0,0,
     0.9f,-0.9f,0, 1,0,
    -0.9f, 0.9f,0, 0,1,
     0.9f, 0.9f,0, 1,1
  };
  int pos = pGetAttribLocation(g_prog, "a_pos");
  int uv  = pGetAttribLocation(g_prog, "a_uv");
  pEnableVertexAttribArray(pos);
  pVertexAttribPointer(pos, 3, GL_FLOAT, 0, 20, v);
  pEnableVertexAttribArray(uv);
  pVertexAttribPointer(uv, 2, GL_FLOAT, 0, 20, v+3);
  pDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  pDisableVertexAttribArray(pos);
  pDisableVertexAttribArray(uv);
}
