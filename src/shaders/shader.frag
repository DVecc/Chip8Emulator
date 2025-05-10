#version 450

layout(binding = 0) uniform sampler2D chip8Texture;
layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

void main() {
    float pixel = texture(chip8Texture, fragUV).r;
    outColor = vec4(vec3(pixel), 1.0);  // monochrome output
}