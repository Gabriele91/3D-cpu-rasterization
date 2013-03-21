#ifndef RANDERER_H
#define RANDERER_H

#include <Config.h>
#include <Color.h>
#include <VectorialMath.h>
#include <BuffersContext.h>

namespace TinyRasterization {

	struct Poly{
		//point struct
		Vec3 vt[3];
		//normal
		Vec3 normal;
		//diff
		float dzdx,dzdy;
		float zStart;
		//compute
		bool compute(){
			Vec3 //diff 1
				 d1(vt[1].x-vt[0].x,
				    vt[1].y-vt[0].y,
					vt[1].z-vt[0].z),
				 //diff 2
				 d2(vt[2].x-vt[0].x,
				    vt[2].y-vt[0].y,
					vt[2].z-vt[0].z);
			//n.b. set z=1/z (ottimization)
			//calc normal
			normal=d1.cross(d2);
			if(normal.z==0) return false;
			//calc diff:
			// - a / c
			dzdy= -normal.x/normal.z;
			// - b / c
			dzdx= -normal.y/normal.z;
			//
			zStart=vt[0].z-vt[0].x*dzdx-vt[0].y*dzdy;
			//
			return true;
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
			Poly poly;
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
				poly.vt[0]=Vec3(view1,1.0f/clip1.z);
				poly.vt[1]=Vec3(view2,1.0f/clip2.z);
				poly.vt[2]=Vec3(view3,1.0f/clip3.z);
				poly.compute();
				drawTriangle(poly,color);
			}
		}

		struct Edge{
		
			uchar top,bottom;
			float lenY;
			Edge(Vec3 *vt,uchar v1,uchar v2){
				if(vt[v1].y>vt[v2].y)
					{ top=v1; bottom=v2; }
				else 
					{ top=v2; bottom=v1; }
				lenY=vt[top].y-vt[bottom].y;
			}

		};

		

		void drawTriangle(const Poly& poly,const Color& color){
			//vertex y sorting:
			int top=0,middle=1,bottom=2;
			if(poly.vt[middle].y>poly.vt[top].y) Math::swap(middle,top);
			if(poly.vt[bottom].y>poly.vt[middle].y){ 
				Math::swap(middle,bottom);
				if(poly.vt[middle].y>poly.vt[top].y) Math::swap(middle,top);
			}
			// draw:
			//from : http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html
			Vec3 start,end;
			float dx1,dx2,dx3;
			float ydiff;
			//calc diff
			if (poly.vt[middle].y-poly.vt[bottom].y > 0) 
				dx1=(poly.vt[middle].x-poly.vt[bottom].x)/(poly.vt[middle].y-poly.vt[bottom].y); 
			else 
				dx1=0;

			if (poly.vt[top].y-poly.vt[bottom].y > 0) 
				dx2=(poly.vt[top].x-poly.vt[bottom].x)/(poly.vt[top].y-poly.vt[bottom].y); 
			else 
				dx2=0;

			if (poly.vt[top].y-poly.vt[middle].y > 0) 
				dx3=(poly.vt[top].x-poly.vt[middle].x)/(poly.vt[top].y-poly.vt[middle].y); 
			else 
				dx3=0;

			//start draw
			start=end=poly.vt[bottom];
			//
			if(dx1 > dx2) {

				//first triangle
				ydiff=ceilf(poly.vt[middle].y-start.y);
				while(ydiff--){
					//span
					for(int x=start.x;x<end.x;++x){
						ctx->setPixel(color,x,start.y);
					}
					++start.y;
					++end.y;
					start.x+=dx2;
					end.x+=dx1;
				}
				//second triangle
				end=poly.vt[middle];
				ydiff=ceilf(poly.vt[top].y-start.y);
				while(ydiff--){
					//span
					for(int x=start.x;x<end.x;++x){
						ctx->setPixel(color,x,start.y);
					}
					//span
					++start.y;
					++end.y;
					start.x+=dx2;
					end.x+=dx3;
				}
			} 
			else {
				//first triangle
				ydiff=ceilf(poly.vt[middle].y-start.y);
				while(ydiff--){
					//span
					for(int x=start.x;x<end.x;++x){
						ctx->setPixel(color,x,start.y);
					}
					//span
					++start.y;
					++end.y;
					start.x+=dx1;
					end.x+=dx2;
				}
				//second triangle
				start=poly.vt[middle];
				ydiff=ceilf(poly.vt[top].y-start.y);
				while(ydiff--){
					//span
					for(int x=start.x;x<end.x;++x){
						ctx->setPixel(color,x,start.y);
					}
					//span
					++start.y;
					++end.y;
					start.x+=dx3;
					end.x+=dx2;
				}
			}
			//end draw
		}
		
	};

};

#endif