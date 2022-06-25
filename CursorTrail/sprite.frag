#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform float alpha;

void main()
{    
    color = vec4(1.0, 1.0, 1.0, alpha) * texture(image, TexCoords);
}  