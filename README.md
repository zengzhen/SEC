SEC
===
 * Extract SEC of manipulation actions, and measure the similarity between two manipulation action sequences. 
 * Learn manipulation action for recognition tasks.

#Usage
1. In Matlab, navigate to `EECS556/code`; Run

        >> addpath(genpath(‘.’))
2. Manually unzip  the `ModelNumber_TissueType.mnc.gz` into `Brainweb_MRIDatabase/WebDownloads`, then run

        >> get_data_ready
 which will automatically generate original images in .ppm format, and ground truth labels `train_data.mat` in folder `Brainweb_MRIDatabase/Images`. <br />
3.  Run 

        >> mriTrain
    classifiers are learned and stored in `mri_results/train_results`
4. run 

        >> mriTestScript
    test images will be loaded and labeld based on learned classifiers. <br />
    Test results are stored in `mri_results/test_results`

#Parameters
Parameters that can be tuned: <br /> 
    `ncv`: nvc-fold cross-validation
    `nsegments`: list of number of regions for multiple hypothesis
    `train`: training images indicies <br /> 
    `test`: test iamges indicies <br /> 
    `trainind1`: training images for edge classifer <br /> 
    `sigma`: Used to smooth the input image before segmenting it (0.1) <br />
    `k`: Value for the threshold function (100) <br /> 
    `min`: Minimum component size enforced by post-processing (5) 
    
    util.cpp/linking parameters:
     * search radius around the center: 0.05
     * overlap_ratio threshold to establish linking: 0.0f
    trackRigid.cpp/tracker parmaeters:
     * icp max iteration number: 2000
     * search radius around each point of transformed cloud: 0.03
     * color threshold for match points between previous and current frame cloud: 6.0f
    pcd_cloud.cpp/region_grow color threshhold:
     * point neighboring: 6
     * region merging: 5
    extract_sec.cpp/segmentation threshold: (smalled cluster size)
     * initialSeg threshold: 100
     * tableObjSeg threshold: 10
    table_obj_seg.cpp/
     * threshold for finding plane: 0.01

#Troubleshooting
1. Complain about opencv2/flann, try directly build the project in terminal instead of using Kdevelop

        >> cd build
        >> cmake ..
        >> make
2. Segmentation fault after plane detection and object clusters extraction, 
 * in function `TableObject::Segmentation::seg(bool view2D)`, comment out `prune();`, since it might prune out the hand when the hand-arm is too close to the tabletop plane
 * display color detection result by setting `bool DEBUG_COLOR = true;` in `extract_sec.cpp`

	

#Git Clone Repository Instructions
If you have not generated any SSH key for github on your machine, please do so following this [tutorial](https://help.github.com/articles/generating-ssh-keys)

1. Create a new folder for the repository you are going to clone, and navigate into the folder, eg

		$  mkdir EECS556_code
		$  cd EECS556_code/
		$  git init
2. Set up git
		
		$  git remote add origin _SSH_
		$  git pull origin master
	where `_SSH_` is the SSH clone URL shown on the github repository webpage (near the bottom of the right column, above "Download ZIP").
3. Push to git
	After you commit any changes, push to git

		$  git push origin master



