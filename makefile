#OPENCV
INCLUDE_OPENCV = $(shell pkg-config --cflags opencv)
LIBDIR_OPENCV = $(shell pkg-config --libs opencv)

#eigen3
INCLUDE_EIGEN = -I /usr/include/eigen3

#PCL
INCLUDE_PCL = -I /usr/include/pcl-1.7
LIBDIR_PCL = -lpcl_common -lpcl_octree -lpcl_io -lpcl_common -lpcl_octree 

#BOOST
LIBDIR_BOOST = -lboost_thread -lboost_date_time -lboost_iostreams -lboost_serialization -lboost_system -lboost_filesystem

#OPENNI
INCLUDE_OPENNI = -I /usr/include/ni
LIBDIR_OPENNI = -lOpenNI

#VTK
INCLUDE_VTK = -I/usr/include/vtk-5.8
LIBDIR_VTK = /usr/lib/libvtkViews.so.5.8.0 /usr/lib/libvtkInfovis.so.5.8.0 \
	     /usr/lib/libvtkWidgets.so.5.8.0 /usr/lib/libvtkHybrid.so.5.8.0 \
	     /usr/lib/libvtkParallel.so.5.8.0 /usr/lib/libvtkVolumeRendering.so.5.8.0 \
	     /usr/lib/libvtkRendering.so.5.8.0 /usr/lib/libvtkGraphics.so.5.8.0 \
	     /usr/lib/libvtkImaging.so.5.8.0 /usr/lib/libvtkIO.so.5.8.0 \
	     /usr/lib/libvtkFiltering.so.5.8.0 /usr/lib/libvtkCommon.so.5.8.0 \
	     /usr/lib/libvtksys.so.5.8.0 
#G2O
INCLUDE_G2O = -I /usr/local/include/g2o
LIBDIR_G2O = /usr/local/lib/libg2o_core.so /usr/local/lib/libg2o_types_sba.so \
	/usr/local/lib/libg2o_solver_csparse.so /usr/local/lib/libg2o_csparse_extension.so \
	/usr/local/lib/libg2o_stuff.so
#cotomap
INCLUDE_OCTOMAP = -I /usr/local/include/octomap
LIBDIR_OCTOMAP = /usr/local/lib/liboctomap.so.1.9 \
		 /usr/local/lib/liboctomath.so 
all:mkdir octomap_with_pcl octomap_only

mkdir:
	rm -fr bin/
	rm -fr lib/
	mkdir bin
	mkdir lib
octomap_only:octomap_only.o 
	g++ -std=c++11 -O3 -Wno-deprecated -o octomap_only lib/octomap_only.o $(LIBDIR_OCTOMAP) $(LIBDIR_OPENCV)
	mv octomap_only bin/
octomap_only.o:src/octomap_only.cpp
	g++ -std=c++11 -O3 -Wno-deprecated -c src/octomap_only.cpp $(INCLUDE_OCTOMAP) $(INCLUDE_OPENCV) $(INCLUDE_EIGEN)
	mv octomap_only.o lib/
octomap_with_pcl:octomap_with_pcl.o 
	g++ -std=c++11 -O3 -Wno-deprecated -o octomap_with_pcl lib/octomap_with_pcl.o $(LIBDIR_OCTOMAP) $(LIBDIR_PCL) $(LIBDIR_OPENCV) $(LIBDIR_BOOST)
	mv octomap_with_pcl bin/
octomap_with_pcl.o:src/octomap_with_pcl.cpp
	g++ -std=c++11 -O3 -Wno-deprecated -c src/octomap_with_pcl.cpp $(INCLUDE_OCTOMAP) $(INCLUDE_OPENCV) $(INCLUDE_EIGEN) $(INCLUDE_PCL)	
	mv octomap_with_pcl.o lib/
clean:
	rm -f lib/*.o bin/octomap_only bin/octomap_with_pcl