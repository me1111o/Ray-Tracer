#version 330 core

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 position;
    float spotCutoff;
    vec4 spotDirection;
};

#define MAX_LIGHTS 10

uniform int numLights;
uniform Light lights[MAX_LIGHTS];


uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

uniform sampler2D textureSampler;


in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;


out vec4 FragColor;

void main()
{

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-FragPos);
    vec3 result = vec3(0.0);

    for (int i = 0; i < numLights; i++) {

        vec3 ambient = lights[i].ambient * materialAmbient;

        vec3 lightDir;
        if (lights[i].position.w == 0.0)
            lightDir = normalize(lights[i].position.xyz);
        else
            lightDir = normalize(lights[i].position.xyz - FragPos);

        float diff = max(dot(norm, lightDir), 0.0);

        vec3 diffuse = lights[i].diffuse * (diff * materialDiffuse);


        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
        vec3 specular = lights[i].specular * (spec * materialSpecular);

        float spotlightFactor = 1.0;
        if (lights[i].spotCutoff < 180.0) {
            vec3 spotDir = normalize(lights[i].spotDirection.xyz);
            float theta = dot(lightDir, -spotDir);
            float cutoff = cos(radians(lights[i].spotCutoff));
            if (theta > cutoff)
                spotlightFactor = theta;
            else
                spotlightFactor = 0.0;
        }

        result += ambient * (diffuse + specular) + spotlightFactor * (diffuse + specular);
    }

    vec4 texColor = texture(textureSampler, TexCoord);
    FragColor = vec4(result, 1.0) * texColor;

}
