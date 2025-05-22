using namespace std;
#include <iostream>
#include <array>
#include <string>
#include "TreapTree.h"


// инициализация размера, засчет детей
int TreapTree::size(Treap* me)
{
	return me == NULL ? 0 : me->subtreeSize;
}

// высчитывание размера и суммы
void TreapTree::recalc(Treap* me)
{
	if (me == NULL) return;
	// раннее обозначение размера, как размер самого узла, то биш 1
	me->subtreeSize = 1;
	// раннее обозначение суммы, как значение самого узла
	me->sum = me->data + me->toProp * size(me);
	//размер и сумма считаются как сумма размеров и сумм двух детей, как размер под деревьев
	for (Treap* t : me->kids) if (t != NULL) me->subtreeSize += t->subtreeSize;
	for (Treap* t : me->kids) if (t != NULL) me->sum += t->sum + t->toProp * size(t);
}

// ленивое распространения
void TreapTree::prop(Treap* me)
{
	//если значение равно нулю или не существует, прерыть работу
	if (me == NULL) return;
	if (me->toProp == 0) return;

	//в противном случае нужно распростанить это значение на два дочерних
	for (Treap* t : me->kids) if (t != NULL) t->toProp += me->toProp;
	me->data += me->toProp;

	//значение распространения установим как ноль, ведь мы уже учли его
	me->toProp = 0;

	//нужно пересчитать все значения
	//потому что есть вероятность, что значение детей могли поменяться
	recalc(me);
}


//метод для объединения двух Treap
TreapTree::Treap* TreapTree::merge(Treap* l, Treap* r)
{
	//если на входе левой ветке ничего нет, то возвращаем правую ветку
	if (l == NULL) return r;
	//аналогично, если на входе правой ветке ничего нет, то возвращаем левую ветку
	if (r == NULL) return l;

	prop(l); prop(r);
	/* Правило слияния:
	*
	* Распределяем по приоритету
	* Взять узел с наименьшим приоритетом
	* В зависимости от стороны
	* Дерево должно распределиться, как в сегментарном дереве
	*
	*/
	if (l->priority < r->priority)
	{
		l->kids[1] = merge(l->kids[1], r);
		recalc(l);
		return l;
	}
	else
	{
		r->kids[0] = merge(l, r->kids[0]);
		recalc(r);
		return r;
	}
}

/*
*метод для разбиения Treap
*по количеству слева элементов с массиве, составленном из дерева
*/
array<TreapTree::Treap*, 2> TreapTree::split(Treap* me, int nInLeft)
{
	//если узла нет, значит обе половины пусты
	if (me == NULL) return { NULL, NULL };

	//сохраняем в ленивой обработке
	prop(me);

	/* Правило разбиения:
	*
	* Нужно узнать, какую из сторон собирается разделиться
	* Соединяем дочернее под дерево с крайним элементом
	* Делаем перерасчет
	* Возвращаем обе части, возникшие при делении
	*
	*/
	if (size(me->kids[0]) >= nInLeft) //заняли левую сторону, значит крайний узел будет справа
	{
		//вызываем функцию split 
		array<Treap*, 2> leftRes = split(me->kids[0], nInLeft);
		me->kids[0] = leftRes[1];
		recalc(me);
		return { leftRes[0], me };
	}
	else //заняли правую сторону, значит крайний узел будет слева
	{
		nInLeft = nInLeft - size(me->kids[0]) - 1;
		array<Treap*, 2> rightRes = split(me->kids[1], nInLeft);
		me->kids[1] = rightRes[0];
		recalc(me);
		return { me, rightRes[1] };
	}
	return { NULL, NULL };
}

//метод для работы в диапазоне массива, составленном из дерева
TreapTree::Treap* TreapTree::rangeAdd(TreapTree::Treap* t, int l, int r, int toAdd)
{
	array<Treap*, 2> a = split(t, l), b = split(a[1], r - l + 1);
	b[0]->toProp += toAdd;
	return merge(a[0], merge(b[0], b[1]));
}

	// меняет ротацию правой ветки
