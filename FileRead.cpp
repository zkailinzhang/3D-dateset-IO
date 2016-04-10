#include "stdafx.h"
#include "FileRead.h"


CFileRead::CFileRead(void)
{	
}


CFileRead::~CFileRead(void)
{
}
void CFileRead::readLineData(char *FileName)
{
	
}
void CFileRead::readObjData(char *FileName)
{
	FILE *dataFile;
	dataFile = fileOpen(FileName, "r");
	if (dataFile == NULL)
		exit(1);	
	//定义有用数据行
	nLineTotal_v  = 0;
	nLineTotal_vt = 0;
	nLineTotal_f  = 0;


	char inputLine[512] ;	
	int count;
	GLfloat x,y,z;

	//char v_index[100];
	//char vt_index[100];
	char point1_index[100];
	char point2_index[100];
	char point3_index[100];

	

	for (;;)
	{
		//判断v,vt，f各有几行数据
		if (fgets(inputLine, 512, dataFile) == NULL)
			break;
		if (inputLine[0] == 'v')
		{
			if (inputLine[1] == ' ')
			{
				count = sscanf(inputLine, "v %f %f %f",&x, &y, &z);
				if (count != 3)
					continue;
				nLineTotal_v++;
			}
			else if (inputLine[1] == 't')
			{
 				count = sscanf(inputLine, "vt %f %f %f",&x, &y, &z);
 				if(count != 3)
 					continue;
				nLineTotal_vt++;
			}
		}
		else if(inputLine[0] == 'f')
			nLineTotal_f++;
		//判断obj文件是否有纹理
		if (inputLine[0] == 'm')
		{
			CString str;
			str = inputLine;
			char temp[10] = {'\0'};
			if (str.Left(6) == "mtllib")
			{
				int iStrat = str.Find(_T("/"),0);
				texname = str.Right(str.GetLength()-iStrat-1);
			}
		}
	}
// 	FILE *file;
// 	file = fileOpen(texname,"r");
// 	fgets(inputLine, 512, file);
	
	ObjData_v = new double *[nLineTotal_v];
	for (int i=0;i<nLineTotal_v;i++)
	{
		ObjData_v[i] = new double [3]; 
	}

	ObjData_vt = new double *[nLineTotal_vt];
	for (int i=0;i<nLineTotal_vt;i++)
	{
		ObjData_vt[i] = new double [3];
	}

	ObjData_f_vindex = new int *[nLineTotal_f];
	ObjData_f_vtindex = new int *[nLineTotal_f];
	for (int i=0;i<nLineTotal_f;i++)
	{
		ObjData_f_vindex[i]  = new int [3];
		ObjData_f_vtindex[i] = new int [3];
	}
	
	rewind(dataFile);	

	int v=0,vt=0,f=0;
//	int q,w,e,r,t,p;
	for (;;)
	{
		if (fgets(inputLine, 512, dataFile) == NULL)
			break;		
		if (inputLine[0] == 'v')
		{
			if (inputLine[1] == ' ')
			{
				count = sscanf(inputLine, "v %f %f %f",&x, &y, &z);
				if (count != 3)
					continue;
				ObjData_v[v][0] = x;
				ObjData_v[v][1] = y;
				ObjData_v[v][2] = z;
				v++;
			}
			else if (inputLine[1] == 't')
			{
				count = sscanf(inputLine, "vt %f %f %f",&x, &y, &z);
				ObjData_vt[vt][0] = x;
				ObjData_vt[vt][1] = y;
				ObjData_vt[vt][2] = z;
				vt++;
			}
		}
		else if (inputLine[0] == 'f')
		{
			CString str;
			str = inputLine;
			int iStrat = str.Find(_T(" "),3);
			int iMind = str.Find(_T(" "),iStrat+1);
			int iEnd = str.Find(_T("\n"),iMind+1);

			int jStrat = str.Find(_T("/"),2);
			int jMind = str.Find(_T("/"),jStrat+1);
			int jEnd = str.Find(_T("/"),jMind+1);

			CString str_v1,str_v2,str_v3,
					str_vt1,str_vt2,str_vt3;

			str_v1  = str.Mid(2+1,jStrat-(2+1));
			str_vt1 = str.Mid(jStrat+1,iStrat-(jStrat+1));
			str_v2  = str.Mid(iStrat+1,jMind-(iStrat+1));
			str_vt2 = str.Mid(jMind+1,iMind-(jMind+1));
			str_v3  = str.Mid(iMind+1,jEnd-(iMind+1));
			str_vt3 = str.Mid(jEnd+1,iEnd-(jEnd+1));

			ObjData_f_vindex[f][0] = _ttoi(str_v1.GetBuffer(str_v1.GetLength()));								
			ObjData_f_vindex[f][1] = _ttoi(str_v2.GetBuffer(str_v2.GetLength()));
			ObjData_f_vindex[f][2] = _ttoi(str_v3.GetBuffer(str_v3.GetLength()));

			ObjData_f_vtindex[f][0] = _ttoi(str_vt1.GetBuffer(str_vt1.GetLength()));
			ObjData_f_vtindex[f][1] = _ttoi(str_vt2.GetBuffer(str_vt2.GetLength()));
			ObjData_f_vtindex[f][2] = _ttoi(str_vt3.GetBuffer(str_vt3.GetLength()));
			f++;

// 			q = _ttoi(str_v1.GetBuffer(str_v1.GetLength()));
// 			w = _ttoi(str_v2.GetBuffer(str_v2.GetLength()));
// 			e = _ttoi(str_v3.GetBuffer(str_v3.GetLength()));
// 
// 			r = _ttoi(str_vt1.GetBuffer(str_vt1.GetLength()));
// 			t = _ttoi(str_vt2.GetBuffer(str_vt2.GetLength()));
// 			p = _ttoi(str_vt3.GetBuffer(str_vt3.GetLength()));

		}
	}
}


