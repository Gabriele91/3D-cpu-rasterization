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
				,height(0){}
			int x,
				y,
			    width,
			    height;
		}viewPort;
		Vec2 window;
		Vec2 windowSize;
		/* ptr to BuffersContext */
		BuffersContext *ctx;

	public:
		
		
		Randerer(BuffersContext *ctx=NULL):ctx(ctx){}
		void setBuffersContext(BuffersContext *_ctx=NULL){ ctx=_ctx; }
		void setViewPort(int x,int y, int width, int height){
			viewPort.x=x;
			viewPort.y=y;
			viewPort.width=width;
			viewPort.height=height;
			window.x= (viewPort.x + 1) * viewPort.width*0.5;
			window.y= (viewPort.y + 1) * viewPort.height*0.5;
			windowSize.x=viewPort.width-viewPort.x;
			windowSize.y=viewPort.height-viewPort.y;
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

		//N.B. FAST_IMPLEMETATION isn't my implementation
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
				poly.vt[0]=Vec3(view1,1.0/clip1.z);
				poly.vt[1]=Vec3(view2,1.0/clip2.z);
				poly.vt[2]=Vec3(view3,1.0/clip3.z);
				drawTriangle(poly,color);

#ifdef FAST_IMPLEMETATION
				poly.vt[0]=Vec3(view3,1.0/clip3.z);
				poly.vt[1]=Vec3(view2,1.0/clip2.z);
				poly.vt[2]=Vec3(view1,1.0/clip1.z);
				drawTriangle(poly,color);
#endif

			}
		}


