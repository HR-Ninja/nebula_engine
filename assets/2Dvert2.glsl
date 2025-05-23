#version 450 core
layout (location = 0) in vec4 vertex;  // position.xy, texcoord.xy
layout (location = 1) in mat4 instanceModel;

uniform mat4 projection;

out vec2 TexCoords;

void main() {
    gl_Position = projection * instanceModel * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}