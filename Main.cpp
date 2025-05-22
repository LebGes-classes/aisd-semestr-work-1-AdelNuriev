#include <array>
#include <string>
#include <random>
#include "TreapTree.h"
using namespace std;



int main()
{
    TreapTree treeSet = TreapTree();
    treeSet.insert(30);
    treeSet.insert(30);
    treeSet.insert(20);
    treeSet.insert(40);
    treeSet.insert(70);
    treeSet.insert(60);
    treeSet.insert(80);

    treeSet.print("", false, true);
	return 0;
}