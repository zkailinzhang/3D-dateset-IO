/* 
OpenGL一个室内三维渲染（OBJ文件导入和贴图）  

小老鼠O2015-02-12上传
视角可移动，旋转的房间简单三维场景，有灯光，有阴影，导入OBJ文件，有纹理贴图，对于初学者还是比较好的，简单易懂
*/
#include<stdlib.h>

#include <stdio.h>

#include<Windows.h>
#include<math.h>

#include<atlimage.h>
#include<GL/glut.h>
#include<GL/glext.h>
#include <GL/glaux.h>
// Include GLM
//#include "glm.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include<vector>
#include <iostream>
using namespace std;


static double seex=100.0;  //0
static double seey=100.0;  //-20.0;
static double seez=100.0;  //50

static double atx=-0.0;   //0
static double aty=-0.0;  //-20
static double atz=-0.0;  // 40
static double angle1=0.0;
static double angle2=0.0;
static double dzx=0.0;
static double dzy=0.0;

const double Pi=3.141592653589793;
static double d=90;

GLuint texture[1];  //为纹理分配空间

static double scalex =1.0;
static double scaley =1.0;
static double scalez =1.0;

vector<glm::vec3> vertices;  
vector<glm::vec2> uvs;  
vector<glm::vec3> normals;  
int nodesSize;
bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
	){
		//printf("Loading OBJ file %s...\n", path);

		std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_uvs;
		std::vector<glm::vec3> temp_normals;


		FILE * file ;
		errno_t err;
		err= fopen_s(&file,path, "r");
		if( file == NULL ){
			printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
			return false;
		}
		printf("Loading OBJ file %s...\n", path);
		while( 1 ){

			char lineHeader[128];

			// read the first word of the line
			int res = fscanf_s(file, "%s", lineHeader,sizeof(129));

			if (res == EOF)

				break; // EOF = End Of File. Quit the loop.

			// else : parse lineHeader
			if ( strcmp( lineHeader, "v" ) != 0){	
				if (strcmp(lineHeader, "vt") == 0){
					glm::vec2 uv;
					fscanf_s(file, "%f %f\n", &uv.x, &uv.y );
					
					
					//uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
					temp_uvs.push_back(uv);			
				}
				else if(strcmp(lineHeader, "f") == 0){
					//cout<<"Get f"<<endl;
					std::string vertex1, vertex2, vertex3;
					unsigned int vertexIndex[3],  vtIndex[3];
					int matches = fscanf_s(file, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &vtIndex[0], &vertexIndex[1], &vtIndex[1], &vertexIndex[2], &vtIndex[2]);  
					if (matches != 6){  
						printf("File can't be read by our simple parser :-( Try exporting with other options\n");  
						return false;  
					}  
					vertexIndices.push_back(vertexIndex[0]);  
					vertexIndices.push_back(vertexIndex[1]);  
					vertexIndices.push_back(vertexIndex[2]);  
					uvIndices.push_back(vtIndex[0]);  
					uvIndices.push_back(vtIndex[1]);  
					uvIndices.push_back(vtIndex[2]);  
				}

				continue;
			}
			if ( strcmp( lineHeader, "v" ) == 0 ){
				//cout<<"Get v"<<endl;
				glm::vec3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
				temp_vertices.push_back(vertex);
			}

		}
		//printf("Loading OBJ file %s...\n", path);
		// For each vertex of each triangle
		for( unsigned int i=0; i<vertexIndices.size(); i++ ){

			// Get the indices of its attributes
			unsigned int vertexIndex = vertexIndices[i];
			unsigned int uvIndex = uvIndices[i];

			// Get the attributes thanks to the index
			glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
			glm::vec2 uv = temp_uvs[ uvIndex-1 ];

			// Put the attributes in buffers
			out_vertices.push_back(vertex);
			out_uvs .push_back(uv);

		}

		return true;
}

