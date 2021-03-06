#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 Color;

uniform sampler2D tex;

void main()
{
    /* vec3 shading = vec3(1.0); */

    /* if (Normal.x != 0) shading = shading * 0.9; */
    /* if (Normal.z != 0) shading = shading * 0.7; */
    /* if (Normal.y < 0) shading = shading * 0.5; */

    vec3 ambient = 0.6 * vec3(mix(texture(tex, TexCoords), Color, Color.a));

    vec3 norm = normalize(Normal);
    vec3 light_dir = normalize(vec3(-1., .8, -.8));
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = 1. * diff * vec3(texture(tex, TexCoords));

    FragColor = vec4(ambient + diffuse, 1.0);
    /* FragColor = vec4(1.0); */
}

