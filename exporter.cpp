// Convert a Goxel text file to a saved octomap file
// Notes:
//   - Uses Octomap 1.9.0

#include <fstream>
#include <iostream>
#include <octomap/OcTree.h>
#include <octomap/Pointcloud.h>
#include <string>


int main(int argc, char** argv)
{
	// Processs args
	if (argc < 3)
	{
		std::cerr << "Usage: exporter <infile> <outfile> <resolution> [origin] \n";
	}

	auto infile = std::string(argv[1]);
	auto outfile = std::string(argv[2]);
	auto resolution = std::atof(argv[3]);
	
	octomap::Pointcloud pc;
	std::ifstream gox(infile);

	// Process each line
	//  1) Figure in resolution to calculate points
	//  2) Add point to an octomap::PointCloud object
	std::string line;
	while (line.empty() || line.front() == '#') std::getline(gox, line);
		

	float resOffset = -0.001;
	std::string x, y, z, c;
	while (gox >> x >> y >> z >> c)
	{
		if (x == "#") continue;

		pc.push_back(std::stof(x) * resolution + resOffset,
					 std::stof(y) * resolution + resOffset,
					 std::stof(z) * resolution + resOffset);

//		std::cout << std::stof(x) * resolution << " "
//				  << std::stof(y) * resolution << " "
//				  << std::stof(z) * resolution << std::endl;
	}
	
	// Insert point cloud into new octomap
	auto octree = octomap::OcTree(resolution);
	octree.insertPointCloud(pc, octomap::point3d(0, 0, 0));
	
	// Write octomap to disk
	octree.writeBinary(outfile);

	return 0;
}
