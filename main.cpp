#include <Config.h>
#include <VectorialMath.h>
#include <ApplicationIstance.h>
#include <Application.h>
#include <Randerer.h>
#include <iostream>
using namespace TinyRasterization;

class LineApp: ApplicationIstance{

public:
	uint suprlong;
	LineApp():ApplicationIstance(840, //width
								 480, //hight
								 60   //fps
								 ){}
	/*
	struct Vertex{
		Vec3 xyz;
		Vec2 uv;
		Vertex(const Vec3& xyz,const Vec2& uv)
			   :xyz(xyz)
			   ,uv(uv){}
			   
	};
	std::vector<Vertex> points;
	*/
	std::vector<Vec3> points;
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
	Randerer render;

	void start(){
		
		points.push_back(Vec3(-50 ,0   ,0 ));
		points.push_back(Vec3(50  ,-20   ,0 ));
		points.push_back(Vec3(0   ,50  ,0 ));
		points.push_back(Vec3(0   ,0   ,50));

		//tri-line
		lineIndex.push_back(0); lineIndex.push_back(1); 
		lineIndex.push_back(1); lineIndex.push_back(2);
		lineIndex.push_back(2); lineIndex.push_back(0);
		//tri		
		triIndex.push_back(0);
		triIndex.push_back(1);
		triIndex.push_back(2);	
		//
		/*
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
		render.setViewPort(0,0,getWidth(),getHight());
		render.setProjection(proj);
		mov=Vec3(0,0,200);
		mov2=Vec3(0,0,300);

	}

	void update(BuffersContext *ctx,float dt){

		render.setBuffersContext(ctx);
		
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
		render.setModelView(mT.mul(quad.getMatrix()));
		render.drawTriangleWireFrame(points,triIndex,Color(255,0,0,255));
		//draw
		mT.setTranslation(mov);
		render.setModelView(mT.mul(quad.getMatrix()));
		render.drawTriangleWireFrame(points,triIndex,Color(255,0,0,255));
		render.drawTriangleEasy(points,triIndex,Color(255,0,0,255));
		//

	}
	void end(){

	}

};


int main(int,const char**){
	LineApp app;
	Application win((ApplicationIstance*)(&app));
	win.exec();

	return 0;
}
