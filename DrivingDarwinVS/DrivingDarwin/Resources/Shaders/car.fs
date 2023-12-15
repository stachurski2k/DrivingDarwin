#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D tex;
uniform vec3 color;
void main(){
    vec4 col=texture(tex,TexCoords);
    col=vec4((col.r+col.g+col.b)*color,col.w);
    if(col.w<0.1){
        discard;
    }
    FragColor=col;
}
