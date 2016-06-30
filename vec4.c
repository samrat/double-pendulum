typedef struct {
  float x, y, z, w;

} vec4;

static vec4
vec4_add(vec4 a, vec4 b) {
  vec4 result;

  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  result.w = a.w + b.w;

  return result;
}

static vec4
vec4_scale(float t, vec4 a) {
  vec4 result;

  result.x = t * a.x;
  result.y = t * a.y;
  result.z = t * a.z;
  result.w = t * a.w;

  return result;
}
