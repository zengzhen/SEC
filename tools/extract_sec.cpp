#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <pcl/io/pcd_io.h>
#include <pcl/io/openni_grabber.h>

#include <pcl/console/parse.h>

#include <pcl/common/time.h>
#include <pcl/common/centroid.h>
#include <pcl/common/distances.h>
#include <pcl/common/transforms.h>

#include <pcl/kdtree/kdtree_flann.h>

#include <pcl/search/pcl_search.h>

#include <pcl/visualization/cloud_viewer.h>
#include <pcl/visualization/pcl_visualizer.h>


#include <pcl/filters/passthrough.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/approximate_voxel_grid.h>

#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>

#include <pcl/registration/icp.h>
#include <pcl/registration/eigen.h>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include <pcl/filters/conditional_removal.h>
#include <pcl/filters/passthrough.h>

#include "TableObjectSegmentation/table_obj_seg.h"
#include "TableObjectSegmentation/pcd_cloud.h"
#include "Visualizer/view2D.h"
#include "Visualizer/view3D.h"
#include "Tracker/trackRigid.h"
#include "util/util.h"
#include "Detector/colorDetector.h"
#include "Detector/touchDetector.h"

#include "SEC/mainGraph.h"

bool VERBOSE = false;
bool DEBUG = false;
bool DEBUG_COLOR = false;

