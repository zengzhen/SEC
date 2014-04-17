/**
 * \file        TYPEDEF.h
 * \author      Zhen Zeng (zengzhen@umich.edu)
 * \brief       view 3d point cloud in 2d image
 */

#ifndef TYPEDEF
#define TYPEDEF

#include <pcl/point_cloud.h>
#include <pcl/tracking/particle_filter.h>

typedef pcl::PointXYZRGBA RefPointType;
typedef pcl::tracking::ParticleXYZRPY ParticleT;
typedef pcl::tracking::ParticleFilterTracker<RefPointType, ParticleT> ParticleFilter;
typedef pcl::PointCloud<RefPointType> Cloud;
typedef Cloud::Ptr CloudPtr;
typedef const CloudPtr CloudConstPtr;

struct linker{
    std::vector<int> link_index;
    std::vector<float> overlap_ratio;
};

struct graph{
    std::vector<int> relational_graph;
    int video_index;
};

struct sec{
    int row;
    std::vector<int> cols;
    std::vector<std::vector<const char*>> event_chain;
};

typedef std::vector<linker> linkerList;

extern bool VERBOSE;

extern bool DEBUG;

extern bool DEBUG_COLOR;

/* Tracking Parameters
 */
// util.cpp: linking parameters: [1] search radius around the center; [2] overlap_ratio threshold to establish linking
// trackRigid.cpp: tracker parmaeters: [3] icp max iteration number; [4] search radius around each point of transformed cloud; [5] color threshold for match points between previous and current frame cloud; 
// pcd_cloud.cpp: region_grow color threshhold ([6] point neighboring and [7] region merging)
// planar_segmentation.cpp: [8] tableObjSeg threshold; [9] initialSeg threshold
// table_obj_seg.cpp: [10] threshold for findplane
// [1] 0.05
// [2] 0.0f
// [3] 2000
// [4] 0.03
// [5] 6.0f
// [6] 6
// [7] 5
// [8] 10
// [9] 100
// [10] 0.01

#endif  // TYPEDEF