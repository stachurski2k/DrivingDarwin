#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D tex;
void main(){
    vec4 col=texture(tex,TexCoords);
    if(col.w<0.1){
        discard;
    }
    FragColor=col;
}
