#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D tex;

void main()
{
    vec3 shading = vec3(1.0);

    if (Normal.x != 0) shading = shading * 0.9;
    if (Normal.z != 0) shading = shading * 0.7;
    if (Normal.y < 0) shading = shading * 0.5;

    FragColor = texture(tex, TexCoords) * vec4(shading, 1.0);
    /* FragColor = vec4(1.0); */
}

