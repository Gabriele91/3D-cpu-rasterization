#include "Randerer.h"
//using name space
using namespace TinyRasterization;
//implement costructor Gradients:
Gradients::Gradients(const Vertex3D *pV){

	float oneOverdX=1.0/
    ((pV[1].space.x-pV[2].space.x)*(pV[0].space.y-pV[2].space.y)-
	 (pV[0].space.x-pV[2].space.x)*(pV[1].space.y-pV[2].space.y));

	float oneOverdY=-oneOverdX;

	for(int Counter=0;Counter<3;++Counter){
		const float _oneOverZ=1.0/pV[Counter].space.x;
		oneOverZ[Counter]=_oneOverZ;
		uOverZ[Counter]=pV[Counter].uv.u/_oneOverZ;
		vOverZ[Counter]=pV[Counter].uv.v/_oneOverZ;
	}
	
	#define diffValue_YtoX(value)\
	(((value[1]-value[2])*  (pV[0].space.y-pV[2].space.y))-((value[0]-value[2])* (pV[1].space.y-pV[2].space.y)))
	#define diffValue_XtoY(value)\
	(((value[1]-value[2])*  (pV[0].space.x-pV[2].space.x))-((value[0]-value[2])* (pV[1].space.x-pV[2].space.x)))
	
	//dOnOverZdX=d(1/z)/dX, dOnOverZdY=d(1/z)/dY
	dOneOverZdX=diffValue_YtoX(oneOverZ)*oneOverdX;
	dOneOverZdY=diffValue_XtoY(oneOverZ)*oneOverdY;	
	//dUOverZdX=d(u/z)/dX, dUOverZdY=d(u/z)/dY
	dUOverZdX=diffValue_YtoX(uOverZ)*oneOverdX;
	dUOverZdY=diffValue_XtoY(uOverZ)*oneOverdY;
	//dVOverZdX=d(v/z)/dX, dVOverZdY=d(v/z)/dY
	dVOverZdX=diffValue_YtoX(vOverZ)*oneOverdX;
	dVOverZdY=diffValue_XtoY(vOverZ)*oneOverdY;	
	
	#undef diffValue_YtoX
	#undef diffValue_XtoY
}
//implement edge 
Edge::Edge(const Gradients& gradients,
		   const Vertex3D* pVertices,
		   int top,int bottom){
	//this Y top
	y=ceil(pVertices[top].space.y);
	//this Y bottom
	int endY=ceil(pVertices[bottom].space.y);
	//this height
	height=endY-y;
	//Y error
	float YpreStep=y-pVertices[top].space.y;
	//calc hight nd width in float
	float realWidth=pVertices[bottom].space.x-pVertices[top].space.x;
	float realHight=pVertices[bottom].space.y-pVertices[top].space.y;
	//calc this X (start span)
	x=((realWidth*YpreStep)/realHight)*pVertices[top].space.x;
	//calc span line step
	xStep=realWidth/realHight;
	//X error
	float XpreStep=x-pVertices[top].space.x;
	//calc 1/z [Gradients and Stap]
	oneOverZ=gradients.oneOverZ[top]+
		     XpreStep * gradients.dOneOverZdY +
			 XpreStep * gradients.dOneOverZdX;
	oneOverZStep=xStep* gradients.dOneOverZdX + gradients.dOneOverZdY;
	//calc u/z [Gradients and Stap]
	uOverZ=gradients.uOverZ[top]*
		   XpreStep * gradients.dUOverZdY +
		   XpreStep * gradients.dUOverZdX;
	uOverZStep=xStep* gradients.dUOverZdX + gradients.dUOverZdY;
	//calc v/z [Gradients and Stap]
	vOverZ=gradients.vOverZ[top]+
		   XpreStep * gradients.dVOverZdY +
		   XpreStep * gradients.dVOverZdX;
	vOverZStep=xStep* gradients.dVOverZdX + gradients.dVOverZdY;



}
int Edge::step(){
	//next step
	x+=xStep;               //next X
	++y;                    //next Y
	--height;               //reduce height
	oneOverZ+=oneOverZStep; //next 1/z
	uOverZ+=uOverZStep;     //next u/z
	vOverZ+=vOverZStep;     //next v/z
	return height;			//return height remaining
}

