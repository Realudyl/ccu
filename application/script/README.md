本文将逐一介绍app工程下的script文件夹中各文件的用途：
1、<Ccu.sct> ：
该sct文件为app工程的原有sct文件副本，主要储存app代码链路；
用途：bootloader工程编译时会将该app的sct文件与bootloader的sct文件合成为<project.sct>文件，供app工程选用编译。

2、<project.sct> ：
该sct文件由bootloader工程编译时所运行的<CreatUpgrader.bat>脚本将app的<Ccu.sct>文件与bootloader的<bootloader.sct>文件合成，供app工程选用编译。

3、<getVersion.bat> ：
该bat脚本在编译app工程时会先运行，其作用是运行<version_maker.sh>脚本，进而获取“版本信息”和“提交时间”。

4、<creatUpdateFile.bat> ：
该bat脚本在编译app工程完成后会运行，
主要用于运行<configParse.exe>从</tools/update/config.xml>中获取“设备信息”以及<md5sum.exe>生成md5校验值，
并与hex文件合成新的hex文件到根目录的update目录下。

5、<version_maker.sh> ：
该sh脚本由<getVersion.bat>脚本运行，主要用于提取<AutoGenVersion.h>头文件（该文件位于app目录下的/user/app/main中）中的“版本信息”和“提交时间”。

6、<md5sum.exe> ：
该exe文件由<creatUpdateFile.bat>脚本运行，用于生成md5文件与hex文件合成。






注意：app工程设置编译脚本处的“..\script\creatUpdateFile.bat <设备名称>”，用户定义<设备名称>得能</tools/update/config.xml>文件中找到。