int
main (int argc, char** argv)
{
    pcl::visualization::PCLVisualizer result_viewer("planar_segmentation");
    result_viewer.addCoordinateSystem();
    result_viewer.setCameraPosition(-0.504623,0.0647437,-0.758519, -0.443141,0.0788583,-0.502855, 0.00533475,-0.998535,0.0538437);
    result_viewer.setCameraClipDistances(0.0136198,13.6198);
    //2.8494,8.48591/0.0712695,0.486438,0.865/-1.77289,1.38452,-4.06431/0.0224361,-0.982044,-0.187315/0.523599/1920,600/1921,52
    //clipDistance  / pos x, y, z              / view x, y, z            / up x, y, z                  / fovy   /win_size/win_pos
    //0.0136198,13.6198/-0.443141,0.0788583,-0.502855/-0.504623,0.0647437,-0.758519/0.00533475,-0.998535,0.0538437/0.523599/800,450/425,277
    
    std::string basename_cloud=argv[1];
    unsigned int index_start = std::atoi(argv[2]);
    unsigned int index_end = std::atoi(argv[3]);
    int view_id=-1;
    if(argc>4) view_id=std::atoi(argv[4]);
    if(argc>5) VERBOSE = (bool)argv[5];
    
    std::string basename_pcd = (basename_cloud.find(".pcd") == std::string::npos) ? (basename_cloud + ".pcd") : basename_cloud;
    std::string filename_pcd;
    
    boost::filesystem::path p(basename_cloud);
    boost::filesystem::path dir=p.parent_path();
    std::string mainGraph_file = dir.string();
    mainGraph_file = mainGraph_file + "mainGraph.txt";
    
    unsigned int idx = index_start;
    
    TableObject::Segmentation tableObjSeg;
    TableObject::Segmentation initialSeg;
    TableObject::trackRigid tracker;
    TableObject::view2D view2D;
    TableObject::colorDetector finger1Detector(0,100,0,100,100,200);
    TableObject::colorDetector finger2Detector(150,250,0,100,0,100);
    TableObject::colorDetector cupDetector(100,255,100,255,0,100);
    TableObject::colorDetector blockDetector(100,150,140,210,50,120);
    TableObject::touchDetector touchDetector(0.015);
    
    TableObject::mainGraph mainGraph((int)index_start);
    
    TableObject::pcdCloud pcdSceneCloud;
    CloudPtr sceneCloud;
    CloudPtr cloud_objects(new Cloud);
    CloudPtr cloud_finger1(new Cloud);
    CloudPtr cloud_finger2(new Cloud);
    CloudPtr cloud_cup(new Cloud);
    CloudPtr cloud_block(new Cloud);
    CloudPtr cloud_hull(new Cloud);
    std::vector<pcl::PointIndices> clusters;
    pcl::ModelCoefficients coefficients;
    
    tableObjSeg.setThreshold(10);
    initialSeg.setThreshold(200);
    
    pcl::PointIndices f1_indices;
    pcl::PointIndices f2_indices;
    pcl::PointIndices object_indices;
    
    int video_id=0;

    while( idx <= index_end && !result_viewer.wasStopped())
    { 
        std::cout << std::endl;
        std::cout << "frame id=" << idx << std::endl;
        filename_pcd = cv::format(basename_cloud.c_str(), idx);
        
        CloudPtr planeCloud(new Cloud);
        
        if(idx==index_start){
            /***************************************
             *  object cloud extraction
             ***************************************/
            initialSeg.resetCloud(filename_pcd);
            initialSeg.seg(false);
            initialSeg.getObjects(cloud_objects, clusters);
            initialSeg.getCloudHull(cloud_hull);
            initialSeg.getPlaneCoefficients(coefficients);
            initialSeg.getsceneCloud(pcdSceneCloud);
            initialSeg.getTableTopCloud(planeCloud);
            sceneCloud=pcdSceneCloud.getCloud();
            
            /***************************************
             *  fingertip, hand_arm extraction
             ***************************************/
            //opencv color filtering for fingertip_1
            finger1Detector.setInputCloud(cloud_objects, clusters);
            finger1Detector.filter(f1_indices,cloud_finger1);
            finger1Detector.showDetectedCloud(result_viewer, "finger1");
            
            //opencv color filtering for fingertip_2
            finger2Detector.setInputCloud(cloud_objects, clusters);
            finger2Detector.filter(f2_indices,cloud_finger2);
            finger2Detector.showDetectedCloud(result_viewer, "finger2");
            
            //opencv color filtering for block
//             cupDetector.setInputCloud(cloud_objects, clusters);
//             cupDetector.filter(object_indices, cloud_block);
//             cupDetector.showDetectedCloud(result_viewer, "block");
            
            // remove hand (include cluster that contain the detected fingertips and also the other clusters that are touching the cluster)
            std::vector<int> hand_arm=TableObject::findHand(cloud_objects, clusters, f1_indices);
            for(int i=hand_arm.size()-1; i>=0; i--)
            {
                clusters.erase(clusters.begin()+hand_arm[i]);
                std::cout << "removing hand_arm : cluster index = " << hand_arm[i] << std::endl;
            }
            
            /***************************************
             *  Tracking initialization
             ***************************************/
            tracker.init(cloud_objects, clusters);
            
            /***************************************
             *  Touch detection
             ***************************************/
            std::vector<pcl::PointIndices> touch_clusters=clusters;
            touch_clusters.push_back(f1_indices);
            touch_clusters.push_back(f2_indices);
            std::cout << "touch_clusters size = " << touch_clusters.size() << std::endl;
            // touch detection between each pair of objects (including fingertips, tabletop objects and tabletop)
            for(int i=0; i<touch_clusters.size(); i++)
            {
                CloudPtr object_i(new Cloud);
                pcl::copyPointCloud(*cloud_objects, touch_clusters[i], *object_i);
                int j;
                bool touch;
                for(j=i+1; j<touch_clusters.size(); j++)
                {
                    CloudPtr object_j(new Cloud);
                    pcl::copyPointCloud(*cloud_objects, touch_clusters[j], *object_j);
                
                    // touch detection between object_i and object_j
                    char relation [50];
                    sprintf(relation, "object%d_object%d", i, j);
                    std::cout << relation << std::endl;
                    touch=touchDetector.detect(object_i, object_j);
                    touchDetector.showTouch(result_viewer, relation, 100+250*(j-i-1), 40+20*i);
                    
                    // relational scene graph -> main graph
                    if(touch) {
                        mainGraph.addInitialRelationalGraph(2);
                    }else{
                        mainGraph.addInitialRelationalGraph(0);
                    }
                }
                
                // touch detection between each objects and tabletop
                char relation [50];
                sprintf (relation, "object%d_object%d", i, (int)touch_clusters.size());
                std::cout << relation << std::endl;
                touch=touchDetector.detectTableTouch(object_i, coefficients);
                touchDetector.showTouch(result_viewer, relation, 100+250*(j-i-1), 40+20*i);
                
                // relational scene graph -> main graph
                if(touch) {
                    mainGraph.addInitialRelationalGraph(2);
                }else{
                    mainGraph.addInitialRelationalGraph(0);
                }
            }
            
            
            /***************************************
             *  Visualization
             ***************************************/  
            // darw original cloud
//             pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGBA> rgb(sceneCloud);
//             result_viewer.addPointCloud<RefPointType>(sceneCloud, rgb, "new frame");
            
            // draw extracted object clusters
//             TableObject::view3D::drawClusters(result_viewer, cloud_objects, touch_clusters);  

            // draw extracted plane points
//             pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGBA> plane(planeCloud);
//             result_viewer.addPointCloud<RefPointType>(planeCloud, plane, "tabletop");
            std::stringstream ss; 
            ss << (int)touch_clusters.size();
            result_viewer.addText3D(ss.str(), planeCloud->points.at(334*640+78),0.1);
            
            // draw extracted plane contour polygon
            result_viewer.addPolygon<RefPointType>(cloud_hull, 0, 255, 0, "polygon");
        }else
        {            
            /***************************************
             *  object cloud extraction
             ***************************************/
            tableObjSeg.resetCloud(filename_pcd);
            tableObjSeg.seg(cloud_hull,false);
            tableObjSeg.getObjects(cloud_objects, clusters);
            tableObjSeg.getsceneCloud(pcdSceneCloud);
            sceneCloud=pcdSceneCloud.getCloud();
            
            /***************************************
             *  fingertip extraction
             ***************************************/
            //opencv color filtering for fingertip_1
            finger1Detector.setInputCloud(cloud_objects, clusters);
            finger1Detector.filter(f1_indices,cloud_finger1);
            finger1Detector.showDetectedCloud(result_viewer, "finger1");

            //opencv color filtering for fingertip_2
            finger2Detector.setInputCloud(cloud_objects, clusters);
            finger2Detector.filter(f2_indices,cloud_finger2);
            finger2Detector.showDetectedCloud(result_viewer, "finger2");
            
            /***************************************
             *  Tracking objects
             ***************************************/
            std::vector<pcl::PointIndices> tracked_clusters;
            tracker.linking(cloud_objects, clusters);
            tracker.track(cloud_objects,tracked_clusters);
//             tracker.viewTranformedCloud(result_viewer, view_id);
            for(int i=0; i<tracked_clusters.size(); i++)
            {
                tracker.viewTrackedCloud(result_viewer, i, 255, 0, 0);
            }
            
            /***************************************
             *  Touch detection
             ***************************************/
            std::vector<pcl::PointIndices> touch_clusters=tracked_clusters;
            touch_clusters.push_back(f1_indices);
            touch_clusters.push_back(f2_indices);
            std::cout << "touch_clusters size = " << touch_clusters.size() << std::endl;
            // touch detection between each pair of objects (including fingertips, tabletop objects and tabletop)
            for(int i=0; i<touch_clusters.size(); i++)
            {
                CloudPtr object_i(new Cloud);
                pcl::copyPointCloud(*cloud_objects, touch_clusters[i], *object_i);
                int j;
                bool touch;
                for(j=i+1; j<touch_clusters.size(); j++)
                {
                    CloudPtr object_j(new Cloud);
                    pcl::copyPointCloud(*cloud_objects, touch_clusters[j], *object_j);
                
                    // touch detection between object_i and object_j
                    char relation [50];
                    sprintf(relation, "object%d_object%d", i, j);
                    std::cout << relation << std::endl;
                    touch=touchDetector.detect(object_i, object_j);
                    touchDetector.showTouch(result_viewer, relation, 100+250*(j-i-1), 40+20*i);
                    
                    // relational scene graph -> main graph
                    if(touch) {
                        mainGraph.addRelationalGraph(2);
                    }else{
                        mainGraph.addRelationalGraph(0);
                    }
                }
                
                // touch detection between each objects and tabletop
                char relation [50];
                sprintf (relation, "object%d_object%d", i, (int)touch_clusters.size());
                std::cout << relation << std::endl;
                touch=touchDetector.detectTableTouch(object_i, coefficients);
                touchDetector.showTouch(result_viewer, relation, 100+250*(j-i-1), 40+20*i);
                
                // relational scene graph -> main graph
                if(touch) {
                    mainGraph.addRelationalGraph(2);
                }else{
                    mainGraph.addRelationalGraph(0);
                }
            }

            /***************************************
             *  Visualization
             ***************************************/
            // draw extracted point clusters
            TableObject::view3D::drawText(result_viewer, cloud_objects, touch_clusters);
            
            /***************************************
             *  Main Graph
             ***************************************/
            mainGraph.compareRelationGraph((int)idx);
        }       
        
             
//         result_viewer.setRepresentationToSurfaceForAllActors();
//         pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGBA> rgb(sceneCloud);
//         result_viewer.updatePointCloud<RefPointType>(sceneCloud, rgb, "new frame");

        result_viewer.spinOnce (100);
        boost::this_thread::sleep (boost::posix_time::microseconds (100000));
        
        char screenshot[50]; // make sure it's big enough
        snprintf(screenshot, sizeof(screenshot), "result/demo10/original/sec_%d.png", (int)video_id);
        std::cout << screenshot << std::endl;
        result_viewer.saveScreenshot(screenshot);
        
        idx=idx+3;
        video_id=video_id+1;
    }
    
    mainGraph.displayMainGraph();
    mainGraph.recordMainGraph(mainGraph_file);
    
    
    
    while (!result_viewer.wasStopped ())
    {
        result_viewer.spinOnce (100);
        boost::this_thread::sleep (boost::posix_time::microseconds (100000));
    }
    
}