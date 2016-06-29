#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "vec2.c"
#include "util.c"
#include "glutils.c"

#define WIDTH 800
#define HEIGHT 600

float position[] = {
     0.0f,  0.0f, // pivot
     0.0f, -0.5f, // mass
};

GLuint lines_index[2];

static struct {
  GLuint vertex_buffer, index_buffer;

  GLuint vertex_shader, fragment_shader, program;

  struct {
    struct {
      GLuint position;
    } attributes;

  } mass;

  /* double xpos, ypos; */

  /* mouse_button button; */

  bool pause;

} g_gl_state;

static int
make_resources() {
  /* Create buffers */
  g_gl_state.vertex_buffer = make_buffer(GL_ARRAY_BUFFER,
                                         position,
                                         sizeof(position));
  g_gl_state.index_buffer  = make_buffer(GL_ELEMENT_ARRAY_BUFFER,
                                         lines_index,
                                         sizeof(lines_index));

  /* Compile GLSL program  */
  g_gl_state.vertex_shader = make_shader(GL_VERTEX_SHADER,
                                         "line.vert");
  g_gl_state.fragment_shader = make_shader(GL_FRAGMENT_SHADER,
                                           "line.frag");
  g_gl_state.program = make_program(g_gl_state.vertex_shader,
                                    g_gl_state.fragment_shader);

  /* Look up shader variable locations */
  g_gl_state.mass.attributes.position =
    glGetAttribLocation(g_gl_state.program, "position");

  return 1;
}

static void
render(GLFWwindow *window) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(g_gl_state.program);

  glBindBuffer(GL_ARRAY_BUFFER, g_gl_state.vertex_buffer);
  glEnableVertexAttribArray(g_gl_state.mass.attributes.position);
  glVertexAttribPointer(g_gl_state.mass.attributes.position,
                        2, GL_FLOAT, GL_FALSE,
                        0, 0);
  glDrawArrays(GL_LINES, 0, 2);

  glDisableVertexAttribArray(g_gl_state.mass.attributes.position);

  glfwSwapBuffers(window);
}

/* System of diffeqs that specifies how the pendulum moves */
vec2 pendulum(vec2 current) {
  vec2 result;

  result.x = current.y;                 /* theta' = v */
  result.y = -sinf(current.x);          /* v' = -sin(theta) */

  return result;
}

vec2
rk4_weighted_avg(vec2 a, vec2 b, vec2 c, vec2 d) {
  vec2 result;

  result.x = (a.x + 2*b.x + 2*c.x + d.x) / 6.0;
  result.y = (a.y + 2*b.y + 2*c.y + d.y) / 6.0;

  return result;
}

/* Compute next step of autonomous differential equation. */
vec2
rk4(vec2 current, float dt) {
  vec2 k1 = pendulum(current);
  vec2 k2 = pendulum(vec2_add(current, vec2_scale(dt/2, k1)));
  vec2 k3 = pendulum(vec2_add(current, vec2_scale(dt/2, k2)));
  vec2 k4 = pendulum(vec2_add(current, vec2_scale(dt, k3)));

  vec2 k = rk4_weighted_avg(k1, k2, k3, k4);

  vec2 result = vec2_add(current, vec2_scale(dt, k));

  return result;
}


int main() {
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(WIDTH, HEIGHT, "Pendulum", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  if (gl3wInit() != 0) {
    fprintf(stderr, "GL3W: failed to initialize\n");
    return 1;
  }

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  make_resources();
  g_gl_state.pause = false;

  vec2 init = {.x = 0.2,
               .y = 1.0};
  vec2 current = init;
  float dt = 0.05;
  float radius = 0.5f;
  while (!glfwWindowShouldClose(window)) {
    if (!g_gl_state.pause) {

    }

    current = rk4(current, dt);
    // printf("%f %f\n", current.x, current.y);
    position[2] = radius * sinf(current.x);
    position[3] = -radius * cosf(current.x);


    glBindBuffer(GL_ARRAY_BUFFER, g_gl_state.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(position), position, GL_DYNAMIC_DRAW);

    render(window);

    glfwPollEvents();
  }

}
