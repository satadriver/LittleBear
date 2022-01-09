

#include <iostream>
#include "windows.h"

using namespace std;


class FileOperator{
	FileOperator (){};
	~FileOperator(){};

public:
	static int CheckPathExist(string path);
	static int CheckFileExist(string filename);
};

