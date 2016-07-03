#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "vec4.c"
#include "util.c"
#include "glutils.c"

#define WIDTH 600
#define HEIGHT 600
#define TAIL_LENGTH 100

float l1 = 1;
float l2 = 0.5;


float position[] = {
     0.0f,  0.0f, // pivot
     0.0f, -0.5f, // mass_1
     0.0f, 0.7f,  // mass_2
};

float tail1[TAIL_LENGTH][2];
float tail2[TAIL_LENGTH][2];
int tail_pos[TAIL_LENGTH];

static struct {
  GLuint vertex_buffer, tail_index_buffer;

  /* Same buffer is used for both tails */
  GLuint tail_buffer;

  GLuint vertex_shader, fragment_shader, program;

  GLuint tail_vertex_shader, tail_fragment_shader, tail_program;

  struct {
    struct {
      GLuint position;
    } attributes;

  } mass;

  struct {
    struct {
      GLuint position;
    } attributes;

    struct {
      GLuint color;
    } uniforms;
  } tail;

  /* double xpos, ypos; */

  /* mouse_button button; */

  bool pause, draw_tails;

} g_gl_state;

static int
make_resources() {
  /* Create buffers */
  g_gl_state.vertex_buffer = make_buffer(GL_ARRAY_BUFFER,
                                         position,
                                         sizeof(position));
  g_gl_state.tail_buffer = make_buffer(GL_ARRAY_BUFFER,
                                       tail1,
                                       sizeof(tail1));
  g_gl_state.tail_index_buffer = make_buffer(GL_ELEMENT_ARRAY_BUFFER,
                                             tail_pos,
                                             sizeof(tail_pos));

  /* Compile GLSL program  */
  g_gl_state.vertex_shader = make_shader(GL_VERTEX_SHADER,
                                         "line.vert");
  g_gl_state.fragment_shader = make_shader(GL_FRAGMENT_SHADER,
                                           "line.frag");
  g_gl_state.program = make_program(g_gl_state.vertex_shader,
                                    g_gl_state.fragment_shader);


  g_gl_state.tail_vertex_shader = make_shader(GL_VERTEX_SHADER,
                                              "tail.vert");
  g_gl_state.tail_fragment_shader = make_shader(GL_FRAGMENT_SHADER,
                                                "tail.frag");
  g_gl_state.tail_program = make_program(g_gl_state.tail_vertex_shader,
                                         g_gl_state.tail_fragment_shader);

  /* Look up shader variable locations */
  g_gl_state.mass.attributes.position =
    glGetAttribLocation(g_gl_state.program, "position");

  g_gl_state.tail.attributes.position =
    glGetAttribLocation(g_gl_state.tail_program, "position");

  g_gl_state.tail.uniforms.color =
    glGetUniformLocation(g_gl_state.tail_program, "color");

  return 1;
}