void Randerer::drawTriangle(const Vertex3D *pV /* , Texture& image */){
	
	int top, middle, bottom, middleForCompare, bottomForCompare;
	//sorting
	float Y[]={pV[0].space.y,pV[1].space.y,pV[2].space.y};
	//y sorting
	if(Y[0]<Y[1]){
		if(Y[2]<Y[0]){
			top = 2; 
			middle = 0;
			bottom = 1;
			middleForCompare = 0;
			bottomForCompare = 1;
		}
		else{			
			top = 0;
			if(Y[1] < Y[2]) {
				middle = 1; 
				bottom = 2;
				middleForCompare = 1; 
				bottomForCompare = 2;
			} else {
				middle = 2; 
				bottom = 1;
				middleForCompare = 2; 
				bottomForCompare = 1;
			}
		}
	}
	else{
		if(Y[2] < Y[1]) {
			top = 2; 
			middle = 1; 
			bottom = 0;
			middleForCompare = 1; 
			bottomForCompare = 0;
		} else {
			top = 1;
			if(Y[0] < Y[2]) {
				middle = 0; 
				bottom = 2;
				middleForCompare = 3; 
				bottomForCompare = 2;
			} else {
				middle = 2; 
				bottom = 0;
				middleForCompare = 2; 
				bottomForCompare = 3;
			}
		}
	}
	//create gradients
	Gradients gradients(pV);
	//edge
	Edge topToBottom(gradients,pV,top,bottom);
	Edge topToMiddle(gradients,pV,top,middle);	
	Edge middleToBottom(gradients,pV,middle,bottom);
	//ptr edge
	Edge *pLeft, *pRight;
	//
	int middleIsLeft;
	// the triangle is clockwise, so if bottom > middle then middle is right
	if(bottomForCompare > middleForCompare) {
		middleIsLeft = 0;
		pLeft = &topToBottom; 
		pRight = &topToMiddle;
	} else {
		middleIsLeft = 1;
		pLeft = &topToMiddle; 
		pRight = &topToBottom;
	}
	//
	int _Height = topToMiddle.height;
	//draw
	while(_Height--) {
		//DrawScanLine(Dest,Gradients,pLeft,pRight,Texture);
		drawSpanLine(gradients,pLeft,pRight);
		topToMiddle.step(); 
		topToBottom.step();
	}
	//left or right?
	if(middleIsLeft) {
		pLeft = &middleToBottom;
		pRight = &topToBottom;
	} else {
		pLeft = &topToBottom;
		pRight = &middleToBottom;
	}
	
	while(_Height--) {
		//DrawScanLine(Dest,Gradients,pLeft,pRight,Texture);
		drawSpanLine(gradients,pLeft,pRight);
		middleToBottom.step(); 
		topToBottom.step();
	}
}


void Randerer::drawSpanLine(const Gradients&  Gradients,Edge *pLeft,Edge *pRight/* , Texture& image */ ){
	//start line, ad step
	int xStart = ceil(pLeft->x);
	float xPrestep = xStart - pLeft->x;
	//calc width line
	int _Width = ceil(pRight->x) - xStart;
	//calc step
	float OneOverZ = pLeft->oneOverZ + xPrestep * Gradients.dOneOverZdX;
	float UOverZ = pLeft->uOverZ + xPrestep * Gradients.dUOverZdX;
	float VOverZ = pLeft->vOverZ + xPrestep * Gradients.dVOverZdX;
	while(_Width-- > 0)
	{
		//calc values
		float Z = 1/OneOverZ;
		int U = UOverZ * Z;
		int V = VOverZ * Z;
		//ctx:
		ctx->setPixel(Color(255,U*255,V*255,255),_Width,pLeft->y);
		//linear value update
		OneOverZ += Gradients.dOneOverZdX;
		UOverZ += Gradients.dUOverZdX;
		VOverZ += Gradients.dVOverZdX;
	}
}