#ifndef FAST_IMPLEMETATION
		void computeInterpolants
			         (const Vec3& v0,
					  const Vec3& v1,
					  const Vec3& v2,
					  float& a,
					  float& b,
					  float& c,
					  float& d){
			a = (v1.z - v2.z) * v0.y + (v2.z - v0.z) * v1.y + (v0.z - v1.z) * v2.y;
			b = v0.z * (v1.x - v2.x) + v1.z * (v2.x - v0.x) + v2.z * (v0.x - v1.x);
			c = v0.x * (v1.y - v2.y) + v1.x * (v2.y - v0.y) + v2.x * (v0.y - v1.y);
			d = - (  (v2.z * v1.y - v1.z * v2.y) * v0.x 
				   + (v0.z * v2.y - v2.z * v0.y) * v1.x 
				   + (v1.z * v0.y - v0.z * v1.y) * v2.x );
		}

		void drawTriangle(const Poly& poly,const Color& color){
			//
			//help from : http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html
			//
			//vertex y sorting:
			int top=0,middle=1,bottom=2;
			if(poly.vt[middle].y>poly.vt[top].y) Math::swap(middle,top);
			if(poly.vt[bottom].y>poly.vt[middle].y){ 
				Math::swap(middle,bottom);
				if(poly.vt[middle].y>poly.vt[top].y) Math::swap(middle,top);
			}
			//
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
			//((a * -XPOS + b * -YPOS - d) / c) 
			//#define planeEQ(XPOS,YPOS) ((a * -XPOS + b * -YPOS - d) / c)
			//((a * -XPOS + b * -YPOS - d) * ic)
			#define planeEQ(XPOS,YPOS) ((a * -XPOS + b * -YPOS - d) * ic)
			float a,b,ic,d,zplane=0.0;
			computeInterpolants (poly.vt[bottom],
								 poly.vt[middle],
								 poly.vt[top],
								 a,b,ic,d);
			//invers of c
			if(ic)
				ic=1.0/ic;
			//start draw
			start=end=poly.vt[bottom];
			//
			if(dx1 > dx2) {

				//first triangle
				ydiff=ceilf(poly.vt[middle].y-start.y);
				while(ydiff--){
					//span
					for(int x=start.x;x<end.x;++x){	
						zplane=planeEQ(x,start.y);
						if(zplane<ctx->getZValue(x,start.y)){
							ctx->setZValue(zplane,x,start.y);		
							ctx->setPixel(color,x,start.y);		
						}			
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
					//zbuffer x
					for(int x=start.x;x<end.x;++x){	
						zplane=planeEQ(x,start.y);
						if(zplane<ctx->getZValue(x,start.y)){
							ctx->setZValue(zplane,x,start.y);		
							ctx->setPixel(color,x,start.y);		
						}			
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
						zplane=planeEQ(x,start.y);
						if(zplane<ctx->getZValue(x,start.y)){
							ctx->setZValue(zplane,x,start.y);		
							ctx->setPixel(color,x,start.y);		
						}			
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
						zplane=planeEQ(x,start.y);
						if(zplane<ctx->getZValue(x,start.y)){
							ctx->setZValue(zplane,x,start.y);		
							ctx->setPixel(color,x,start.y);		
						}			
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

#else 
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
			//" taken "
			//from: https://github.com/Biracun/Acun3D/blob/master/Acun3D/Rasteriser.cpp
			//{ http://www.acunliffe.com/2011/10/3d-software-renderer/ http://luki.webzdarma.cz/eng_05_en.htm }
			
			const int y1 = (int)(16.0f * poly.vt[0].y + 0.5f);
			const int y2 = (int)(16.0f * poly.vt[1].y + 0.5f);
			const int y3 = (int)(16.0f * poly.vt[2].y + 0.5f);


			const int x1 = (int)(16.0f * poly.vt[0].x + 0.5f);
			const int x2 = (int)(16.0f * poly.vt[1].x + 0.5f);
			const int x3 = (int)(16.0f * poly.vt[2].x + 0.5f);


			// Precalculate half-space function coefficients (so they can be calculated incrementally in the loop)
			const int dy1 = x1 - x2;
			const int dy2 = x2 - x3;
			const int dy3 = x3 - x1;


			const int dx1 = y1 - y2;
			const int dx2 = y2 - y3;
			const int dx3 = y3 - y1;


			const int fdx1 = dx1 << 4;
			const int fdx2 = dx2 << 4;
			const int fdx3 = dx3 << 4;


			const int fdy1 = dy1 << 4;
			const int fdy2 = dy2 << 4;
			const int fdy3 = dy3 << 4;


			// Work out min and max X and Y
			#define min3(v1,v2,v3) Math::min(v1,Math::min(v2,v3))
			#define max3(v1,v2,v3) Math::max(v1,Math::max(v2,v3))
			int minX = (min3((int)x1, (int)x2, (int)x3) + 0xF) >> 4;
			int maxX = (max3((int)x1, (int)x2, (int)x3) + 0xF) >> 4;
			int minY = (min3((int)y1, (int)y2, (int)y3) + 0xF) >> 4;
			int maxY = (max3((int)y1, (int)y2, (int)y3) + 0xF) >> 4;
			#undef min3
			#undef max3


			// Make sure it's not outside of the screen
			if (minX < 0) minX = 0;
			if (minY < 0) minY = 0;

			if (maxX >= viewPort.width)
				maxX = viewPort.width - 1;
			if (maxY >= viewPort.height)
				maxY =  viewPort.height - 1;


			// Calculate half-space initial values
			int check1 = (int)((dy1 * (minY << 4)) - (dy1 * y1) - (dx1 * (minX << 4)) + (dx1 * x1));
			int check2 = (int)((dy2 * (minY << 4)) - (dy2 * y2) - (dx2 * (minX << 4)) + (dx2 * x2));
			int check3 = (int)((dy3 * (minY << 4)) - (dy3 * y3) - (dx3 * (minX << 4)) + (dx3 * x3));


			// Extend values if required for fill convention purposes
			if (dx1 < 0 || (dx1 == 0 && dy1 > 0))
				check1++;
			if (dx2 < 0 || (dx2 == 0 && dy2 > 0))
				check2++;
			if (dx3 < 0 || (dx3 == 0 && dy3 > 0)) 
				check3++;


			// The same for z-coordinate for z-buffering
			float dxZ;
			float dyZ;
			float initialZ = computeInterpolants((float)minX, 
												 (float)minY, 
												  poly.vt[0],
												  poly.vt[1],
												  poly.vt[2], 
												  dxZ, 
												  dyZ);


		
			//loop temp values
			int checkTemp[]={0,0,0};
			float tempZ=0.0;
			//for y
			for (int y = minY; y < maxY; y++){
				// Create temporary copies of our incremental values because they'll be needed in the next iteration
				checkTemp[0] = check1;
				checkTemp[1] = check2;
				checkTemp[2] = check3;
				//z value
				tempZ = initialZ;
				//for x
				for (int x = minX; x < maxX; x++){
					if (checkTemp[0] > 0 && checkTemp[1] > 0 && checkTemp[2] > 0){
						// If on top of screen
						if (ctx->getZValue(x,y) > tempZ){						
							ctx->setZValue(tempZ,x,y);
							ctx->setPixel(color,x,y);
						}
					}
					// Increment values in x
					checkTemp[0]  -= fdx1;
					checkTemp[1]  -= fdx2;
					checkTemp[2]  -= fdx3;
					//next z
					tempZ += dxZ;
				}
				// Increment values in y
				check1 += fdy1;
				check2 += fdy2;
				check3 += fdy3;
				//next z
				initialZ += dyZ;
			}


		}
#endif


	};

};

#endif