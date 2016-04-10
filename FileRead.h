#pragma once	



class CFileRead
{
public:
	CFileRead(void);
	~CFileRead(void);

	//定义ASC文件
	int nLineTotal ;
	double **AscData;

	//定义OBJ文件
	int nLineTotal_v;
	double **ObjData_v;

	int nLineTotal_vt;
	double **ObjData_vt;

	int nLineTotal_f;
	int **ObjData_f_vindex;
	int **ObjData_f_vtindex;

	//定义纹理贴图
	AUX_RGBImageRec *TextureImage[1];
	GLuint	texture[1];

	CString texname,texpic;

	void readLineData(char *FileName);
	void readObjData(char *FileName);
	FILE *fileOpen(const char *Filename, const char *mode);
	void loadtextures(char *Filename);


	void readASCData(char *FileName);


};

