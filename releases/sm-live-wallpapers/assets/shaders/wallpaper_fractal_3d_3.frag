uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

vec3   iResolution = vec3(uScreenWidth, uScreenHeight, 1.0);
float  iGlobalTime = 1.2425*uTimer;
vec4   iMouse = vec4(0.0, 0.0, 0.0, 1.0);

// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.



//Triplex inversion
vec3 tinv(vec3 p) 
{
return vec3(p.x,-p.yz)/dot(p,p);
}




vec4 ot; 
float g=.60;
vec3 CSize = vec3(1.28);
vec3 C1 = vec3(1.0);
vec3 C =  vec3(1.0);
const int MaxIter = 10;
float igt = iGlobalTime;
float zoom=1.0;

float map( vec3 p )
{
	float dr = 1.0;
	p=p.yxz;
	ot = vec4(1000.0); 
    C=p*.45;

	for( int i=0; i<MaxIter;i++ )
	{
        
        //BoxFold
       
        p = clamp(p,-CSize,  CSize) * 2.0 - p;
       
        float r2 = dot(p,p);
        if(r2>100.)continue;
		
        ot = min( ot, vec4(abs(p),r2) );

		//Scaling, triplex inversion and translation 
          
            
		dr= dr/r2/g;            
		p = tinv(g*(p))+C;
	}	
	return .16*abs(p.x)/dr;  //Try this
	return .2*length(p)/dr*log(length(p));
	
}

float trace( in vec3 ro, in vec3 rd )
{
	float maxd = 4.;
	float precis = 0.001;
      
    float h=precis*2.0;
    float t = 0.0;
    for( int i=0; i<200; i++ )
    {
	if( t>maxd ||  h<precis*(.1+t)) continue;//break;//        
        
        t += h;
		h = map( ro+rd*t );
    }

   	if( t>maxd ) t=-1.0;
    return t;
}

vec3 calcNormal( in vec3 pos )
{
	vec3  eps = vec3(.0001,0.0,0.0);
	vec3 nor;
	nor.x = map(pos+eps.xyy) - map(pos-eps.xyy);
	nor.y = map(pos+eps.yxy) - map(pos-eps.yxy);
	nor.z = map(pos+eps.yyx) - map(pos-eps.yyx);
	return normalize(nor);
}

void main(void)
{
	vec2 p = -1.0 + 2.0*gl_FragCoord.xy / iResolution.xy;
        p.x *= iResolution.x/iResolution.y;

	
	vec2 m = vec2(-.5)*6.28;
	if( iMouse.z>0.0 )m = (iMouse.xy/iResolution.xy-.5)*6.28;
	m+=.5*vec2(cos(0.15*igt),cos(0.09*igt))+.3;      
	
    // camera


	vec3 ta = vec3(0.,.5+.2*sin(0.12*igt),0.);
	vec3 ro = ta- .9*zoom*vec3( cos(m.x)*cos(m.y), sin(m.y), sin(m.x)*cos(m.y));
	
	
	vec3 cw = normalize(ta-ro);
	vec3 cp = vec3(0.,1.,0.0);
	vec3 cu = normalize(cross(cw,cp));
	vec3 cv = normalize(cross(cu,cw));
	vec3 rd = normalize( p.x*cu + p.y*cv + 2.0*cw );


    // trace	
	vec3 col = vec3(0.8,0.8,1.);
	float t = trace( ro, rd );
	if( t>0.0 )
	{
		
		vec3 pos = ro + t*rd;
		vec3 nor = calcNormal( pos );
		
		// lighting
        vec3  light1 = vec3(  0.577, 0.577, -0.577 );
        vec3  light2 = vec3( -0.707, 0.707,0.0  );
		float key = clamp( dot( light1, nor ), 0.0, 1.0 );
		float bac = clamp( 0.2 + 0.8*dot( light2, nor ), 0.0, 1.0 );
		float amb = (0.7+0.3*nor.y);
		float ao = pow( clamp(ot.w*2.0,0.2,1.0), 1.2 );		
        vec3 brdf = vec3(ao)*(.4*amb+key+.2*bac);

        // material		
		vec3 rgb = vec3(1.0);
		
		rgb =(0.4*abs(sin(2.5+(vec3(.5*ot.w,ot.y*ot.y,2.-5.*ot.w))))+0.6*sin(vec3(-0.2,-0.6,0.8)+2.3+ot.x*22.5))*.85 + .15;
		rgb.gbr=mix(rgb,rgb.bgr+vec3(0.3,0.1,-.2),0.5+.5*sin(15.*ot.w));


		// color
		col = mix(vec3(0.8,0.8,1.),rgb*brdf,exp(-0.28*t));
	}

	
	gl_FragColor=vec4(col,1.0);
}