static void
render(GLFWwindow *window) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(g_gl_state.program);

  glBindBuffer(GL_ARRAY_BUFFER, g_gl_state.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(position), position, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(g_gl_state.mass.attributes.position);
  glVertexAttribPointer(g_gl_state.mass.attributes.position,
                        2, GL_FLOAT, GL_FALSE,
                        0, 0);
  glDrawArrays(GL_LINE_STRIP, 0, 3);

  glPointSize(16.0f);
  glVertexAttribPointer(g_gl_state.mass.attributes.position,
                        2, GL_FLOAT, GL_FALSE,
                        0, (GLvoid*)(2*sizeof(float)));
  glDrawArrays(GL_POINTS, 0, 2);
  glDisableVertexAttribArray(g_gl_state.mass.attributes.position);

  if (g_gl_state.draw_tails) {
    /* Tail 1 */
    glUseProgram(g_gl_state.tail_program);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_gl_state.tail_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(tail_pos), tail_pos, GL_DYNAMIC_DRAW);

    glUniform1f(g_gl_state.tail.uniforms.color,
                0.1f);
    glBindBuffer(GL_ARRAY_BUFFER, g_gl_state.tail_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(tail1), tail1, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(g_gl_state.tail.attributes.position);
    glVertexAttribPointer(g_gl_state.mass.attributes.position,
                          2, GL_FLOAT, GL_FALSE,
                          0, 0);
    // glDrawArrays(GL_LINE_STRIP, 0, TAIL_LENGTH);
    glDrawElements(GL_LINE_STRIP,
                   TAIL_LENGTH,
                   GL_UNSIGNED_INT,
                   0);

    /* Tail 2 */
    glUniform1f(g_gl_state.tail.uniforms.color,
                0.5f);
    glBindBuffer(GL_ARRAY_BUFFER, g_gl_state.tail_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(tail2), tail2, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(g_gl_state.tail.attributes.position);
    glVertexAttribPointer(g_gl_state.mass.attributes.position,
                          2, GL_FLOAT, GL_FALSE,
                          0, 0);
    glDrawElements(GL_LINE_STRIP,
                   TAIL_LENGTH,
                   GL_UNSIGNED_INT,
                   0);
  }
  glfwSwapBuffers(window);
}

/* System of diffeqs that specifies how the pendulum moves */
vec4 pendulum(vec4 current) {
  vec4 result;

  float m1 = 1;
  float m2 = 1;
  float g = 9.8f;

  float th1 = current.x;
  float v1 = current.y;
  float th2 = current.z;
  float v2 = current.w;

  float del = th2 - th1; /* del = theta_2 - theta_1 */

  float den1 = (m1+m2)*l1 - m2*l1*cosf(del)*cosf(del);

  result.x = v1;                 /* theta' = v */
  result.y = (m2*l1*v1*v1*sinf(del)*cosf(del) + m2*g*sinf(th2)*cosf(del) + m2*l2*v2*v2*sinf(del) - (m1+m2)*g*sinf(th1)) / den1;

  float den2 = (m1+m2)*l2 - m2*l2*cosf(del)*cosf(del);
  result.z = v2;
  result.w = (-m2*l2*v2*v2*sinf(del)*cosf(del) + (m1+m2)*(g*sinf(th1)*cosf(del) - l1*v1*v1*sinf(del) - g*sinf(th2))) / den2;

  return result;
}

vec4
rk4_weighted_avg(vec4 a, vec4 b, vec4 c, vec4 d) {
  vec4 result;

  result.x = (a.x + 2*b.x + 2*c.x + d.x) / 6.0;
  result.y = (a.y + 2*b.y + 2*c.y + d.y) / 6.0;
  result.z = (a.z + 2*b.z + 2*c.z + d.z) / 6.0;
  result.w = (a.w + 2*b.w + 2*c.w + d.w) / 6.0;

  return result;
}

/* Compute next step of autonomous differential equation. */
vec4
rk4(vec4 current, float dt) {
  vec4 k1 = pendulum(current);
  vec4 k2 = pendulum(vec4_add(current, vec4_scale(dt/2, k1)));
  vec4 k3 = pendulum(vec4_add(current, vec4_scale(dt/2, k2)));
  vec4 k4 = pendulum(vec4_add(current, vec4_scale(dt, k3)));

  vec4 k = rk4_weighted_avg(k1, k2, k3, k4);

  vec4 result = vec4_add(current, vec4_scale(dt, k));

  return result;
}

void
key_callback(GLFWwindow *window, int key,
             int scancode, int action, int mods) {
  if (action == GLFW_RELEASE) {
    switch(key) {
    case GLFW_KEY_T: {
      g_gl_state.draw_tails = !g_gl_state.draw_tails;
    } break;

    case GLFW_KEY_P: {
      g_gl_state.pause = !g_gl_state.pause;
    } break;

    }
  }
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

  glfwSetKeyCallback(window, key_callback);

  if (gl3wInit() != 0) {
    fprintf(stderr, "GL3W: failed to initialize\n");
    return 1;
  }

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  make_resources();
  g_gl_state.pause = false;
  g_gl_state.draw_tails = false;

  vec4 init = {.x = 2.8,
               .y = -1.2,
               .z = 0.1,
               .w = 2.0};
  vec4 current = init;
  float dt = 0.01;
  float scale = 0.4f;

  int tail_index = 0;

  for (int i = 0; i < TAIL_LENGTH; i++)
    tail_pos[i] = i;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    if (g_gl_state.pause) {
      continue;
    }

    current = rk4(current, dt);

    position[2] = scale * l1 * sinf(current.x);
    position[3] = -scale * l1 * cosf(current.x);
    tail1[tail_index][0] = position[2];
    tail1[tail_index][1] = position[3];

    position[4] = scale*(l1*sinf(current.x) + l2*sinf(current.z));
    position[5] = -scale*(l1*cosf(current.x) + l2*cosf(current.z));
    tail2[tail_index][0] = position[4];
    tail2[tail_index][1] = position[5];

    tail_index = (tail_index + 1) % TAIL_LENGTH;

    /* Shift indices for line strip. 0th element needs to point to
       latest point, and so forth. Otherwise you'll see a loop. */
    for (int i = 0; i < TAIL_LENGTH; i++) {
      tail_pos[i] = (tail_index + i) % TAIL_LENGTH;
    }

    render(window);

  }

}
