#ifndef __FILES_TOOL_H
#define __FILES_TOOL_H

#include <iostream>
#include <string>
#include <vector>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <fstream>

using namespace std;

void getFilesAllName(string path, vector<string>& files);//��ȡĳ����·���������ļ�����������·��
void getAllFiles(string path, vector<string>& files);//��ȡĳ����·���������ļ������ļ����ƣ���������·��
void getJustCurrentDir(string path, vector<string>& files);//ֻ��ȡĳ����·���µĵ�ǰ�ļ�����
void getJustCurrentFile(string path, vector<string>& files);//ֻ��ȡĳ����·���µĵ�ǰ�ļ���
void getFilesAll(string path, vector<string>& files);//ֻ��ȡĳ����·���µ������ļ���(��������ǰĿ¼����Ŀ¼���ļ�)

#endif