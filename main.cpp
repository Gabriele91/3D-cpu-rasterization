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
	struct Vertex{
		Vec3 xyz;
		Vec2 uv;
		Vertex(const Vec3& xyz,const Vec2& uv)
			   :xyz(xyz)
			   ,uv(uv){}
			   
	};
	std::vector<Vertex> points;
	std::vector<uint> lineIndex;
	std::vector<uint> triIndex;

	Vec3 alpha;
	Vec3 mov;
	Vec3 mov2;

	//model:
	Matrix4x4 mT;
	Quaternion quad;
	//Matrix4x4 model;
	//Matrix4x4 view;
	Matrix4x4 proj;

	void start(){
		
		points.push_back(Vertex(Vec3(-50 ,0   ,0 ) ,Vec2(0,0)));
		points.push_back(Vertex(Vec3(50  ,0   ,0 ) ,Vec2(1,0)));
		points.push_back(Vertex(Vec3(0   ,50  ,0 ) ,Vec2(0.5,1)));
		points.push_back(Vertex(Vec3(0   ,0   ,50) ,Vec2(0.5,1)));

		//tri-line
		lineIndex.push_back(0); lineIndex.push_back(1); 
		lineIndex.push_back(1); lineIndex.push_back(2);
		lineIndex.push_back(2); lineIndex.push_back(0);
		//tri		
		triIndex.push_back(0);
		triIndex.push_back(1);
		triIndex.push_back(2);	
		//
		triIndex.push_back(0);
		triIndex.push_back(2);
		triIndex.push_back(3);
		//
		triIndex.push_back(1);
		triIndex.push_back(2);
		triIndex.push_back(3);
		//
		triIndex.push_back(0);
		triIndex.push_back(1);
		triIndex.push_back(3);
		Vec2 a(1,2);
		Vec2 b(3,4);
		Math::swap(a,b);
		//triangles
		/*
		tri.v1.xyz=Vec3(-50 ,50  ,0);
		tri.v2.xyz=Vec3(50  ,50  ,0);
		tri.v3.xyz=Vec3(0   ,50  ,0);
		Vec3 vCross1=tri.v2.xyz-tri.v1.xyz;
		Vec3 vCross2=tri.v3.xyz-tri.v1.xyz;
		Vec3 dir=vCross1.cross(vCross2);
		Vec3 normal=dir.getNormalize();
		*/


		//set matrix
		proj.setPerspective(45.0f, 840.0f/480.0f ,0.1f, 10.0f);
		mov=Vec3(0,0,200);
		mov2=Vec3(0,0,300);

	}

	void update(BuffersContext *ctx,float dt){


		
		if (GetAsyncKeyState('W') & 0x8000) mov.y-=1;
		if (GetAsyncKeyState('S') & 0x8000) mov.y+=1;
		
		if (GetAsyncKeyState('A') & 0x8000) mov.x+=1;
		if (GetAsyncKeyState('D') & 0x8000) mov.x-=1;

		if (GetAsyncKeyState('E') & 0x8000) mov.z+=1;
		if (GetAsyncKeyState('C') & 0x8000) mov.z-=1;

		mT.setTranslation(mov);
		
		if (GetAsyncKeyState(VK_PRIOR) & 0x8000) mov2.y-=1;
		if (GetAsyncKeyState(VK_NEXT) & 0x8000) mov2.y+=1;
		
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) mov2.x+=1;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) mov2.x-=1;

		if (GetAsyncKeyState(VK_UP) & 0x8000) mov2.z+=1;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) mov2.z-=1;
		////////////////////////////////////////////////////

		if (GetAsyncKeyState('T') & 0x8000) alpha.x-=1;
		if (GetAsyncKeyState('B') & 0x8000) alpha.x+=1;
		
		if (GetAsyncKeyState('F') & 0x8000) alpha.y-=1;
		if (GetAsyncKeyState('H') & 0x8000) alpha.y+=1;

		if (GetAsyncKeyState('R') & 0x8000) alpha.z-=1;
		if (GetAsyncKeyState('Y') & 0x8000) alpha.z+=1;


		quad.setFromEulero(Math::torad(alpha.x),
						   Math::torad(alpha.y),
						   Math::torad(alpha.z));
		
		Vec4 vClip1,vClip2,vClip3;	
		
		
		ctx->clearZbuffer(2);
		ctx->clear();
		//offset triangle
		mT.setTranslation(mov2);
		drawModelVireWare(ctx,proj.mul(mT.mul(quad.getMatrix())),Color(255,0,0,255));
		//draw
		mT.setTranslation(mov);
		drawModelVireWare(ctx,proj.mul(mT.mul(quad.getMatrix())),Color(255,0,0,255));
		//

	}
	void end(){

	}
	void drawModelVireWare(BuffersContext *ctx,const Matrix4x4& projectViewModel,const Color& color){
			Vec4 vClip1,vClip2,vClip3;	
			//for each vertex		
			for(int i=0;i<triIndex.size();i+=3){
				//clip space
				vClip1=projectViewModel.mul(Vec4(points[triIndex[i]].xyz,1));
				vClip2=projectViewModel.mul(Vec4(points[triIndex[i+1]].xyz,1));
				vClip3=projectViewModel.mul(Vec4(points[triIndex[i+2]].xyz,1));
				//div space
				Vec3 vDiv1 = vClip1.xyz()/vClip1.w;
				Vec3 vDiv2 = vClip2.xyz()/vClip2.w;
				Vec3 vDiv3 = vClip3.xyz()/vClip3.w;
				//window space
				Vec2 vView1=vDiv1.xy()*Vec2(getWidth(),getHight())+Vec2(getWidth()/2.0,getHight()/2.0);
				Vec2 vView2=vDiv2.xy()*Vec2(getWidth(),getHight())+Vec2(getWidth()/2.0,getHight()/2.0);
				Vec2 vView3=vDiv3.xy()*Vec2(getWidth(),getHight())+Vec2(getWidth()/2.0,getHight()/2.0);
				//draw
				drawLine(ctx,Color(0,0,255,255),vView1,vView2);
				drawLine(ctx,Color(0,0,255,255),vView2,vView3);
				drawLine(ctx,Color(0,0,255,255),vView3,vView1);
			}
	}
	

	//function
	void drawLine(BuffersContext *ctx,
				  const Color& color,
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
