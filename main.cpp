#include <Config.h>
#include <VectorialMath.h>
#include <ApplicationIstance.h>
#include <Application.h>
#include <iostream>
using namespace TinyRasterization;

class LineApp: ApplicationIstance{

public:
	uint suprlong;
	LineApp():ApplicationIstance(840, //width
								 480, //hight
								 60   //fps
								 ){}
	
	std::vector<Vec4> points;
	std::vector<uint> index;

	Vec3 alpha;
	Vec3 mov;

	//model:
	Matrix4x4 mT;
	Quaternion quad;
	//Matrix4x4 model;
	//Matrix4x4 view;
	Matrix4x4 proj;

	void start(){
		
		points.push_back(Vec4(-50 ,-50 ,-50 ,1));
		points.push_back(Vec4(50  ,-50 ,-50 ,1));
		points.push_back(Vec4(50  ,-50 ,50  ,1));
		points.push_back(Vec4(-50 ,-50 ,50  ,1));

		points.push_back(Vec4(-50 ,50 ,-50 ,1));
		points.push_back(Vec4(50  ,50 ,-50 ,1));
		points.push_back(Vec4(50  ,50 ,50  ,1));
		points.push_back(Vec4(-50 ,50 ,50  ,1));

		//quad 1
		index.push_back(0); index.push_back(1); 
		index.push_back(1); index.push_back(2);
		index.push_back(2); index.push_back(3);
		index.push_back(3); index.push_back(0);
		//quad 2		
		index.push_back(4); index.push_back(5); 
		index.push_back(5); index.push_back(6);
		index.push_back(6); index.push_back(7);
		index.push_back(7); index.push_back(4);

		//"connects quads" 	
		index.push_back(0); index.push_back(4); 
		index.push_back(1); index.push_back(5);
		index.push_back(2); index.push_back(6);
		index.push_back(3); index.push_back(7);

		//set matrix
		proj.setPerspective(45.0f, 840.0f/480.0f ,0.1f, 10.0f);
		mov=Vec3(0,0,200);

	}

	void update(BuffersContext *ctx,float dt){

		ctx->clear();
		
		if (GetAsyncKeyState('W') & 0x8000) mov.y-=10;
		if (GetAsyncKeyState('S') & 0x8000) mov.y+=10;
		
		if (GetAsyncKeyState('A') & 0x8000) mov.x+=10;
		if (GetAsyncKeyState('D') & 0x8000) mov.x-=10;

		if (GetAsyncKeyState('E') & 0x8000) mov.z+=1;
		if (GetAsyncKeyState('C') & 0x8000) mov.z-=1;

		mT.setTranslation(mov);
		
		if (GetAsyncKeyState('T') & 0x8000) alpha.x-=1;
		if (GetAsyncKeyState('B') & 0x8000) alpha.x+=1;
		
		if (GetAsyncKeyState('F') & 0x8000) alpha.y-=1;
		if (GetAsyncKeyState('H') & 0x8000) alpha.y+=1;

		if (GetAsyncKeyState('R') & 0x8000) alpha.z-=1;
		if (GetAsyncKeyState('Y') & 0x8000) alpha.z+=1;


		quad.setFromEulero(Math::torad(alpha.x),
						   Math::torad(alpha.y),
						   Math::torad(alpha.z));

		//calc matrix
		const Matrix4x4& viewModel=mT.mul(quad.getMatrix());
		const Matrix4x4& projectViewModel=proj.mul(viewModel);
		
		Vec4 vClip1,vClip2;
		Vec3 v1,v2;

		//for each vertex
		for(int i=0;i<index.size();i+=2){
			//clip space
			vClip1=projectViewModel.mul(points[index[i]]);
			vClip2=projectViewModel.mul(points[index[i+1]]);
			//div space
			Vec3 divspace1 = vClip1.xyz()/vClip1.w;
			Vec3 divspace2 = vClip2.xyz()/vClip2.w;
			//window space
			Vec2 viewWindow1=divspace1.xy()*Vec2(getWidth(),getHight())+Vec2(getWidth()/2.0,getHight()/2.0);
			Vec2 viewWindow2=divspace2.xy()*Vec2(getWidth(),getHight())+Vec2(getWidth()/2.0,getHight()/2.0);
			//draw
			drawLine(ctx,Color(255,255,255,255),viewWindow1,viewWindow2);
		}

	}
	void end(){

	}

	//function
	void drawLine(BuffersContext *ctx,
				  const Color&& color,
				  Vec2 v1,Vec2 v2){
	 
	  int x0=v1.x, y0=v1.y, 
		  x1=v2.x, y1=v2.y;
	  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
	  int err = (dx>dy ? dx : -dy)/2, e2=0;
 
	  for(;;){
		ctx->setPixel(color,x0,y0);
		if (x0==x1 && y0==y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	  }

	}

};


int main(int,const char**){
	LineApp app;
	Application win((ApplicationIstance*)(&app));
	win.exec();

	return 0;
}
