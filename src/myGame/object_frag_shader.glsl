#version 150
// A texture is expected as program attribute
uniform sampler2D Texture;

// The vertex shader fill feed this input
in vec2 FragTexCoord;

// The final color
out vec4 FragmentColor;

void main() {
  // Ambient light
  vec3 lightColor = vec3(1,1,1);
  float ambientStrength = 0.25f;
  vec3 ambientColor = ambientStrength * lightColor;

  // Lookup the color in Texture on coordinates given by fragTexCoord
  FragmentColor = vec4(ambientColor, 1.0f) * texture(Texture, FragTexCoord);
}