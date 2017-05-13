#define PI 3.141

uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;


#define MULTISAMPLE 
//#define SIMPLE fast version

#ifdef MULTISAMPLE
const float frames = 8.;
#else
const float frames = 1.;
#endif

const float shutter = 1./60.; //shutter speed
const float ap = 0.3; 		//aperture
const int maxSteps = 44;


float gTime;

float rand(vec2 co){
	return fract(sin(dot(co.xy, vec2(12.9898,78.233)))*43758.5453);
}

vec3 rotateY(vec3 p,float a)
{
	float sa = sin(a);
	float ca = cos(a);
	vec3 r;
	r.x = ca*p.x + sa*p.z;
	r.y = p.y;
	r.z = -sa*p.x + ca*p.z;
	return r;

}

vec3 rotateZ(vec3 p,float a)
{
	float sa = sin(a);
	float ca = cos(a);
	vec3 r;
	r.x =-sa*p.y + ca*p.x; 
	r.y = ca*p.y + sa*p.x;
	
	r.z = p.z;
	return r;

}

float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) +
         length(max(d,0.0));
}




const float landang=4.5;
const float speed=25.;
const float falls=1.+.38;
const float curve=1.*.27;	
const float sep = 6.0;
const float rate=speed/sep;

float domino(vec3 q )
{
 	return sdBox( q, vec3(1.0,8.,4.)*0.5 );
}

vec3 dominoRotate( vec3 p )
{
	float dist=gTime*speed;

	p=rotateY(p,gTime*0.2);
	p.y+=4.*sin(gTime*0.2);
	p.y+=7.;
	p.x+=dist;

	return p;
}

float dominoNum(vec3 p)
{
	p=dominoRotate( p );

	return floor(( (p.x)/sep));
}

vec3 dominoPos(vec3 p, float off)
{
	p = dominoRotate( p );

	float num = floor(( (p.x)/sep));
	
	vec3 q = p;
	//q.y-=4.;
	q.x+=0.;
	q.x= mod(p.x+off*sep,sep)-0.5*sep;
	q.x-=off*sep;
	float rot = (gTime*rate-num-off)*curve;
	rot=(1.-max(1.-rot,0.)*max(1.-rot,0.));

	float a=PI; //standing
	if(num+5.+off < ((gTime)*rate))
		a=landang;
	else if(num+off < ((gTime)*rate))
		a=a=min(PI+falls*rot,landang);
	q=rotateZ(q,a);
	q.x-=0.5;
	q.y+=4.0;
	
	return q;
}

float spc=1.13;
float rad=0.18;

vec3 domCol = vec3(0.1);
vec3 dotCol = vec3(0.9);
vec3 dominoTex(vec3 p, float rnd )
{
#ifndef SIMPLE
	vec3 q=p;

	if (( abs(q.z) > spc*1.5)  || (abs(q.y) > spc*3.+rad) || q.x<0.)
		return domCol;

	if (( abs(q.z) < 1.8) && (abs(q.y) < 0.1) )
		return dotCol;

	if (q.y > 0.)
		q.y-=6.0*0.51;
	else
		q.y+=6.0*0.51;
	q+=0.5*spc;
	

	q = mod(q,spc)-0.5*spc;
	p/=spc;
	if (p.y> 0.)
		p.y -=0.19;
	int spotN = int(p.z+1.45)+3*int(p.y+3.1);

	int n = int(rand(vec2(rnd,-rnd))*7.);

	if (spotN>8)
	{
		n = int(rand(vec2(-rnd,rnd))*7.);
		spotN-=9;
	}
	

	if ((spotN == 1) || (spotN ==7)) 
		return domCol;
	if (((spotN == 0) || (spotN == 8)) && (n < 4))	
		return domCol;
	if ((spotN == 4) && (n/2 == (n+1)/2))
		return domCol;
	if (((spotN ==2) || (spotN ==6)) && n < 2)
		return domCol;
	if (((spotN ==3) || (spotN ==5)) && n != 6)
		return domCol;	

	if (pow(q.y,2.)+pow(q.z,2.)<rad )
		return dotCol;

#endif
	return domCol;

}


