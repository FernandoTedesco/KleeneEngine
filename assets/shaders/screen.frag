#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;

void main()
{
    vec3 hdrColor = texture(screenTexture,TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor;

    vec3 color = hdrColor;
    float a =2.51;
    float b= 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    vec3 mapped = clamp((color * (a*color+b))/(color*(c*color+d)+e),0.0,1.0);

    float contrast = 1.2;
    mapped = (mapped -0.5)*contrast + 0.5;
    float gray = dot(mapped, vec3(0.299, 0.587, 0.114));
    float saturation = 1.3;
    mapped = mix(vec3(gray), mapped, saturation);
    vec3 tint = vec3(1.0,1.0,1.0);
    mapped *= tint;
    vec2 center = vec2(0.5,0.5);
    float dist = distance(TexCoords,center);
    float vignette = smoothstep(0.8,0.25, dist*(1.0));
    mapped *= vignette;
    mapped = pow(mapped,vec3(1.0/2.2));
    FragColor = vec4(mapped,1.0);

}