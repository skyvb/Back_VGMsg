/**
自动备份Xen服务器VG存储信息
备份的意义:出现误删除虚拟机时,使用VG信息恢复被删除的虚拟机
备份文件目录:/usr/back
作者:木鱼De眼泪
QQ:2269812208
使用方法:
1.使用gcc编译成back_vg
gcc -o back_vg main.c
2.创建定时任务
crontab -e
添加(每天1:30执行)
30 1 * * * /root/back_vg
3.程序会自动创建备份文件目录(如需自定义目录,修改Back_Path)
4.程序会自动删除四天前的备份文件(如需修改时间,修改if(tt-t>345600)中的345600[计算方式:1天*86400])
**/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

//定义源文件夹,目标文件夹
#define Source_Path "/etc/lvm/backup"
#define Back_Path "/usr/back"

//创建文件夹
int CreatBack(){
    DIR *dir=opendir(Back_Path);
    if(dir==NULL){
       return mkdir(Back_Path,S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    }
    return 0;
}

//备份文件
void BackUpVG(){
    DIR *dir=opendir(Source_Path);
    struct dirent *ptr;
	//取当前时间戳
    time_t tt;
    time(&tt);
	//扫描文件列表找到VG开头的文件
    while((ptr=readdir(dir))!=NULL){
        if(strstr(ptr->d_name,"VG") || strstr(ptr->d_name,"EXT")){
            char cmd[256];
            sprintf(cmd,"cp %s/%s %s/%s_%d",Source_Path,ptr->d_name,Back_Path,ptr->d_name,tt);
            system(cmd);
        }
    }
    closedir(dir);
}
//分割字符
char **CharSPlit(const char *src,const char *delim){
    int size_char_p=sizeof(char *);
    char **result=(char **)malloc(size_char_p);
    char **ptmp=NULL;
    char *src_copy;
    int index=0;
    int result_size=1;
    int i=0;
    char *p=NULL;

    if(src==NULL||delim==NULL){
        *result=strdup("");
        return result;
    }

    src_copy=strdup(src);
    p=strtok(src_copy,delim);
    if(p==NULL||strlen(p)==0){
        free(src_copy);
        *result=strdup("");
        return result;
    }

    ptmp=(char **)realloc(result,(++result_size)*size_char_p);
    if(ptmp==NULL){
        free(result);
        return NULL;
    }
    result=ptmp;
    *result=strdup(p);
    index++;

    while((p=strtok(NULL,delim))){
        if(*p==""){
            continue;
        }
        ptmp=(char **)realloc(result,(++result_size)*size_char_p);
        if(ptmp==NULL){
            for(i=0;i<result_size;i++){
                free(result[i]);
            }
            free(result);
            return NULL;
        }
        result=ptmp;
        result[index]=strdup(p);
        index++;
    }
    free(src_copy);
    result[index]=strdup("");
    return result;
}
//分割字符串
char CharSPlitRow(const char *src,const char *delim){
    int size_char_p=sizeof(char *);
    char **result=(char **)malloc(size_char_p);
    char **ptmp=NULL;
    char *src_copy;
    int index=0;
    int result_size=1;
    int i=0;
    char *p=NULL;

    if(src==NULL||delim==NULL){
        *result=strdup("");
        return result;
    }

    src_copy=strdup(src);
    p=strtok(src_copy,delim);
    if(p==NULL||strlen(p)==0){
        free(src_copy);
        *result=strdup("");
        return result;
    }

    ptmp=(char **)realloc(result,(++result_size)*size_char_p);
    if(ptmp==NULL){
        free(result);
        return NULL;
    }
    result=ptmp;
    *result=strdup(p);
    index++;

    while((p=strtok(NULL,delim))){
        if(*p==""){
            continue;
        }
        ptmp=(char **)realloc(result,(++result_size)*size_char_p);
        if(ptmp==NULL){
            for(i=0;i<result_size;i++){
                free(result[i]);
            }
            free(result);
            return NULL;
        }
        result=ptmp;
        result[index]=strdup(p);
        index++;
    }
    free(src_copy);
    result[index]=strdup("");
    return index;
}
//分割字符串取得时间戳
int RecvStamp(char *str,char *des){
    char **res=NULL;
    char **p=NULL;
    char tmp[strlen(str)];
    long long result=0;
    res=CharSPlit(str,des);
    int row=CharSPlitRow(str,des);
    p=res;

    int i=1;
    for(i;i<=row;i++){
        if(i==row)
            sprintf(tmp,"%s",*p);
        free(*p);
        p++;
    }
    free(res);
    result=atoll(tmp);
}
//删除四天前的备份
void DelBackUp(){
    DIR *dir=opendir(Back_Path);
    struct dirent *ptr;

    time_t tt;
    time(&tt);

    while((ptr=readdir(dir))!=NULL){
        if(strstr(ptr->d_name,"VG")){
            int t=RecvStamp(ptr->d_name,"_");
            if(tt-t>345600){
                char delfile[128];
                sprintf(delfile,"%s/%s",Back_Path,ptr->d_name);
                remove(delfile);
            }
        }
    }
}

int main(void)
{
    if(CreatBack()==0){
        BackUpVG();
        DelBackUp();
    }
    return 0;
}

