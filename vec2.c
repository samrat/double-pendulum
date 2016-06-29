typedef struct {
  float x, y, z;

} vec2;

static vec2
vec2_add(vec2 a, vec2 b) {
  vec2 result;

  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;

  return result;
}

static vec2
vec2_scale(float t, vec2 a) {
  vec2 result;

  result.x = t * a.x;
  result.y = t * a.y;
  result.z = t * a.z;

  return result;
}
