// time.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"




#include <iostream>
#include <pcl/point_cloud.h>
#include <pcl/io/obj_io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <time.h>

using namespace std;
using namespace pcl;

class StopWatch
{
	boost::posix_time::ptime before;
	boost::posix_time::ptime after;
public:
	void start() {
		before = boost::posix_time::microsec_clock::local_time();
	}

	double stop() {
		after = boost::posix_time::microsec_clock::local_time();
		return (static_cast<double> ((after - before).total_milliseconds()));
	}
};

int oldsaveOBJFile(const std::string &file_name,
	const pcl::PolygonMesh &mesh, unsigned precision = 5)
{
	if (mesh.cloud.data.empty())
	{
		PCL_ERROR("[pcl::io::saveOBJFile] Input point cloud has no data!\n");
		return (-1);
	}
	// Open file
	std::ofstream fs;
	fs.precision(precision);
	fs.open(file_name.c_str());

	/* Write 3D information */
	// number of points
	int nr_points = mesh.cloud.width * mesh.cloud.height;
	// point size
	unsigned point_size = static_cast<unsigned> (mesh.cloud.data.size() / nr_points);
	// number of faces for header
	unsigned nr_faces = static_cast<unsigned> (mesh.polygons.size());
	// Do we have vertices normals?
	int normal_index = getFieldIndex(mesh.cloud, "normal_x");
	const float *mesh_cloud_data = (float *)&mesh.cloud.data[0];

	// Write the header information
	fs << "####" << std::endl;
	fs << "# OBJ dataFile simple version. File name: " << file_name << std::endl;
	fs << "# Vertices: " << nr_points << std::endl;
	if (normal_index != -1)
		fs << "# Vertices normals : " << nr_points << std::endl;
	fs << "# Faces: " << nr_faces << std::endl;
	fs << "####" << std::endl;

	// Write vertex coordinates
	fs << "# List of Vertices, with (x,y,z) coordinates, w is optional." << std::endl;
	for (int i = 0; i < nr_points; ++i)
	{
		int xyz = 0;
		for (size_t d = 0; d < mesh.cloud.fields.size(); ++d)
		{
			int c = 0;
			// adding vertex
			if ((mesh.cloud.fields[d].datatype == pcl::PCLPointField::FLOAT32) && (
				mesh.cloud.fields[d].name == "x" ||
				mesh.cloud.fields[d].name == "y" ||
				mesh.cloud.fields[d].name == "z"))
			{
				if (mesh.cloud.fields[d].name == "x")
					// write vertices beginning with v
					fs << "v ";

				float value;
				memcpy(&value, &mesh.cloud.data[i * point_size + mesh.cloud.fields[d].offset + c * sizeof(float)], sizeof(float));
				fs << value;
				if (++xyz == 3)
					break;
				fs << " ";
			}
		}
		if (xyz != 3)
		{
			PCL_ERROR("[pcl::io::saveOBJFile] Input point cloud has no XYZ data!\n");
			return (-2);
		}
		fs << std::endl;
	}

	fs << "# " << nr_points << " vertices" << std::endl;

	if (normal_index != -1)
	{
		fs << "# Normals in (x,y,z) form; normals might not be unit." << std::endl;
		// Write vertex normals
		for (int i = 0; i < nr_points; ++i)
		{
			int nxyz = 0;
			for (size_t d = 0; d < mesh.cloud.fields.size(); ++d)
			{
				int c = 0;
				// adding vertex
				if ((mesh.cloud.fields[d].datatype == pcl::PCLPointField::FLOAT32) && (
					mesh.cloud.fields[d].name == "normal_x" ||
					mesh.cloud.fields[d].name == "normal_y" ||
					mesh.cloud.fields[d].name == "normal_z"))
				{
					if (mesh.cloud.fields[d].name == "normal_x")
						// write vertices beginning with vn
						fs << "vn ";

					float value;
					memcpy(&value, &mesh.cloud.data[i * point_size + mesh.cloud.fields[d].offset + c * sizeof(float)], sizeof(float));
					fs << value;
					if (++nxyz == 3)
						break;
					fs << " ";
				}
			}
			if (nxyz != 3)
			{
				PCL_ERROR("[pcl::io::saveOBJFile] Input point cloud has no normals!\n");
				return (-2);
			}
			fs << std::endl;
		}

		fs << "# " << nr_points << " vertices normals" << std::endl;
	}

	fs << "# Face Definitions" << std::endl;
	// Write down faces
	if (normal_index == -1)
	{
		for (unsigned i = 0; i < nr_faces; i++)
		{
			fs << "f ";
			size_t j = 0;
			for (; j < mesh.polygons[i].vertices.size() - 1; ++j)
				fs << mesh.polygons[i].vertices[j] + 1 << " ";
			fs << mesh.polygons[i].vertices[j] + 1 << std::endl;
		}
	}
	else
	{
		for (unsigned i = 0; i < nr_faces; i++)
		{
			fs << "f ";
			size_t j = 0;
			for (; j < mesh.polygons[i].vertices.size() - 1; ++j)
				fs << mesh.polygons[i].vertices[j] + 1 << "//" << mesh.polygons[i].vertices[j] + 1 << " ";
			fs << mesh.polygons[i].vertices[j] + 1 << "//" << mesh.polygons[i].vertices[j] + 1 << std::endl;
		}
	}
	fs << "# End of File" << std::endl;

	// Close obj file
	fs.close();
	return 0;
}

