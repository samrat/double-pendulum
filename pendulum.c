#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "vec2.c"
#include "util.c"

#define WIDTH 800
#define HEIGHT 600

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
  vec2 init = {.x = 0,
               .y = 0.5};
  vec2 current = init;
  float dt = 0.01;

  for (int i = 0; i < 400; i++) {
    current = rk4(current, dt);
    printf("%f %f\n", current.x, current.y);
  }
}
