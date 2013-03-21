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

	struct Poly{
		//point struct
		struct {
		    float x,y; float z,w;
		}vt[3];
		//normal
		Vec3 normal;
	};

	struct Edge{
	
		//poly:
		Poly *p;
		//y values
		int top,bottom;
		int hight;
		float yFactor;
		float yStep;
		float dy;
		//x values
		int first;	int last;
		float xStepDiff;
		float xState;
		/**
		*  A polygon edge rapresetation
		*
		*  @param Poly (ponter to polygon)
		*  @param top (index top y vertex) vt[top].y<vt[bottom].y
		*  @param bottom (index bottom vertex) vt[top].y<vt[bottom].y
		*/
		Edge(Poly *p,int top,int bottom)
			:p(p),top(top),bottom(bottom)
		{
			if(p->vt[top].x<p->vt[bottom].x){
				first=top;
				last=bottom;
			}
			else{
				last=top;
				first=bottom;		
			}
			/////////////////////////////
			//        (y index, x index)
			//
			//
			//   p1             (top,last)
			//                        /|  1
			//                       / !  2
			//   p2 (bottom,first)  /  !  3
			//                      1 2 3
			//  ^ ^ ^ ^
			//  | | | |
			//  p1(1,3)
			//  p2(3,1)
			///////////////////////////////
			//
			//
			// p1      (top,first)
			//       1 |\
			//       2 | \ 
			// p2    3 |  \(bottom,last) 
			//         1 2 3
			//  ^ ^ ^ ^
			//  | | | |
			//  p1(1,1)
			//  p2(3,3)
			///////////////////////////////
			///////////////////////////////
			//calc y
			hight=(int)(ceilf(p->vt[bottom].y)-floorf(p->vt[top].y));
			//calc factors
			//y
			yFactor=(p->vt[bottom].y-p->vt[top].y)/(float)(hight);
			yStep=p->vt[top].y;
			//diff
			dy=yStep-p->vt[top].y; //0 ndr
			//calc diff x
			xStepDiff=(p->vt[last].x-p->vt[first].x)/(float)(hight);
			xState=p->vt[first].x;
		}

		DFORCEINLINE void step(){
			//step y
			yStep+=yFactor;
			dy=yStep-p->vt[top].y;
			--hight;
			//step X
			xState+=xStepDiff;
		}

	
	};

	struct Span{
		
		Poly *p;
		Edge *e;
		int width;
		float x1,x2;
		float xFactor;
		float xStep;
		float dx;
		

		Span(Poly *p,Edge *e,float x1,float x2)
			:p(p),e(e),x1(x1),x2(x2){
			//calcs
			width=(int)(ceilf(x2)-floorf(x1));
			xFactor=(x2-x1)/(float)(width);
			xStep=x1;
			dx=xStep-x1; //0 ndr
		
		}

		DFORCEINLINE void setp(){
			xStep+=xFactor;
			dx=xStep-x1;
			--width;
		}

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
		Vec2 windowSize;
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
			windowSize.x=viewPort.width-viewPort.x;
			windowSize.y=viewPort.hight-viewPort.y;
		}
		//set model matrix
		void setModelView(const Matrix4x4& _modelView){
			modelView=std::move(_modelView);
			modelViewProjection=projection.mul(modelView);
		}
		//set project matrix
		void setProjection(const Matrix4x4& _projection){
			projection=std::move(_projection);
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
				view1=div1.xy()*windowSize+window;
				view2=div2.xy()*windowSize+window;
				view3=div3.xy()*windowSize+window;
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
				view1=div1.xy()*windowSize+window;
				view2=div2.xy()*windowSize+window;
				view3=div3.xy()*windowSize+window;
				//draw
				p[0].space=Vec3(clip1.xy(),div1.z);
				p[1].space=Vec3(clip1.xy(),div2.z);
				p[2].space=Vec3(clip2.xy(),div3.z);
				drawTriangle(&p[0]);
			}
		}
		void drawTriangle(const Vertex3D *pV /* , Texture& image */);
		void drawSpanLine(const Gradients&  Gradients,
						  Edge *pLeft,
						  Edge *pRight/* , Texture& image */ );
	};

};

#endif