int newsaveOBJFile(const std::string &file_name,
	const pcl::PolygonMesh &mesh, unsigned precision = 5)
{
	if (mesh.cloud.data.empty())
	{
		PCL_ERROR("[pcl::io::saveOBJFile] Input point cloud has no data!\n");
		return (-1);
	}
	// Open file
	std::ofstream fs;
	fs.precision(precision);
	fs.open(file_name.c_str());

	/* Write 3D information */
	// number of points
	int nr_points = mesh.cloud.width * mesh.cloud.height;
	// point size
	unsigned point_size = static_cast<unsigned> (mesh.cloud.data.size() / nr_points);  //16
	// number of faces for header
	unsigned nr_faces = static_cast<unsigned> (mesh.polygons.size());
	// Do we have vertices normals?
	int normal_index = getFieldIndex(mesh.cloud, "normal_x");  // -1
	const float *mesh_cloud_data = (float *)&mesh.cloud.data[0];  //???

	cout << mesh.cloud.fields.size() << " " << mesh.cloud.width * mesh.cloud.height << " " << static_cast<unsigned> (mesh.cloud.data.size() / nr_points) << " " << normal_index << endl; 


	// Write the header information
	fs << "####" << '\n';
	fs << "# OBJ dataFile simple version. File name: " << file_name << '\n';
	fs << "# Vertices: " << nr_points << '\n';
	if (normal_index != -1)
		fs << "# Vertices normals : " << nr_points << '\n';
	fs << "# Faces: " << nr_faces << '\n';
	fs << "####" << '\n';

	// Write vertex coordinates
	fs << "# List of Vertices, with (x,y,z) coordinates, w is optional." << '\n';
	for (int i = 0; i < nr_points; ++i)
	{
		int xyz = 0;
		for (size_t d = 0; d < mesh.cloud.fields.size(); ++d)     //3
		{
			int c = 0;
			// adding vertex
			if ((mesh.cloud.fields[d].datatype == pcl::PCLPointField::FLOAT32) && (
				mesh.cloud.fields[d].name == "x" ||
				mesh.cloud.fields[d].name == "y" ||
				mesh.cloud.fields[d].name == "z"))
			{
				if (mesh.cloud.fields[d].name == "x")
					// write vertices beginning with v
					fs << "v ";

				float value;
				memcpy(&value, &mesh.cloud.data[i * point_size + mesh.cloud.fields[d].offset + c * sizeof(float)], sizeof(float));  //c一直为0
				fs << value;
				if (++xyz == 3)
					break;      //跳出for  3  循环
				fs << " ";
			}
		}
		if (xyz != 3)
		{
			PCL_ERROR("[pcl::io::saveOBJFile] Input point cloud has no XYZ data!\n");
			return (-2);
		}
		fs << '\n';
	}

	fs << "# " << nr_points << " vertices" << '\n';

	if (normal_index != -1)
	{
		fs << "# Normals in (x,y,z) form; normals might not be unit." << '\n';
		// Write vertex normals
		for (int i = 0; i < nr_points; ++i)
		{
			int nxyz = 0;
			for (size_t d = 0; d < mesh.cloud.fields.size(); ++d)
			{
				int c = 0;
				// adding vertex
				if ((mesh.cloud.fields[d].datatype == pcl::PCLPointField::FLOAT32) && (
					mesh.cloud.fields[d].name == "normal_x" ||
					mesh.cloud.fields[d].name == "normal_y" ||
					mesh.cloud.fields[d].name == "normal_z"))
				{
					if (mesh.cloud.fields[d].name == "normal_x")
						// write vertices beginning with vn
						fs << "vn ";

					float value;
					memcpy(&value, &mesh.cloud.data[i * point_size + mesh.cloud.fields[d].offset + c * sizeof(float)], sizeof(float));
					fs << value;
					if (++nxyz == 3)
						break;
					fs << " ";
				}
			}
			if (nxyz != 3)
			{
				PCL_ERROR("[pcl::io::saveOBJFile] Input point cloud has no normals!\n");
				return (-2);
			}
			fs << '\n';
		}

		fs << "# " << nr_points << " vertices normals" << '\n';
	}

	fs << "# Face Definitions" << '\n';
	// Write down faces
	if (normal_index == -1)
	{
		for (unsigned i = 0; i < nr_faces; i++)
		{
			fs << "f ";
			size_t j = 0;
			for (; j < mesh.polygons[i].vertices.size() - 1; ++j)
				fs << mesh.polygons[i].vertices[j] + 1 << " ";
			fs << mesh.polygons[i].vertices[j] + 1 << '\n';
		}
	}
	else
	{
		for (unsigned i = 0; i < nr_faces; i++)
		{
			fs << "f ";
			size_t j = 0;
			for (; j < mesh.polygons[i].vertices.size() - 1; ++j)
				fs << mesh.polygons[i].vertices[j] + 1 << "//" << mesh.polygons[i].vertices[j] + 1 << " ";
			fs << mesh.polygons[i].vertices[j] + 1 << "//" << mesh.polygons[i].vertices[j] + 1 << '\n';
		}
	}
	fs << "# End of File" << '\n';

	// Close obj file
	fs.close();
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{

	PointCloud<PointXYZ>::Ptr pcd(new PointCloud<PointXYZ>);
	io::loadPCDFile("object_template_1.pcd", *pcd);

	cout << pcd->points.size() << " " << *pcd->points.data() << " " << pcd->width * pcd->height ; 



	PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>);
	for (int y = 0; y < 480; y++) {
		for (int x = 0; x < 640; x++) {
			cloud->points.push_back(PointXYZ(x, y, 1));
		}
	}

	PolygonMesh mesh;
	toPCLPointCloud2(*cloud, mesh.cloud);

	StopWatch watch;
	watch.start();
	oldsaveOBJFile("pcl_saveOBJFile.obj", mesh);
	cout << "time for old_saveOBJFile = " << watch.stop() << "ms" << endl;

	watch.start();
	newsaveOBJFile("new_saveOBJFile.obj", mesh);
	cout << "time for new_saveOBJFile = " << watch.stop() << "ms" << endl;

	return 0;
}