#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define MAX_DEPTH 1

layout (location = 0) in vec2 fragTexCoord;

layout(binding = 0) uniform sampler2D depthTex;
layout(binding = 1, std140) uniform globals {
  mat4  invProj;
} uniforms;

layout(location = 0) out vec4 color;

vec3 eyePos(sampler2D depthTex, vec2 texCoord) {
  vec4 csPos;
  csPos.xy = texCoord * 2 - 1;
  csPos.z = texture(depthTex, texCoord).r * 2 - 1;
  csPos.w = 1;
  vec4 ePos = uniforms.invProj * csPos;
  return ePos.xyz / ePos.w;
}

void main() {
  float depth = texture(depthTex, fragTexCoord).r;
  if (depth > MAX_DEPTH) {
    discard;
    return;
  }

  vec3 posEye = eyePos(depthTex, fragTexCoord);
  vec2 texelSize = 1 / vec2(textureSize(depthTex, 0));

  vec3 ddx = eyePos(depthTex, fragTexCoord + vec2(texelSize.x, 0)) - posEye;
  vec3 ddx2 = posEye - eyePos(depthTex, fragTexCoord + vec2(-texelSize.x, 0));
  if (abs(ddx.z) > abs(ddx2.z)) {
    ddx = ddx2;
  }
  
  vec3 ddy = eyePos(depthTex, fragTexCoord + vec2(0, texelSize.y)) - posEye;
  vec3 ddy2 = posEye - eyePos(depthTex, fragTexCoord + vec2(0, -texelSize.y));
  if (abs(ddy2.z) < abs(ddy.z)) {
    ddy = ddy2;
  }

  vec3 n = normalize(cross(ddx, ddy));
  color = vec4(n, 1);
}
