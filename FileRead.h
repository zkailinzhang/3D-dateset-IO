#pragma once	



class CFileRead
{
public:
	CFileRead(void);
	~CFileRead(void);

	//����ASC�ļ�
	int nLineTotal ;
	double **AscData;

	//����OBJ�ļ�
	int nLineTotal_v;
	double **ObjData_v;

	int nLineTotal_vt;
	double **ObjData_vt;

	int nLineTotal_f;
	int **ObjData_f_vindex;
	int **ObjData_f_vtindex;

	//����������ͼ
	AUX_RGBImageRec *TextureImage[1];
	GLuint	texture[1];

	CString texname,texpic;

	void readLineData(char *FileName);
	void readObjData(char *FileName);
	FILE *fileOpen(const char *Filename, const char *mode);
	void loadtextures(char *Filename);


	void readASCData(char *FileName);


};