//CFileRead cfileread;
void CFileRead::readASCData(char *FileName)
{
	FILE *dataFile;
	dataFile = fileOpen(FileName, "r");
	if (dataFile == NULL)
		exit(1);	
	nLineTotal = 0;
	//Sleep(1);
	do 
	{
		char temp[256] = {'\0'};
		if (fgets(temp, 256, dataFile) == NULL)
			break;
		nLineTotal++;
	} while (1);
	rewind(dataFile);

	AscData = new double *[nLineTotal];
	for (int i=0;i<nLineTotal; i++)
	{
		AscData[i] = new double [3];
	}
	for (int i=0; i<nLineTotal; i++)
	{
		char temp[256] = {'\0'};
		if (fgets(temp, 256, dataFile) == NULL)
			break;
		CString str;
		str = temp;
		int iStart = str.Find(_T(" "),0);
		int iMid = str.Find(_T(" "),iStart+1);
		CString str1,str2,str3;
		str1 = str.Left(iStart);
		str2 = str.Mid(iStart+1, iMid-iStart-1);
		str3 = str.Right(str.GetLength()-iMid-1);

		AscData[i][0] = atof(str1.GetBuffer(str1.GetLength()));
		AscData[i][1] = atof(str2.GetBuffer(str2.GetLength()));
		AscData[i][2] = atof(str3.GetBuffer(str3.GetLength()));
	}
}
FILE* CFileRead::fileOpen(const char *filename, const char *mode)
{
	FILE *retVal;
	retVal = fopen(filename, mode);
	if (retVal != NULL)
		return retVal;
	return NULL;	
}



void CFileRead::loadtextures(char *Filename)
{
	FILE *File;
	File = fileOpen(Filename,"r");

	memset(TextureImage, 0, sizeof(void *)*1);

	TextureImage[0] = auxDIBImageLoad(Filename);
	
	glGenTextures(1,&texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureImage[0]->sizeX, TextureImage[0]->sizeY,
					0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}