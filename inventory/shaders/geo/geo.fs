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
uniform int scale;
uniform vec2 config;
float getProceduralHeight(float x, float z)
{

  float res = 1024;
  vec2 pos = vec2(x,z);
  pos = pos;
  //pos = pos/config.y;
  if(pos.x <= 0 || pos.x >= res || pos.y <= 0 || pos.y >= res)
  {
      return -64;
  }

  pos = pos/res;

  float h = texture(heightmap, pos).r;
  h = 2*h - 1.0;
  h = h*128 + 50;
  

  return h;
}


vec3 getNormal(float x, float y)
{
    float h = 1.0/256.0; // 0.2
    vec3 me = vec3(x,0,y);
  
    vec3 meX = vec3(x+h,0,y);
    vec3 meZ = vec3(x,0,y+h);

    me.y = getProceduralHeight(me.x,me.z);
    meX.y = getProceduralHeight(meX.x,meX.z);
    meZ.y = getProceduralHeight(meZ.x,meZ.z);

    vec3 ans = normalize(cross(meZ-me,meX-me));

    return ans;

}


uniform sampler2D normalmap;

/*
uniform int scale;
uniform vec2 config;
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
*/

in vec3 fragWorldPos;
in vec2 fragRadialPos;
out vec4 outColor;

uniform DirectionalLight sun;
uniform int debugCol;

#define n_cascades 4
uniform sampler2D cascShadowmap[n_cascades];
uniform float cascFarPlane[n_cascades];
in vec4 fragLightProjPos[n_cascades];
in float fragDepth;


void main()
{
  vec3 fragNorm = getNormal(fragWorldPos.x, fragWorldPos.z); 
	vec3 toLight = normalize(-1*sun.dir);	
  int cascIdx = n_cascades-1;
/*
  for(int i=0; i<n_cascades; i++)
  {
    if(abs(fragDepth) <= cascFarPlane[i])
    {
      cascIdx = i;
      break;
    }
  }
  //cascIdx = 2;
  vec3 projCoords = fragLightProjPos[cascIdx].xyz / fragLightProjPos[cascIdx].w;
  projCoords = projCoords * 0.5 + 0.5;
  float bias = max(0.05 * (1.0 - dot(fragNorm, toLight)), 0.005);
  float shadow = 0.0;
  float pcfDepth = texture(cascShadowmap[cascIdx], projCoords.xy).r;
  shadow += (projCoords.z - bias) > pcfDepth ? 1.0 : 0.0;
  if(projCoords.z > 1.0)
    shadow = 0.0;
*/

  float shadow = 0;


  float stepScale = 5;
  float step = dot(-sun.dir, vec3(0,1,0)) * stepScale + 2;
  
  int num_steps = 5;
  shadow = 0;
  vec3 position = fragWorldPos;
  float bias = max(0.05 * (1.0 - dot(fragNorm, toLight)), 0.005);
  for(int i=0; i<num_steps; i++)
  {
    vec3 toSun = -sun.dir;
    vec3 checkPos = position + step*toSun;
    checkPos.y = getProceduralHeight(checkPos.x, checkPos.z);    
    if(checkPos.y >= position.y+bias)
    {
      shadow = 1;
      break;
    }
  }


  float ambient = 0.2;
	float diffuse = max(dot(toLight,fragNorm), ambient);
  float dotSky = max(dot(vec3(0,1,0), fragNorm), ambient) * 0.2;
  diffuse = (1.0 - shadow) * diffuse + shadow * dotSky;

  outColor = vec4(0.7,0.9,0.85,1);
  outColor.rgb = diffuse * outColor.rgb;


}
