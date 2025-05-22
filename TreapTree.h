#pragma once
#include <functional>
#include <random>
using namespace std;



class TreapTree
{
	struct Treap
	{
		// ключ и приоритет
		int data, priority;

		// два ребенка 0 - левый, 1 - правый
		array<Treap*, 2> kids;

		// параметры с суммой, количеством и ленивым распространением
		int subtreeSize, sum, toProp;

		/* Конструктор для параметра ключа
		* с рандомной генерацией приоритета с mt19937
		*/
		
		Treap(int data)
		{	
			kids = { NULL, NULL };
			this->data = data;
			this->priority = rand();
			this->subtreeSize = 0;
			this->sum = 0;
			this->toProp = 0;
		}
	};
	Treap* me;

private:
	int size(Treap* me);

	void recalc(Treap* me);

	void prop(Treap* me);

	Treap* rangeAdd(Treap* t, int l, int r, int toAdd);

	Treap* merge(Treap* l, Treap* r);

	array<Treap*, 2> split(Treap* me, int nInLeft);

	Treap* rightRotate(Treap* y);

	Treap* leftRotate(Treap* x);

	Treap* search(Treap* me, int data);

	Treap* insert(Treap *me, int data);

	Treap* deleteNode(Treap* me, int data);

	void print(const string& prefix, Treap* me, bool isLeft, bool isTreap);

public: 
	TreapTree()
	{
		this->me = nullptr;
	}

	void deleteNode(int data) {
		me = deleteNode(me, data);
	}

	void search(int data) {
		me = search(me, data);
	}

	void insert(int data) {
		me = insert(me, data);
	}

	void print(const string& prefix, bool isLeft, bool isTreap) {
		print(prefix, me, isLeft, isTreap);
	}
};