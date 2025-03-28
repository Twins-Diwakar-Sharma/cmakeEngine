#version 400


struct DirectionalLight
{
	vec3 dir;
	vec3 col;
};

const float threshold = 1.20;
const float noiseSpan = 64;

// random value between -1 and 1
vec2 randomVec2(vec2 st)
{
    st = vec2( dot(st,vec2(0.650,-0.420)),
              dot(st,vec2(-0.290,0.260)) );
    return -1.0 + 2.0*fract(sin(st)*12024.561);
}

float perlinNoise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

   // vec2 u = f*f*(3.0-2.0*f);
    vec2 u = f*f*f*(f*(f*6.0-15.0)+10.0);
    return mix( mix( dot( randomVec2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( randomVec2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( randomVec2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( randomVec2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

float fbm (vec2 p) 
{
  
    // Initial values
    float value = 0.0;
    float amplitude = 0.5;

    // Loop of octaves
    int OCTAVES = 3;
    for (int i = 0; i < OCTAVES; i++) 
    {

        value += amplitude * (perlinNoise(p));
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}


uniform sampler2D heightmap;
float getProceduralHeight(float x, float z)
{
/*   vec2 p = vec2(x,z)/noiseSpan;
   float value = fbm(p);
   float height = value * noiseSpan;
   return height;
 */ 
  
  float res = 512;
  vec2 pos = vec2(x,z);
  pos = pos + vec2(res/2, res/2);
  if(pos.x <= 0 || pos.x >= res || pos.y <= 0 || pos.y >= res)
  {
      return -64;
  }

  pos = pos/res;

  float h = texture(heightmap, pos).r;
  h = 2*h - 1;
  return h*128 + 50;
}

uniform int scale;
uniform vec2 config;
uniform sampler2D normalmap;
vec3 getNormal(float x, float y)
{
  float res = 512;
  vec2 pos = vec2(x,y);
  pos = pos + vec2(res/2, res/2);
  pos = pos/config.y;
  if(pos.x <= 0 || pos.x >= res || pos.y <= 0 || pos.y >= res)
  {
      return vec3(0,1,0);
  }

  pos = pos/res;

  vec3 normal = texture(normalmap, pos).rgb;
  normal = 2.0*normal - vec3(1.0);
  return normal;
}

in vec3 fragWorldPos;
in vec3 testColor;
in vec2 fragRadialPos;
out vec4 outColor;

uniform DirectionalLight sun;
uniform int debugCol;

void main()
{
  vec3 fragNorm = getNormal(fragWorldPos.x, fragWorldPos.z); 
	vec3 toLight = normalize(-1*sun.dir);	
  float ambient = 0.2;
	float diffuse = max(dot(toLight,fragNorm), ambient);
  
  outColor = vec4(0.7,0.9,0.85,1);
  outColor.rgb = diffuse * outColor.rgb;
}
