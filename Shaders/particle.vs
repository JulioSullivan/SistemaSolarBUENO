#version 330 core
smooth out vec4 vSmoothColor;
uniform mat4 MVP;
uniform float time;

const vec3 a = vec3(0,2,0); //acceleration of particles

const float rate = 1/200.0; //rate of emission
const float life = 2.5; //life of particle

//constants
const float PI = 3.14159;
const float TWO_PI = 2*PI;

//colormap colours
const vec3 RED = vec3(1,0,0);
const vec3 GREEN = vec3(0,1,0);
const vec3 YELLOW = vec3(1,1,0);

//pseudorandom number generator
float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) *
	43758.5453);
}

//pseudorandom direction on a sphere
vec3 uniformRadomDir(vec2 v, out vec2 r) {
	r.x = rand(v.xy);
	r.y = rand(v.yx);
	float theta = mix(0.0, PI / 6.0, r.x);
	float phi = mix(0.0, TWO_PI, r.y);
	return vec3(sin(theta) * cos(phi), cos(theta), sin(theta)
	* sin(phi));
}

void main() {
	vec3 pos=vec3(0);
	float t = gl_VertexID*rate;
	float alpha = 1;
	if(time>t) {
		float dt = mod((time-t), life);
		vec2 xy = vec2(gl_VertexID,t);
		vec2 rdm=vec2(0);
		//Point emission
		//pos = ((uniformRadomDir(xy, rdm) + 0.5*a*dt)*dt);
		//Disc emission
		pos = ( uniformRadomDir(xy, rdm) + 0.5*a*dt)*dt;
		vec2 rect = (rdm*2.0 - 1.0);
		float dotP = dot(rect, rect);
		if(dotP<1)
			pos += vec3(rect.x, 0, rect.y);
		alpha = 1.0 - (dt/life);
	}
	vSmoothColor = vec4(mix(RED,YELLOW,alpha),alpha);
	gl_Position = MVP*vec4(pos,1);
}