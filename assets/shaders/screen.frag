#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform sampler2D depthTexture;
float near = 0.1;
float far = 100.0;
float LinearizeDepth(float depth)
{
    float z = depth*2.0 -1.0;
    return(2.0*near*far)/(far+near-z*(far-near));
}


void main()
{
    vec3 hdrColor = texture(screenTexture,TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor * 0.8;
    float depthValue = texture(depthTexture,TexCoords).r;
    float depthDist = LinearizeDepth(depthValue);
    float fogStart =2.0;
    float fogEnd = 40.0;
    vec3 fogColor = vec3(0.05,0.05,0.08);
    float fogFactor = clamp((depthDist - fogStart)/(fogEnd - fogStart), 0.0,1.0);
    //hdrColor = mix(hdrColor,fogColor,fogFactor);

    float blurIntensity=abs(TexCoords.y - 0.5)*2.0;
    blurIntensity = smoothstep(0.4,0.8,blurIntensity);
    if(false) //blurIntensity>0.0 DOF off for now
    {
        vec3 blurredColor = vec3(0.0);
        float totalWeight = 0.0;
        float spread = 0.004 * blurIntensity;
        for(int x = -1; x <= 1; x++)
        {
            for(int y = -1;y<=1;y++)
            {
                vec2 offset = vec2(float(x),float(y))* spread;
                blurredColor += texture(screenTexture, TexCoords + offset).rgb;
                totalWeight += 1.0;
            }
        }
        blurredColor /= totalWeight;
        hdrColor = mix(hdrColor, blurredColor, blurIntensity);
    }
    vec3 color = hdrColor;
    float a =2.51;
    float b= 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    vec3 mapped = clamp((color * (a*color+b))/(color*(c*color+d)+e),0.0,1.0);
    vec3 brightness = vec3(0.04,0.04,0.05);
    mapped += brightness;
    float contrast = 1.1;
    mapped = (mapped -0.5)*contrast + 0.5;
    float gray = dot(mapped, vec3(0.299, 0.587, 0.114));
    float saturation = 1.35;
    mapped = mix(vec3(gray), mapped, saturation);
    vec3 tint = vec3(1.0,1.0,1.0);
    mapped *= tint;
    vec2 center = vec2(0.5,0.5);
    float vigDist = distance(TexCoords,center);
    float vignette = smoothstep(0.8,0.35, vigDist*(1.0));

    mapped = mix(mapped*vignette,mapped, 0.2);
    mapped = pow(mapped,vec3(1.0/2.2));
    FragColor = vec4(mapped,1.0);

}