TreapTree::Treap* TreapTree::rightRotate(TreapTree::Treap* y)
{
	Treap* x = y->kids[0], * T2 = x->kids[1];

	// меняем ротацию
	x->kids[1] = y;
	y->kids[0] = T2;

	// возвращаем новый узел
	return x;
}

	// меняет ротацию левой ветки
TreapTree::Treap* TreapTree::leftRotate(TreapTree::Treap* x)
{
	Treap* y = x->kids[1], * T2 = y->kids[0];

	// меняем ротацию
	y->kids[0] = x;
	x->kids[1] = T2;

	// возвращаем новый узел
	return y;
}

	//метод для поиска
TreapTree::Treap* TreapTree::search(Treap* me, int data)
{
	//Если узлов нет, то находить не имеет смысла
	if (me == NULL) return NULL;

	//Если уже находимся на нужном узле возвращаем этот узел
	if (me->data == data) return me;

	//Если значение поиска превосходит значение на узле, то идем по правой ветке
	if (me->data < data) return search(me->kids[1], data);

	//Если значение поиска меньше значения на узле, то идем по левой ветке
	return search(me->kids[0], data);
}

	//метод для вставки
TreapTree::Treap* TreapTree::insert(TreapTree::Treap* me, int data)
{
	//Если узлов нет, то создаем новый
	if (me == NULL) return new Treap(data);

	//Если значение меньше, чем значение на узле
	if (me->data >= data)
	{
		//идем дальше по левой ветке
		me->kids[0] = insert(me->kids[0], data);

		//меняем ротацию, если это необходимо
		if (me->kids[0]->priority < me->priority)
			me = rightRotate(me);

	}
	else //Если значение превосходит значение на узле
	{
		//идем дальше по правой ветке
		me->kids[1] = insert(me->kids[1], data);
		
		//меняем ротацию, если это необходимо
		if (me->kids[1]->priority < me->priority)
			me = leftRotate(me);
	}
 //делаем перерасчет
	return me;
}

//удаление узла по значению
TreapTree::Treap* TreapTree::deleteNode(TreapTree::Treap* me, int data)
{
	if (me == NULL) return NULL;

	/*
	аналогично предыдущим методам
	если меньше ходим влево
	если больше ходим вправо
	ходим до тех пор, пока не встретим нужный узел
	*/
	if (me->data > data)
		me->kids[0] = deleteNode(me->kids[0], data);
	else if (me->data < data)
		me->kids[1] = deleteNode(me->kids[1], data);

	/*Если нашелся нужный узел
	Тут ситуация разбивается еще на 4 случая
	1 - Если слева нет узла: переход идет к правому ребенку, происходит свап с удалением
	2 - Если справа нет узла: переход идет к левому ребенку, происходит свап с удалением
	3 - Если оба ребенка существуют
		а: правый ребенок имеет больший приоритет: меняем положения
		б: левый ребенок имеет больший приоритет: меняем положения
	*/

	//случай слева нет узла
	else if (me->kids[0] == NULL)
	{
		Treap* temp = me->kids[1];
		delete(me);
		me = temp;  //устанавливаем на место узла правого ребенка
	}

	// случай справа нет узла
	else if (me->kids[1] == NULL)
	{
		Treap* temp = me->kids[0];
		delete(me);
		me = temp;  //устанавливаем на место узла левого ребенка
	}

	//случай оба не пусты "а"
	else if (me->kids[0]->priority > me->kids[1]->priority)
	{
		me = leftRotate(me);
		me->kids[0] = deleteNode(me->kids[0], data);
	}
	else //случай оба не пусты "б"
	{
		me = rightRotate(me);
		me->kids[1] = deleteNode(me->kids[1], data);
	}
	return me;
}

	/*метод для распечатки дерева
	*
	* ─────7
		  L├────4
		   │   L├────2
		  R└───11
			  R└───13

	*/
void TreapTree::print(const string& prefix, Treap* me, bool is_left, bool root) {
	if (me == NULL) return;

	if (root)
	{
		cout << "─────";
	}
	else
	{
		cout << prefix << (is_left ? "L├────" : "R└───");
	}

	cout << "(" << me->data << " " << me->priority << ")" << endl;


	print(prefix + (is_left ? " │   " : "    "), me->kids[0], true, false);
	print(prefix + (is_left ? " │   " : "    "), me->kids[1], false, false);
};