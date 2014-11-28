//AARO PERÄMAA, 2014

//
// This is a one of my rare non ray marching algorithms. 
// I developed this originally on my phone, so i wanted to make it
// as fast as possible, but still have plenty of features.
// I designed this to be a voxel renderer, so it would have only
// AABB intersection, but later on i added sphere intersection code too.
// 
// The performace with this renderer is pretty decend, since i got it running 
// on my phone (Samsung Galaxy S Advance (lower end version on S II)) ~15fps
// with 800x480 resolution. Using fisheye edge and disabling other lamp i got ~20fps
// With my PC i got OVER 60fps with 1920x1080 (even with 0.5 selected from the dropbox above (is that 3840x2160 then?)) resolution with HD 7770 and Chrome
//
// YOU ARE FREE TO USE THIS CODE, BUT PLEASE GIVE ME CREDIT IF YOU DO.
//

#define MAX 1337. //Max render distance
#define ERR 0.00001 //Smaller this is, more artifacts
                 //but larger value means more shadow bia#define FISHEYE //Fun!
//#define FISHEYE //Not sure if this is a proper fisheye projection, but it seems to work pretty well
//#define FISHEYEEDGE //Use only with fisheye (i did this back when in early stages to hide a small artifact around the corners lol)
#define VIGNETTE 1.0 //i dunno how to spell it xD
//(.8,.84,.87) (.4,.7,.9)
#define AMBIENT vec3(.8) //Ambient light color, used with shadows (also has strange issue, dont even ask)
#define POINT //Pointlight, not very well visible if used with directional
#define DIRECTIONAL //Sun
#define DIRDIR vec3(.5,-1.,-.5) //directional direction lol (also dont comment)
#define SKY
//#define DITHER //implement
#define GLOSS 16.0 //Glossiness
#define GLOSSMAP //Only applies to ground
#define CAMMOVEMENT //Orbiting camera
#define CAMROT //Rotating camera, designed to be used with CAMMOVEMENT
#define SHADOW //Shadows or not?
#define VFOV 1.25 //Vertical field of view

uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;


const vec3 lb1=vec3(-50,-1,-50);
const vec3 rt1=vec3(50,0,50);
const vec3 lb2=vec3(-1,0,-1);
const vec3 rt2=vec3(1,2,1);
vec3 sc=vec3(0,3.+abs(sin(uTimer*3.1415)),0);

float sphere(vec3 d, vec3 o, vec3 sc, float r)
{
vec3 oc=o-sc;
float b=2.*dot(d,oc);
float c=dot(oc,oc)-r*r;
float disc=b*b-4.*c;
if(disc<0.)
  return MAX;

float q;
if(b<0.)
  q=(-b-sqrt(disc))/2.;
else
  q=(-b+sqrt(disc))/2.;

float t0=q;
float t1=c/q;

if(t0>t1){
float tmp=t0;
t0=t1;
t1=tmp;
}
if(t1<0.0)
  return MAX;

if(t0<0.)
return t1;
else
  return t0;
}

vec3 spheren(vec3 p, vec3 c)
{
return normalize(p-c);
}

float aabb(vec3 r, vec3 o, vec3 lb, vec3 rt)
{
// r.dir is unit direction vector of ray
vec3 dirfrac;
dirfrac.x = 1.0 / r.x;
  dirfrac.y = 1.0 / r.y;
  dirfrac.z = 1.0 / r.z;
   // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner //
   //	r.org is origin of ray
   float t1 = (lb.x - o.x)*dirfrac.x;
   float t2 = (rt.x - o.x)*dirfrac.x;
   float t3 = (lb.y - o.y)*dirfrac.y;
   float t4 = (rt.y - o.y)*dirfrac.y;
   float t5 = (lb.z - o.z)*dirfrac.z;
   float t6 = (rt.z - o.z)*dirfrac.z;
   float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
   float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
   float t=0.;
   // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
   if (tmax < 0.) {
   t = tmax;
   return MAX;
   } // if tmin > tmax, ray doesn't intersect AABB
   if (tmin > tmax) {
   t = tmax;
   return MAX;
   }
   t = tmin;
   return t;
}

vec3 aabbn(vec3 p, vec3 lb, vec3 rt) {
if(abs(p.x-lb.x)<ERR)
return vec3(-1,0,0);
if(abs(p.x-rt.x)<ERR)
return vec3(1,0,0);
if(abs(p.y-lb.y)<ERR)
return vec3(0,-1,0);
if(abs(p.y-rt.y)<ERR)
return vec3(0,1,0);
if(abs(p.z-lb.z)<ERR)
return vec3(0,0,-1);
if(abs(p.z-rt.z)<ERR)
return vec3(0,0,1);
return vec3(0,0,0);
}

