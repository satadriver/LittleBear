#pragma once
#include <windows.h>


/*
在IMAGE_DATA_DIRECTORY中，有几项的名字都和导入表有关系，其中包括：
IMAGE_DIRECTORY_ENTRY_IMPORT，
IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT，
IMAGE_DIRECTORY_ENTRY_IAT和
IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT


IMAGE_DIRECTORY_ENTRY_IMPORT就是我们通常所知道的导入表，在PE文件加载时，会根据这个表里的内容加载依赖的DLL，并填充所需函数的地址。
IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT叫做绑定导入表，在第一种导入表导入地址的修正是在PE加载时完成，
如果一个PE文件导入的DLL或者函数多那么加载起来就会略显的慢一些，所以出现了绑定导入，在加载以前就修正了导入表，这样就会快一些。
IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT叫做延迟导入表，一个PE文件也许提供了很多功能，也导入了很多其他DLL，
但是并非每次加载都会用到它提供的所有功能，也不一定会用到它需要导入的所有DLL，因此延迟导入就出现了，
只有在一个PE文件真正用到需要的DLL，这个DLL才会被加载，甚至于只有真正使用某个导入函数，这个函数地址才会被修正。
IMAGE_DIRECTORY_ENTRY_IAT是导入地址表，前面的三个表其实是导入函数的描述，真正的函数地址是被填充在导入地址表中的。
*/

class ImportFunTable {
public:
	static DWORD recover(DWORD module);


};