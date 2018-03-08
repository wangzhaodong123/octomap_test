/*
 * 模仿高博的 octomap_tutor
 * 将数据集 rgbd_dataset_freiburg2_pioneer_slam2.bag 的数据用 octomap 绘制成八叉图
 * 这里只根据 my_association.txt 文件上传了十对数据
 * 如果需要建立所有帧或者修改 KEYFRAMEDURATION 则需要用原始的 rgbd_dataset_freiburg2_pioneer_slam2.bag 数据包
 * 
 * /data文件夹下应有
 *    /rgb 			（存放RGB图片）
 *    /depth			（存放深度图片）
 *    my_association.txt	（关联文件，这个文件是由我自己写的程序（rgb_dep_gro_associate）得到的输出文件）
 * 
 * 在octomap_wtih_pcl里，每建立一个关键帧都特别费时，而且终端容易卡住
 * 于是我就翻octomap的函数手册，发现有 setNodeColor() 函数，于是我就跳过pcl点云部分，直接生成octomap地图
 */

#include <iostream>
#include <vector>
#include <sstream>
//octomap 
#include <octomap/octomap.h>
#include <octomap/ColorOcTree.h>
#include <octomap/math/Pose6D.h>
//opencv
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
//eigen
#include <Eigen/Core>
#include <Eigen/Geometry> 

using namespace std;
using namespace cv;

//相机内参
float camera_scale  = 1000;
float camera_cx     = 325.5;
float camera_cy     = 253.5;
float camera_fx     = 518.0;
float camera_fy     = 519.0;

//表示位姿的结构体
struct Pose
{
  float tx;
  float ty;
  float tz;
  float qx;
  float qy;
  float qz;
  float qw;
};

//每隔多少张图片建立一个关键帧
#define KEYFRAMEDURATION 30

int main( int argc, char** argv )
{
  //校验输入格式
  if ( argc !=2 )
  {
    cerr<<" 请输入 ： ./octomap_test data/（包含 rgb/,depth/,my_association.txt 的数据集地址） "<<endl;
    return 1;
  }
  
  //打开输入参数目录下的my_association.txt文件
  string string_data = argv[1];
  string string_my_association = string_data + "my_association.txt";
  string string_unused;
  ifstream file_my_association( string_my_association );
  if ( !file_my_association.is_open() )
  {
    cerr<<" 关联文件 my_association.txt 打开失败！ "<<endl;
    return 1;
  }
  getline( file_my_association, string_unused, '\n' );
  getline( file_my_association, string_unused, '\n' );
  cout<<"关联文件打开成功！"<<endl;
  cout<<"#   rgb    depth    tx    ty    tz    qx    qy    qz    qw "<<endl;
  
  //建立彩色地图
  octomap::ColorOcTree tree( 0.05 ); 
  
  //循环从文件中读取图片地址和位姿信息
  int count_line = 0;
  int count_group = 0;
  while ( !file_my_association.eof() )
  {
    string string_rgb;
    string string_depth;
    Pose pose_struct;
    //文件流读取信息
    file_my_association>>string_rgb>>string_depth
			>>pose_struct.tx>>pose_struct.ty>>pose_struct.tz
			>>pose_struct.qx>>pose_struct.qy>>pose_struct.qz>>pose_struct.qw;
    
    //每间隔 KEYFRAMEDURATION 选取一个关键帧
     if ( count_group != KEYFRAMEDURATION )
     {
       count_group++;
       continue;
     }
     count_group=0;
     count_line++;

    //使用 eigen 的类保存位姿，方便计算
    Eigen::Quaterniond q( pose_struct.qw, pose_struct.qx, pose_struct.qy, pose_struct.qz );
    Eigen::Isometry3d pose_eigen(q);
    pose_eigen.pretranslate( Eigen::Vector3d( pose_struct.tx, pose_struct.ty, pose_struct.tz ) );
    
    string_rgb = string_data+string_rgb;
    string_depth = string_data+string_depth;
    
        cout<<string_rgb<<"   "<<string_depth<<"  "
    	<<pose_struct.tx<<"  "<<pose_struct.ty<<"  "<<pose_struct.tz<<"  "
	<<pose_struct.qx<<"  "<<pose_struct.qy<<"  "<<pose_struct.qz<<"  "<<pose_struct.qw<<endl;;
    
    Mat image_rgb = imread(string_rgb);
    Mat image_depth = imread(string_depth,-1);

    for ( int m=0; m<image_depth.rows; m++ )
      for ( int n=0; n<image_depth.cols; n++ )
      {
        ushort d = image_depth.at<unsigned short>(m,n);
        if (d == 0)
          continue;
	Eigen::Vector3d point_camera;
	point_camera[2] = double(d) / camera_scale;
        point_camera[0] = (n - camera_cx) * point_camera[2] / camera_fx;
        point_camera[1] = (m - camera_cy) * point_camera[2] / camera_fy;
	Eigen::Vector3d point_world = pose_eigen*point_camera;
        uchar b = image_rgb.at<Vec3b>(m,n)[0];
        uchar g = image_rgb.at<Vec3b>(m,n)[1];
        uchar r = image_rgb.at<Vec3b>(m,n)[2];

	//将3D点插入地图
	octomap::point3d octomap_point3d( point_world[0],point_world[1],point_world[2] );
	octomap::ColorOcTreeNode* node = tree.updateNode(octomap_point3d,true);
	node->setColor(r,g,b);
	//tree.updateNode(octomap_point3d,true);
 	//tree.setNodeColor(point_world[0],point_world[1],point_world[2],r,g,b);
      }
 
    tree.updateInnerOccupancy();
    //count_line++;
    cout<<"第"<<count_line<<"组数据处理成功......"<<endl;
        
    if (count_line==10)
      break;
  }
    
  
  tree.write( string_data +  "octomap_only.ot" );
   
  file_my_association.close();
  cout<<"共处理了"<<count_line<<"组数据。"<<endl;  
 
  return 0;

}