vec3 sky(vec3 d){
#ifdef SKY
vec3 r=mix(vec3(.8,.84,.87),vec3(.4,.7,.9),d.y);
r=mix(r,vec3(1,.95,.85),
max(0.,pow(max(dot(d,-DIRDIR),0.)/1.2,16.)));
return r;
#endif
return vec3(0);
}

vec3 shadow(vec3 d, vec3 o){
float t=1000.;
t=min(t,aabb(d,o,lb2,rt2));
t=min(t,aabb(d,o,lb1,rt1));
t=min(t,sphere(d,o,sc,1.));
  if(distance(t,1000.)>ERR)
return vec3(0);
return AMBIENT;
}

vec3 shade(vec3 n, vec3 p,vec3 d,int i)
{
//how long did it have to take until i made
//a proper diffuse shade? waaay too long

//return n/2.+.5;
vec3 r=vec3(0);
vec3 c;
float gi=1.;
if(i==2)
{
if(mod(floor(p.x),2.0)==
     mod(floor(p.z),2.0))
   {
    if(mod(floor(p.x+p.z),2.0)==
      mod(floor(p.z-p.x),2.0))
   {
   c=vec3(.8,.8,.8);
   gi=.8;
  }
  else{
   c=vec3(0.5,0.5,0.5);
   gi=.5;
  }
}
else {
  c=vec3(.65);
  gi=.65;
}
}
if(i==1)
c=vec3(.9);
if(i==3)
c=n/2.+.5;

  #ifndef GLOSSMAP
  gi=1.;
  #endif

vec3 ld=-DIRDIR;
vec3 lo=vec3(-sin(uTimer)*5.,4.,cos(uTimer)*5.);
vec3 ld2=normalize(lo-p);

vec3 dr=r,pr=r;
vec3 ds=vec3(1),ps=ds;

//r=max(0.,dot(ld2,n))*c*r;

#ifdef DIRECTIONAL
ld=normalize(ld);
dr+=vec3(.9,.90,.8)*c;
//r-=shadow(ld,p+n*(ERR*5.)).zyx;
dr*=max(0.,dot(ld,n));

#ifdef SHADOW
ds=shadow(ld,p+n*(ERR*5.));
#endif

#ifdef GLOSS
dr+=vec3(pow(max(dot(reflect(ld,n),d),0.),GLOSS))*gi;
#endif

dr*=ds;
  #endif

  #ifdef POINT
pr+=3./max(distance(lo, p),1.)*c;
//r-=shadow(ld2,p+n*ERR).zyx;
pr*=max(0.,dot(ld2,n));

#ifdef SHADOW
ps=shadow(ld2,p+n*(ERR*5.));
#endif

#ifdef GLOSS
pr+=vec3(pow(max(dot(reflect(ld2,n),d),0.),GLOSS))*gi;
//r=min(vec3(1),r);
#endif

  pr*=ps;
#endif

  //final composit
  dr=min(vec3(1),dr);
  pr=min(vec3(1),pr);

  r=dr+pr;
r=min(vec3(1),r);
return r;
}


vec3 depthSort(vec3 d, vec3 o){
float t=MAX;
float tmp=0.;
vec3 n=vec3(0,0,0);
int i=0;

tmp=aabb(d,o,lb2,rt2);
if(tmp<t) {
t=tmp;
n=aabbn(d*t+o,lb2,rt2);
i=1;
}

tmp=aabb(d,o,lb1,rt1);

if(tmp<t) {
i=2;
t=tmp;
n=aabbn(d*t+o,lb1,rt1);
}

tmp=sphere(d,o,sc,1.);

if(tmp<t){
i=3;
t=tmp;
n=spheren(d*t+o,sc);
}

if(t!=MAX)
return shade(n, d*t+o, d, i);
return sky(d);
}

void main( void )
{
vec2 uv=gl_FragCoord.xy/vec2(uScreenWidth, uScreenHeight)*2.-1.;
float ar=uScreenWidth/uScreenHeight;
uv.x*=ar;
vec3 c=vec3(0,0,0);
vec3 o=vec3(0.,2.5,5);
#ifdef FISHEYEEDGE
if(length(uv)>.75)
  return;
#endif
#ifdef CAMMOVEMENT
o.x=sin(uTimer)*5.;
o.z=cos(uTimer)*5.;
#endif
vec3 d=normalize(vec3(uv,1./-VFOV));
#ifdef FISHEYE
d.z+=length(uv);
d=normalize(d);
#endif
#ifdef CAMROT
float deg=-uTimer;
vec2 oldR=d.xz;
d.x=oldR.x*cos(deg)-oldR.y*sin(deg);
d.z=oldR.x*sin(deg)+oldR.y*cos(deg);
#endif

c=depthSort(d,o);
#ifdef VIGNETTE
c/=max(length(uv)+VIGNETTE-1.*ar,1.);
#endif
#ifdef DITHER
//TODO: Implement dither (hash)
#endif
gl_FragColor=vec4(c,1);
}