# 提取头文件中的版本信息和提交时间
HEADER_FILE="../user/app/main/AutoGenVersion.h"

# 调试信息：打印头文件内容
#echo "Contents of $HEADER_FILE:"
cat $HEADER_FILE

# 提取版本号和提交时间
APP_VERSION=$(cat $HEADER_FILE | grep "#define cAppVersion" | cut -d '"' -f 2)
APP_COMMIT_TIME=$(cat $HEADER_FILE | grep "#define cAppCommitTime" | cut -d '"' -f 2)

# 调试信息：打印提取到的值
#echo "Extracted APP_VERSION: $APP_VERSION"
#echo "Extracted APP_COMMIT_TIME: $APP_COMMIT_TIME"

# 将版本号写入rename.txt
echo $APP_VERSION >> ./rename.txt

# 创建version.txt并写入提取到的值
echo Creation Time:$APP_COMMIT_TIME > ./version.txt
echo Firmware Ver:$APP_VERSION >> ./version.txt

# 删除update目录下的所有.hex文件
rm ../update/*.hex
