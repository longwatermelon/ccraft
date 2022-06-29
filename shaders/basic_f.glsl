#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D tex;

void main()
{
    /* vec3 shading = vec3(1.0); */

    /* if (Normal.x != 0) shading = shading * 0.9; */
    /* if (Normal.z != 0) shading = shading * 0.7; */
    /* if (Normal.y < 0) shading = shading * 0.5; */

    vec3 ambient = 0.4 * vec3(texture(tex, TexCoords));

    vec3 norm = normalize(Normal);
    vec3 light_dir = normalize(vec3(0.3, 1.0, 0.4));
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = 0.4 * diff * vec3(texture(tex, TexCoords));

    FragColor = vec4(ambient + diffuse, 1.0);
    /* FragColor = vec4(1.0); */
}

