#version 150

layout (std140)
uniform
view {
    mat4 camera, projection;
};

in vec3 frag_position, frag_normal;
in vec4 vertex_color;

out vec4 fragColor;

const vec3 light_direction = vec3(-0.408248, -0.816497, 0.408248);

void main() {
    float shade = 1.0;

    if (length(frag_normal) > 0.05) {
        vec3 normal = normalize(frag_normal);

        shade = 0.5 * (-dot(normal, normalize(light_direction)) + 1.0);
    }
    fragColor =  vec4(vertex_color.rgb * shade, vertex_color.a);
}