//制定纹理的函数
GLuint ATLLoadTexture(const char *fileName)
{
	BITMAP bm;
	GLuint idTexture = 0;
	CImage img;				//需要头文件atlimage.h
	HRESULT hr = img.Load(_T(fileName));
	if ( !SUCCEEDED(hr) )	//文件加载失败
	{
		MessageBox(NULL, _T("文件加载失败"), "ERROR", 0);
		return NULL;
	}


	HBITMAP hbmp = img;
	if (!hbmp)  return -1;	
	if(!GetObject(hbmp, sizeof(bm), &bm))
		return 0;

	glGenTextures(1, &idTexture);  //函数根据纹理参数返回n个纹理索引
	 //glPixelStorei(GL_UNPACK_ALIGNMENT, 4);        // Pixel Storage Mode (Word Alignment / 4 Bytes)
	if (idTexture)				//加载纹理映射
	{
		glBindTexture(GL_TEXTURE_2D, idTexture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glPixelStoref(GL_PACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, bm.bmWidth, bm.bmHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bm.bmBits);	//这里不是GL_RGB
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bm.bmWidth, bm.bmHeight,GL_BGR_EXT, GL_UNSIGNED_BYTE,bm.bmBits);
	}
	return idTexture;

	

}

AUX_RGBImageRec *LoadBMP(char *FileName)
{
	FILE *File = NULL;
	if(!FileName)
		return NULL;
	fopen_s(&File,FileName,"r");
	if (File)
	{
		fclose(File);
		return auxDIBImageLoad(FileName);
	}
	return NULL;
}
int LoadGLTextures()
{
	int Status = FALSE;
	AUX_RGBImageRec *TextureImage[1] = {NULL};

	memset(TextureImage,0,sizeof(void*) * 1);//set the point to NULL
	if (TextureImage[0] = LoadBMP("C:\\1mydata\\tianyuan\\ColorData\\xin000.bmp"))
		Status = TRUE;
	glGenTextures(1,&texture[0]);//命名纹理对象
	glBindTexture(GL_TEXTURE_2D,texture[0]);//绑定纹理
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,TextureImage[0]->sizeX,TextureImage[0]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[0]->data);//指定纹理
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);//指定过滤模式
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);//线性滤波

// 	if (TextureImage[0])
// 	{
// 		if (TextureImage[0]->data)
// 			free(TextureImage[0]->data);
// 		free(TextureImage[0]);
// 	}

	return Status;
}



bool init(GLvoid){

	if(!LoadGLTextures())
		return FALSE;

	//glEnable(GL_TEXTURE_2D); //二维纹理贴图

	glShadeModel(GL_SMOOTH); //启动阴影平滑
	glClearColor(0.0,0.0,0.0,1.0);
	glClearDepth(1.0f);

	glBegin( GL_LINES );
	glColor3d(1.0, 0.0, 0.0);	// X轴 红色
	glVertex3d(-10.0, 0.0, 0.0); 
	glVertex3d(20.0, 0.0, 0.0);
	glColor3d(0.0, 1.0, 0.0);	// Y轴 绿色
	glVertex3d(0.0, -10.0, 0.0);
	glVertex3d(0.0, 20.0, 0.0);
	glColor3d(0.0, 0.0, 1.0);	// Z轴 蓝色
	glVertex3d(0.0, 0.0, -10.0); 
	glVertex3d(0.0, 0.0, 20.0);
	glEnd();

	glPushMatrix();  
	glColor3d(1.0, 0.0, 0.0);
	glTranslatef(20.5f,0.0f,0.0f);  
	glRotatef(90.0f,0.0f,1.0f,0.0f);  //绕y轴正方向旋转90度
	glutSolidCone(5.05,10.2,5,5);  
	glPopMatrix();  


	glPushMatrix();  
	glColor3d(0.0, 1.0, 0.0);
	glTranslatef(0.0f,20.5f,0.0f);  
	glRotatef(-90.0f,1.0f,0.0f,0.0f);  
	glutSolidCone(5.05,10.2,10,10);  
	glPopMatrix();  

	glPushMatrix();  
	glColor3d(0.0, 0.0, 1.0);
	glTranslatef(0.0f,0.0f,20.5f);  
	glRotatef(90.0f,0.0f,0.0f,1.0f);  
	glutSolidCone(5.05,10.2,10,10);  
	glPopMatrix(); 

	glFlush();

	glEnable(GL_DEPTH_TEST); //开启深度测试
	glDepthFunc(GL_LEQUAL);	//深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);



	//texture[0]=ATLLoadTexture("C:\\1mydata\\tianyuan\\ColorData\\xin000.bmp");

// 	glEnable(GL_TEXTURE_2D); //开启2D纹理
// 	glBindTexture(GL_TEXTURE_2D,texture[0]);
// 	glDisable(GL_TEXTURE_2D);
// 
// 	glFlush(); 

	return true;

}



