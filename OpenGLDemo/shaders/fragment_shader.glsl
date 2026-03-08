#version 330 core

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D spriteSheet;

uniform vec2 uvOffset;
uniform vec2 uvScale;

uniform bool useTexture;
uniform vec4 solidColor;        

const vec3 MAGENTA = vec3(1.0, 0.0, 1.0);

void main() {
    if (useTexture) {
        vec2 uv = uvOffset + TexCoord * uvScale;
        //Temporary color so in case of magenta color can be discarded
        vec4 currentColor = texture(spriteSheet, uv);

        //removes any magenta pixels from showing, Need to be carefull I reuse this in any other project since it will delete all magenta from sprite sheets
        if (currentColor.rgb == MAGENTA) discard;

        outColor = currentColor;
    
    //In case of missing texture or texture error replace with solid color
    }else {
        outColor = solidColor;
    }
}



