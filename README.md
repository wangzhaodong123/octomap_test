# octomap_test
改写高博的octomap_tutor，添加了直接由三维点填充octomap::ColorOcTree对象的程序octomap_only
# 文件
data/文件夹下存放的是rgbd_dataset_freiburg2_pioneer_slam2.bag中的十对数据，这十对数据是根据my_association.txt文件从数据集中复制的；

data/my_association.txt是rgb图像，depth图像以及位姿的关联文件，由我自己写的关联程序rgb_dep_gro_associate得到。
# 编译
在octomap_test/目录下执行 make ，有的需要 sudo make；会在当前目录下新建bin/和lib/两个子文件夹，bin/下有octomao_only和octomap_with_pcl两个可执行文件
# 运行
./bin/octomap_only data/      运行不创建pcl点云的程序
./bin/octomap_with_pcl data/  运行创建pcl点云的程序
octovis data/octomap_only.ot
octovis data/octomap_with_pcl.ot  查看两个程序所创建的地图
# 问题
自己写的octomap_only程序所创建的地图只有16M，而用高博程序原理所写的程序创建的地图却有244M，具体为什么还没弄清楚。