float scene(vec3 pos)
{
	return min(domino(dominoPos(pos,0.)),domino(dominoPos(pos,-1.)));

}

// calculate scene normal
vec3 sceneNormal( in vec3 pos )
{
    float eps = 0.0001;
    vec3 n;
    n.x = scene( vec3(pos.x+eps, pos.y, pos.z) ) - scene( vec3(pos.x-eps, pos.y, pos.z) );
    n.y = scene( vec3(pos.x, pos.y+eps, pos.z) ) - scene( vec3(pos.x, pos.y-eps, pos.z) );
    n.z = scene( vec3(pos.x, pos.y, pos.z+eps) ) - scene( vec3(pos.x, pos.y, pos.z-eps) );
    return normalize(n);
}
			
const vec3 lightPos = vec3(-1.0,10.0,7.0);
vec3 l = normalize(lightPos-vec3(0.0));

vec3 trace(vec3 ro, vec3 rd, out vec3 rgb, out bool hit)
{
	hit = false;	

	const float hitThreshold = 0.05;

	vec3 pos = ro+rd;
	
	
	rgb = vec3(1.0);


	for( int i=0;i<maxSteps;i++)
	{
		vec3 currPos =  dominoPos(pos,0.);
		vec3 prevPos =  dominoPos(pos,-1.);
		float curr = domino(currPos);
		float prev = domino(prevPos);
		float d = min( curr, prev );
		if(d<hitThreshold) {
			hit = true;
			float rnd = dominoNum(pos);
				
			
			if (curr < prev)
			{
				rgb = dominoTex(currPos, rnd );
			}
			else
			{
				rgb = dominoTex(prevPos, rnd-1.);
			}
			#ifndef SIMPLE
			vec3 n = sceneNormal(pos);
			rgb+= dot(l,n)*0.05;
			#endif

			return pos;
		}


		float h = 0.;
		h-=4.*sin(gTime*0.2);
		h-=7.;

		if (pos.y < h)
		{
			hit=false;
			return pos;
		}

		pos += d*rd*1.;
		
	}
	rgb = vec3(0.);
	hit=false;
	return pos;	
}


void main( void ) {
	vec2 resolution = vec2(uScreenWidth, uScreenHeight);
	vec2 position = ( (gl_FragCoord.xy/resolution.x)*2. -vec2(1.,resolution.y/resolution.x));

	
	float foc = 28.0;//sin(t)*30.+40.0;
	
	vec3 rgb = vec3(0.0);
	vec3 rf = vec3(position.x*foc,position.y*foc, 0.0);
	vec3 getcol;
	for(float frame = 0.0;frame <frames;frame++)
	{
		float r = rand(vec2(uTimer*position.y,uTimer*position.x)+vec2(frame));
		
		gTime=uTimer+shutter*((r*abs(r))-0.5);

	
		float j=0.;
	
		float a=3.141*2.*rand(vec2(uTimer,uTimer*0.5)*(position+frame));

		vec3 ro=vec3(sin(a)*r*ap,cos(a)*r*ap,foc);
		vec3 rd=normalize(rf-ro);
		bool hit;
		vec3 pos = trace(ro,rd, getcol, hit);
	
		if (hit)
			rgb += getcol/frames;
		else
		{	
			#ifndef SIMPLE
			float ao = 0.5-scene(pos);
			rgb-=clamp( ao, 0.0, 1.0)/frames;
			#endif
			vec3 po = ro+rd*10000.;
			float b = clamp(abs(po.y/10000.),0.0,1.0);
		
		    	rgb+= vec3(1.-b)/frames;
		}
	}
	
	gl_FragColor = vec4( rgb , 1.0 );

}