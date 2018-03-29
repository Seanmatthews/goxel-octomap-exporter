# goxel-octomap-exporter

## Build instructions
`g++ exporter.cpp -o exporter -std=c++11 -L ${OCTOMAP}/lib -loctomap -loctomath -I${OCTOMAP}/octomap/include`