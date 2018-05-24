// Convert a Goxel text file to a saved octomap file
// Notes:
//   - Uses Octomap 1.9.0

#include <cmath>
#include <fstream>
#include <iostream>
#include <octomap/OcTree.h>
#include <octomap/Pointcloud.h>
#include <octomap/math/Vector3.h>
#include <string>

int cnt = 0;

void splitPoint(octomap::Pointcloud& pc, octomath::Vector3 pt, float res, int split)
{
	if (split == 0)
	{
		cnt++;
		pc.push_back(pt);
		return;
	}

	res /= 4.;	
	splitPoint(pc, {pt.x() + res, pt.y() + res, pt.z() + res}, res, split-1);
	splitPoint(pc, {pt.x() - res, pt.y() - res, pt.z() - res}, res, split-1);
	splitPoint(pc, {pt.x() + res, pt.y() - res, pt.z() - res}, res, split-1);
	splitPoint(pc, {pt.x() - res, pt.y() + res, pt.z() - res}, res, split-1);
	splitPoint(pc, {pt.x() - res, pt.y() - res, pt.z() + res}, res, split-1);
	splitPoint(pc, {pt.x() - res, pt.y() + res, pt.z() + res}, res, split-1);
	splitPoint(pc, {pt.x() + res, pt.y() - res, pt.z() + res}, res, split-1);
	splitPoint(pc, {pt.x() + res, pt.y() + res, pt.z() - res}, res, split-1);	
}


int main(int argc, char** argv)
{
	// Processs args
	if (argc < 5)
	{
		std::cerr << "Usage: exporter <infile> <outfile> <goxel res> <voxel res> [split] [offset] [origin] \n";
		return -1;
	}
 
	std::string infile = std::string(argv[1]);
	std::string outfile = std::string(argv[2]);
	float goxelRes = std::atof(argv[3]);
	float voxelRes = std::atof(argv[4]);
    int split = argc > 5 ? std::atoi(argv[5]) : 1;
	float resOffset = argc > 6 ? std::atof(argv[6]) : 0.;
	
	octomap::Pointcloud pc;
	std::ifstream gox(infile);

	// Process each line
	//  1) Figure in resolution to calculate points
	//  2) Add point to an octomap::PointCloud object
	std::string line;
	while (line.empty() || line.front() == '#') std::getline(gox, line);
		

//	float resOffset = -0.00; // need this so adjacent voxels connect in octomap
	std::string x, y, z, c;
	while (gox >> x >> y >> z >> c)
	{
		if (x == "#") continue;

		octomath::Vector3 pt(std::stof(x) * goxelRes + resOffset,
							 std::stof(y) * goxelRes + resOffset,
							 std::stof(z) * goxelRes + resOffset);

		// Interpolate points within a cube specified by goxelRes		
		splitPoint(pc, pt, goxelRes, split);
	}

	
	// Insert point cloud into new octomap
	float newRes = goxelRes / std::pow(2., split);
	std::cout << "final point cloud res: " << newRes << std::endl;
	std::cout << "outputting voxel res: " << voxelRes << std::endl;
	std::cout << "writing " << cnt << " points" << std::endl;

	auto octree = octomap::OcTree(voxelRes);
	octree.insertPointCloud(pc, octomap::point3d(0, 0, 0));
	
	// Write octomap to disk
	octree.writeBinary(outfile);

	return 0;
}
