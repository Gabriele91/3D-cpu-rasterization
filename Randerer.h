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
				poly.vt[0]=Vec3(view1,clip1.w);
				poly.vt[1]=Vec3(view2,clip2.w);
				poly.vt[2]=Vec3(view3,clip3.w);
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

		//diff
		//computeInterpolants
		float computeInterpolants
			         (float minX,
				      float minY,
					  const Vec3& v0,
					  const Vec3& v1,
					  const Vec3& v2,
					  float& dzdx,
					  float& dzdy){
			float a = (v1.z - v2.z) * v0.y + (v2.z - v0.z) * v1.y + (v0.z - v1.z) * v2.y;
			float b = v0.z * (v1.x - v2.x) + v1.z * (v2.x - v0.x) + v2.z * (v0.x - v1.x);
			float c = v0.x * (v1.y - v2.y) + v1.x * (v2.y - v0.y) + v2.x * (v0.y - v1.y);
			if(c==0) return 0;
			//calc diff:
			// - a / c
			dzdx= -a/c;
			// - b / c
			dzdy= -b/c;
			// Calculate initial value, dx and dy	(A * -minX + B * -minY -D) / C		
			float d = - (  (v2.z * v1.y - v1.z * v2.y) * v0.x 
				         + (v0.z * v2.y - v2.z * v0.y) * v1.x 
					     + (v1.z * v0.y - v0.z * v1.y) * v2.x );

			return (a * -minX + b * -minY - d) / c;
			//
		}

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
			//and from: https://github.com/Biracun/Acun3D/blob/master/Acun3D/Rasteriser.cpp
			//and from: http://www.acunliffe.com/2011/10/3d-software-renderer/
			//and from: http://luki.webzdarma.cz/eng_05_en.htm
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

			//compute zbuffer:
			float dzdx=0.0,dzdy=0.0;
			float zstart=
			computeInterpolants
			       (Math::min(poly.vt[bottom].x,Math::min(poly.vt[middle].x,poly.vt[top].x)), //xstart
					Math::min(poly.vt[bottom].y,Math::min(poly.vt[middle].y,poly.vt[top].y)), //ystart
					Vec3(poly.vt[bottom].xy(),1.0/poly.vt[bottom].z),
					Vec3(poly.vt[middle].xy(),1.0/poly.vt[middle].z),
					Vec3(poly.vt[top].xy(),1.0/poly.vt[top].z),
					dzdx,
					dzdy);
			//start draw
			start=end=poly.vt[bottom];
			//
			if(dx1 > dx2) {

				//first triangle
				ydiff=ceilf(poly.vt[middle].y-start.y);
				while(ydiff--){
					//span
					//zbuffer x
					float zx=zstart;
					for(int x=start.x;x<end.x;++x){	
						if(zx<ctx->getZValue(x,start.y)){
							ctx->setZValue(zx,x,start.y);		
							ctx->setPixel(color,x,start.y);		
						}				
						//next z
						//zx=zstart+dzdx*x;
						zx+=dzdx;
					}
					++start.y;
					++end.y;
					start.x+=dx2;
					end.x+=dx1;
					//next z
					zstart+=dzdy;
					//ctx->zbufferToColorBuffer();
					//ctx->swap();
				}
				//second triangle
				end=poly.vt[middle];
				ydiff=ceilf(poly.vt[top].y-start.y);
				while(ydiff--){
					//span
					//zbuffer x
					float zx=zstart;
					for(int x=start.x;x<end.x;++x){	
						if(zx<ctx->getZValue(x,start.y)){
							ctx->setZValue(zx,x,start.y);		
							ctx->setPixel(color,x,start.y);		
						}						
						//next z
						zx+=dzdx;
					}
					//span
					++start.y;
					++end.y;
					start.x+=dx2;
					end.x+=dx3;
					//next z
					zstart+=dzdy;
					//ctx->zbufferToColorBuffer();
					//ctx->swap();
				}
			} 
			else {
				//first triangle
				ydiff=ceilf(poly.vt[middle].y-start.y);
				while(ydiff--){
					//span
					//zbuffer x
					float zx=zstart;
					for(int x=start.x;x<end.x;++x){			
						if(zx<ctx->getZValue(x,start.y)){
							ctx->setZValue(zx,x,start.y);		
							ctx->setPixel(color,x,start.y);		
						}						
						//next z
						zx+=dzdx;
					}
					//span
					++start.y;
					++end.y;
					start.x+=dx1;
					end.x+=dx2;
					//next z
					zstart+=dzdy;
				}
				//second triangle
				start=poly.vt[middle];
				ydiff=ceilf(poly.vt[top].y-start.y);
				while(ydiff--){
					//span
					//zbuffer x
					float zx=zstart;
					for(int x=start.x;x<end.x;++x){			
						if(zx<ctx->getZValue(x,start.y)){
							ctx->setZValue(zx,x,start.y);		
							ctx->setPixel(color,x,start.y);		
						}						
						//next z
						zx+=dzdx;
					}
					//span
					++start.y;
					++end.y;
					start.x+=dx3;
					end.x+=dx2;
					//next z
					zstart+=dzdy;
				}
			}
			//end draw
		}
		
	};

};

#endif