void Obj(){

	GLfloat earth_mat_ambient1[]  = {0.5f, 0.5f, 0.5f, 0.0f};  
	GLfloat earth_mat_diffuse1[]  = {0.5f, 0.5f, 0.5f, 0.0f};  
	glMaterialfv(GL_FRONT, GL_AMBIENT,earth_mat_ambient1);  //指定用于光照计算的当前材质属性
	glMaterialfv(GL_FRONT, GL_DIFFUSE,earth_mat_diffuse1); 

	glBindTexture(GL_TEXTURE_2D,texture[0]);
	glEnable(GL_TEXTURE_2D); 

	glPushMatrix();
	glTranslatef(1.0,-1.8,-2.0);
	glScalef(scalex,scaley,scalez);
	glRotated(-90,0.0,1.0,0.0);
	glColor4f(0.2,0.2,0.2,0.0);
	glBegin(GL_TRIANGLES);  //绘制三角形
	for(int i = 0; i<nodesSize; i++)  
	{  
		//glNormal3f(normals[i].x, normals[i].y, normals[i].z);  
		//glColor3f(1.0,0.0,0.0);

		glTexCoord2f( uvs[i].x,uvs[i].y  );
		glVertex3f( vertices[i].x, vertices[i].y, vertices[i].z);  
	} 
	glEnd();  
	glPopMatrix();
	glDisable(GL_TEXTURE);	

}


void Display(){
	glEnable(GL_DEPTH_TEST); //开启深度测试
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glLoadIdentity();
	gluLookAt(seex,seey,seez,atx,aty,atz,0.0,1.0,0.0 );

	glBindTexture(GL_TEXTURE_2D,texture[0]); 

	Obj();
	//阴影绘制（影子在桌子上）

	glutSwapBuffers();
}



void Reshape(int w,int h)
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0f, 1.5f, 0.0f, 150.0f);//投影矩阵
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//前三个参数表示了观察点的位置，中间三个参数表示了观察目标的位置，最后三个参数代表从(0,0,0)到 (x,y,z)的直线，
	//它表示了观察者认为的“上”方向。
	gluLookAt(seex,seey,seez,atx,aty,atz,0.0,1.0,0.0 );

}


void Mykeyboard(unsigned char key,int x,int y){
	switch(key){
	case('d'):{			 //视点左移
		seex=seex-5.0;
		atx=atx-5.0;
		Display();
		break;
			  }
	case('a'):{			//视点右移
		seex=seex+5.0;
		atx=atx+5.0;
		Display();
		break;
			  }
	case('s'):{			//视点上移
		seey=seey+5.0;
		aty=aty+5.0;
		Display();
		break;
			  }
	case('w'):{			//视点下移
		seey=seey-5.0;
		aty=aty-5.0;
		Display();
		break;
			  }
	case('q'):{			//视点前移
		seez=seez-2.0;
		atz=atz-2.0;
		Display();
		break;
			  }
	case('e'):{			//视点后移
		seez=seez+2.0;
		atz=atz+2.0;
		Display();
		break;
			  }

	}

}


void myMouse(int button,int state,int x,int y)
{
	if(state==GLUT_DOWN)
	{
		glBegin(GL_POINTS);
		glVertex2i(x,480-y);
		glEnd();
		glFlush();
	}
	else if(button==GLUT_RIGHT_BUTTON)
	{
		glClearColor(0.8,0.6,0.7,0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glFlush();
	}
}

void Specialkey(int key,int x,int y){
	dzx=d;
	dzy=d;
	switch(key){
	case(GLUT_KEY_UP):{			//视点上转

		scalex +=0.1;
		scaley +=0.1;
		scalez +=0.1;

		Display();
		break;
					  }
	case(GLUT_KEY_DOWN):{		//视点下转
		scalex -=0.1;
		scaley -=0.1;
		scalez -=0.1;

		Display();
		break;
						}

	}
}

int main(int argc, char* argv[])
{	
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowPosition(100,50);
	glutInitWindowSize(1000,700);
	glutCreateWindow("Xin");
	glutKeyboardFunc(Mykeyboard);
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);

	//glutMotionFunc(myMouse);
	glutSpecialFunc(Specialkey);

	init();

	bool res = loadOBJ("C:\\1mydata\\tianyuan\\ColorData\\xin.obj", vertices, uvs, normals);  
	cout<<vertices.size()<<endl<<uvs.size();  
	nodesSize = vertices.size();
	glutMainLoop();
	return 0;
}
