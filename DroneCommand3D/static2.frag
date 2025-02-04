#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;
uniform float uTime;

void main()
{
    // Shift texture coordinates downward in a looping manner
    vec2 shiftedTexCoords = chTex + vec2(0.0, mod(uTime * 0.013, 1.0));
    
    // Sample the texture with the new coordinates
    vec4 texColor = texture(uTex, shiftedTexCoords);
    
    float brightness = 0.9 - 0.07 * sin(uTime * 0.6);
    
    outCol = texColor * brightness;
}
