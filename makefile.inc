ROOT_PATH1=$(shell work_path=`pwd`; \
until [ -e BLADE_ROOT ]; \
do \
cd ..; \
done; \
root_path=`pwd`; \
cd $${work_path};  \
echo $${root_path}) 


ROOT_PATH=$(strip ${ROOT_PATH1})
SRC_PATH=${ROOT_PATH}/src

