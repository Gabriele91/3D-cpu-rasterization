	drawModel(ctx,proj.mul(mT.mul(quad.getMatrix())),Color(255,0,0,255));
    drawModel(ctx,proj.mul(mT.mul(quad.getMatrix())),Color(255,255,0,255));
	
	void drawModel(BuffersContext *ctx,const Matrix4x4& projectViewModel,const Color& color){
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
				drawTriangle(ctx,color,
							 vView1,(vDiv2.z),
							 vView2,(vDiv2.z),
							 vView3,(vDiv2.z));
			}
	}
	
	class Edge{
		
	public:

		Vec2 v1;
		Vec2 v2;
		float oneOverZ1;
		float oneOverZ2;

		Edge(const Vec2& _v1,float _oneOverZ1,const Vec2& _v2,float _oneOverZ2){
			if(_v1.y<_v2.y){
				v1=_v1;
				oneOverZ1=_oneOverZ1;
				v2=_v2;
				oneOverZ2=_oneOverZ2;
			}
			else{
				v1=_v2;
				oneOverZ1=_oneOverZ2;
				v2=_v1;			
				oneOverZ2=_oneOverZ1;	
			}
		}

	};
	class Span{
	
	public:
		int x1;
		int x2;
		float oneOverZ1;
		float oneOverZ2;

		Span(int _x1,float _oneOverZ1,int _x2,float _oneOverZ2){
			if(_x1<_x2){
				x1=_x1;
				oneOverZ1=_oneOverZ1;
				x2=_x2;
				oneOverZ2=_oneOverZ2;
			}
			else{
				x1=_x2;
				oneOverZ1=_oneOverZ2;
				x2=_x1;
				oneOverZ2=_oneOverZ1;				
			}
		}
		void Draw(BuffersContext *ctx, const Color& color, int y){
			if((x2-x1)!=0){
				//factor zbuffer each pixel
				float zFactor = 0.0f;
				float zFactorStep = 1.0f / (float)(x2-x1);
				float zDiffZ=oneOverZ2-oneOverZ1;
				//draw
				for(int x=x1;x<x2;++x){
					//zbuffer calc
					float vz=oneOverZ1+(zDiffZ*zFactor);
					float current=ctx->getZValue(x,y);
					if(vz<current){
						ctx->setZValue(vz,x,y);
						ctx->setPixel(color,x,y);
					}
					zFactor+=zFactorStep;
				}
			}	
		}
	};


	void drawSpanBtwEdge(BuffersContext *ctx, 
						 const Color& color,
						 const Edge& e1, 
						 const Edge& e2){
		//calc diff
		Vec2 e1Diff(e1.v2-e1.v1);
		Vec2 e2Diff(e2.v2-e2.v1);
		float e1DiffZ=e1.oneOverZ2-e1.oneOverZ1;
		float e2DiffZ=e2.oneOverZ2-e2.oneOverZ1;

		if(e1Diff.y==0.0||e2Diff.y==0.0) return;
		//calc factor		
		float factor1 = (e2.v1.y - e1.v1.y) / e1Diff.y;
		float factorStep1 = 1.0f / e1Diff.y;
		float factor2 = 0.0f;
		float factorStep2 = 1.0f / e2Diff.y;
		// loop through the lines between the edges and draw spans
		for(int y = e2.v1.y; y < e2.v2.y; ++y) {
			/* draw span */
			Span span((int)e1.v1.x + (int)(e1Diff.x * factor1),
				      e1.oneOverZ1+(e1DiffZ*factor1),
					  (int)e2.v1.x + (int)(e2Diff.x * factor2),
				      e2.oneOverZ1+(e2DiffZ*factor2));
			//draw span
			span.Draw(ctx,color,y);
			// increase factors
			factor1 += factorStep1;
			factor2 += factorStep2;
		}
	}
	//draw ttriangle
	void drawTriangle(BuffersContext *ctx,
				      const Color& color,
				      const Vec2& v1,float Z1,
					  const Vec2& v2,float Z2,
					  const Vec2& v3,float Z3){
			/* edge triangle */
			Edge edges[3] = {
				Edge(v1,1.0/Z1, v2,1.0/Z2),
				Edge(v2,1.0/Z2, v3,1.0/Z3),
				Edge(v3,1.0/Z3, v1,1.0/Z1)
			};	
			// find edge with the greatest length in the y axis			
			int longEdge = 0;
			int maxLength = edges[0].v2.y - edges[0].v1.y;
			int length2 =   edges[1].v2.y - edges[1].v1.y;
			int length3 =   edges[2].v2.y - edges[2].v1.y;

			if(maxLength<length2){ longEdge=1; maxLength=length2;  }
			if(maxLength<length3){ longEdge=2; maxLength=length3;  }
			
			int shortEdge1 = (longEdge + 1) % 3;
			int shortEdge2 = (longEdge + 2) % 3;
			//draw
			drawSpanBtwEdge(ctx,color,edges[longEdge], edges[shortEdge1]);
			drawSpanBtwEdge(ctx,color,edges[longEdge], edges[shortEdge2]);
			//
	}