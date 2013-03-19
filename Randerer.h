#ifndef RANDERER_H
#define RANDERER_H

#include <Config.h>
#include <Color.h>
#include <VectorialMath.h>
#include <BuffersContext.h>

namespace TinyRasterization {

	struct Vertex3D{		
		Vec3  space;
		Color color;
		struct{ 
			float u,v;
		}uv;
	};

	struct Gradients{
		Gradients(const Vertex3D* pVertices); //pass 3 Vertex3D points
		float oneOverZ[3];				//1/z for each vertex
		float uOverZ[3];				//u/z for each vertex
		float vOverZ[3];				//v/z for each vertex
		float dOneOverZdX,dOneOverZdY;	//d(1/z)/dX, d(1/z)/dY
		float dUOverZdX,dUOverZdY;		//d(u/z)/dX, d(u/z)/dY
		float dVOverZdX,dVOverZdY;		//d(v/z)/dX, d(v/z)/dY
	};
	struct Edge{
	
		float x,xStep;
		float y,height;
		float oneOverZ,oneOverZStep;
		float uOverZ,uOverZStep;
		float vOverZ,vOverZStep;

		Edge(const Gradients& gradients,const Vertex3D* pVertices,int top,int bottom );
		int step();
	
	};

	class Randerer {
		/* Matrixs */
		Matrix4x4 modelView;
		Matrix4x4 projection;
		Matrix4x4 modelViewProjection;
		/* ViewPort */
		struct ViewPort{
			ViewPort()
				:x(0)
				,y(0)
				,width(0)
				,hight(0){}
			int x,
				y,
			    width,
			    hight;
		}viewPort;
		Vec2 window;
		/* ptr to BuffersContext */
		BuffersContext *ctx;

	public:
		
		
		Randerer(BuffersContext *ctx=NULL):ctx(ctx){}
		void setBuffersContext(BuffersContext *_ctx=NULL){ ctx=_ctx; }
		void setViewPort(int x,int y, int width, int hight){
			viewPort.x=x;
			viewPort.y=y;
			viewPort.width=width;
			viewPort.hight=hight;
			window.x= (viewPort.x + 1) * viewPort.width*0.5;
			window.y= (viewPort.y + 1) * viewPort.hight*0.5;
		}
		//set model matrix
		void setModelView(const Matrix4x4& mv){
			modelView=mv;
			modelViewProjection=projection.mul(modelView);
		}		
		//set project matrix
		void setProjection(const Matrix4x4& pj){
			projection=pj;
			modelViewProjection=projection.mul(modelView);
		}
		//
		void drawTriangleWireFrame(const std::vector<Vec3>& tris,
								   const std::vector<uint>& index,
								   const Color& color){
			Vec4 clip1,clip2,clip3;
			Vec3 div1,div2,div3;
			Vec2 view1,view2,view3;
			for(int i=0;i<index.size();i+=3){
				//clip space
				clip1=modelViewProjection.mul(Vec4(tris[index[i]],1));
				clip2=modelViewProjection.mul(Vec4(tris[index[i+1]],1));
				clip3=modelViewProjection.mul(Vec4(tris[index[i+2]],1));
				//div space
				div1 = clip1.xyz()/clip1.w;
				div2 = clip2.xyz()/clip2.w;
				div3 = clip3.xyz()/clip3.w;
				//window space
				view1=div1.xy()*window;
				view2=div2.xy()*window;
				view3=div3.xy()*window;
				//draw
				drawLine(color,view1,view2);
				drawLine(color,view2,view3);
				drawLine(color,view3,view1);
			}
		}
		//function
		void drawLine(const Color& color,Vec2 v1,Vec2 v2){
	 
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
		//functions
		void drawTriangleEasy(const std::vector<Vec3>& tris,
							  const std::vector<uint>& index,
							  const Color& color){
			Vec4 clip1,clip2,clip3;
			Vec3 div1,div2,div3;
			Vec2 view1,view2,view3;
			Vertex3D p[3];
			for(int i=0;i<index.size();i+=3){
				//clip space
				clip1=modelViewProjection.mul(Vec4(tris[index[i]],1));
				clip2=modelViewProjection.mul(Vec4(tris[index[i+1]],1));
				clip3=modelViewProjection.mul(Vec4(tris[index[i+2]],1));
				//div space
				div1 = clip1.xyz()/clip1.w;
				div2 = clip2.xyz()/clip2.w;
				div3 = clip3.xyz()/clip3.w;
				//window space
				view1=div1.xy()*window;
				view2=div2.xy()*window;
				view3=div3.xy()*window;
				//draw
				p[1].space=div1;
				p[2].space=div1;
				p[3].space=div1;
				drawTriangle(p);
			}
		}
		void drawTriangle(const Vertex3D *pV /* , Texture& image */);
		void drawSpanLine(const Gradients&  Gradients,
						  Edge *pLeft,
						  Edge *pRight/* , Texture& image */ );
	};

};

#endif