#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec3 color;
void main(){
    FragColor=vec4(color,1);
}
