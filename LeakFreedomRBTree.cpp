// LeakFreedomRBTree.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <iomanip>
#include <string>

enum RBTColor { Black, Red };
std::string RBTColorS[] { "Black", "Red" };

template<class T>
struct  RBTNode
{
	std::shared_ptr<T> key;
	std::shared_ptr<RBTColor> color;
	//RBTNode<T> * parent;
	std::shared_ptr<RBTNode<T>*> parent;//Experemental code, needs extra recearch
	//RBTNode<T> * left;
	std::shared_ptr<RBTNode<T>*> left;//Experemental code, needs extra recearch
	//RBTNode<T> * right;
	std::shared_ptr<RBTNode<T>*> right;//Experemental code, needs extra recearch

	RBTNode(T k, RBTColor c, RBTNode *p, RBTNode *l, RBTNode *r) :
		//key(k), 
		key(std::shared_ptr<T>(new T {k})),
		//color(c),
		color(std::shared_ptr<RBTColor>(new RBTColor {c})),
		//parent(p), 
		parent(std::shared_ptr<RBTNode<T>*>(new RBTNode<T>*{ p })),//Experemental code, needs extra recearch
		//left(l), 
		left(std::shared_ptr<RBTNode<T>*>(new RBTNode<T>*{ l })),//Experemental code, needs extra recearch
		//right(r) { };
		right(std::shared_ptr<RBTNode<T>*>(new RBTNode<T>*{ r })) { };//Experemental code, needs extra recearch

	//Key
	T getKey() {
		return *key;
	}

	void setKey(T k) {//Possible dangerous method, try not use it
		*key = k;
	}

	//Color
	RBTColor getColor() {
		return *color;
	}

	void setColor(RBTColor c) {
		*color = c;
	}

	//Parent
	RBTNode<T>* getParent() {//Experemental code, needs extra recearch
		return *parent;
	}

	void setParent(RBTNode<T> *p) {
		*parent = p;
	}

	//Left
	RBTNode<T>* getLeft() {//Experemental code, needs extra recearch
		return *left;
	}

	void setLeft(RBTNode<T> *l) {
		*left = l;
	}

	//Right
	RBTNode<T>* getRight() {//Experemental code, needs extra recearch
		return *right;
	}

	void setRight(RBTNode<T> *r) {
			*right = r;
	}
};

template<class T>
class  RBTree
{
public:
	RBTree();
	~RBTree();

	void insert(T key);    // Вставляем узел, ключ это значение ключа, внешний интерфейс
	void remove(T key); // Удалить ключевой узел
	RBTNode<T>* search(T key); // Вывести инфу о ключевом узле
	void print(); // вывести информацию о всем дереве
	
private:
	RBTNode<T>* grandparent(RBTNode<T> *n);
	RBTNode<T>* uncle(RBTNode<T> *n);

	void insert_ni(RBTNode<T> *&root, T key);
	void print_ni(RBTNode<T> *n);

	void rotate_left(RBTNode<T> *n);
	void rotate_right(RBTNode<T> *n);

	void insert_case1(RBTNode<T> *n);
	void insert_case2(RBTNode<T> *n);
	void insert_case3(RBTNode<T> *n);
	void insert_case4(RBTNode<T> *n);
	void insert_case5(RBTNode<T> *n);

	RBTNode<T>* sibling(RBTNode<T> *n);

	void replace_node(RBTNode<T> *n, RBTNode<T> *child);
	void delete_one_child(RBTNode<T> *n);

	void delete_case1(RBTNode<T> *n);
	void delete_case2(RBTNode<T> *n);
	void delete_case3(RBTNode<T> *n);
	void delete_case4(RBTNode<T> *n);
	void delete_case5(RBTNode<T> *n);
	void delete_case6(RBTNode<T> *n);

	void destroy(RBTNode<T>* &node);

private:
	RBTNode<T> *root;
};

template<class T>
RBTree<T>::RBTree() {
	root = nullptr;
}

template<class T>
RBTree<T>::~RBTree()
{
	destroy(root);
}

//Интерфейсные методы

template<class T>
void RBTree<T>::insert(T key)
{
	insert_ni(root, key);	
}

template<class T>
void RBTree<T>::remove(T key)
{
	RBTNode<T>* delNode = search(key);
	if (delNode != nullptr) {
		if (delNode->getLeft() == nullptr && delNode->getRight() == nullptr) {
			// Нет ни одного потомка (все потомки нулевые)
			if (delNode == delNode->getParent()->getLeft()) {
				delNode->getParent()->setLeft(nullptr);
			}
			else {
				delNode->getParent()->setRight(nullptr);
			}

			delNode = nullptr;
			delete delNode;
		}

		else if (delNode->getLeft() != nullptr && delNode->getRight() != nullptr) {
			//Оба потомка не нулевые
			//Будем искать наибольший элемент в его левом поддереве
			RBTNode<T>* leftSubTreeMaxElem = delNode->getLeft();
			while (leftSubTreeMaxElem->getRight() != nullptr) {
				leftSubTreeMaxElem = leftSubTreeMaxElem->getRight();
			}

			delNode->setKey(leftSubTreeMaxElem->getKey());

			//Выполняется условие: n имеет не более одного ненулевого потомка.
			delete_one_child(leftSubTreeMaxElem);
		}

		else if (delNode->getLeft() != nullptr && delNode->getRight() == nullptr) {
			//Есть левый потомок, а правого нет
			//Будем искать наибольший элемент в его левом поддереве
			RBTNode<T>* leftSubTreeMaxElem = delNode->getLeft();
			while (leftSubTreeMaxElem->getRight() != nullptr) {
				leftSubTreeMaxElem = leftSubTreeMaxElem->getRight();
			}

			delNode->setKey(leftSubTreeMaxElem->getKey());

			//Выполняется условие: n имеет не более одного ненулевого потомка.
			delete_one_child(leftSubTreeMaxElem);
		}

		else {
			//Есть правый потомок, а левого нет
			//Будем искать наименьший элемент в его правом поддереве
			RBTNode<T>* rightSubTreeMinElem = delNode->getRight();
			while (rightSubTreeMinElem->getLeft() != nullptr) {
				rightSubTreeMinElem = rightSubTreeMinElem->getLeft();
			}

			delNode->setKey(rightSubTreeMinElem->getKey());

			//Выполняется условие: n имеет не более одного ненулевого потомка.
			delete_one_child(rightSubTreeMinElem);
		}
	}
	else {
		std::cout << "delNode already = nullptr" << std::endl;
		return;
	}
		
}

template<class T>
RBTNode<T>* RBTree<T>::search(T key)
{
	RBTNode<T>* n = root;

	while (n != nullptr) {
		if (key > n->getKey()) {
			n = n->getRight();
		}
		else if (key < n->getKey()) {
			n = n->getLeft();
		}
		else {
			break;
		}
	}

	return n;
}

template<class T>
void RBTree<T>::print()
{
	if (!root) {
		std::cout << "tree is empty" << std::endl;
	}
	else {
		print_ni(root);
	}
}

// Не интерфейсные (скрытые) методы

template<class T>
RBTNode<T>* RBTree<T>::grandparent(RBTNode<T>* n)
{
	if ((n != nullptr) && ((n->getParent()) != nullptr))
		return n->getParent()->getParent();
	else
		return nullptr;
}

template<class T>
RBTNode<T>* RBTree<T>::uncle(RBTNode<T>* n)
{
	RBTNode<T> *g = grandparent(n);
	if (g == nullptr)
		return nullptr; // No grandparent means no uncle
	if (n->getParent() == g->getLeft())
		return g->getRight();
	else
		return g->getLeft();
}

template<class T>
void RBTree<T>::insert_ni(RBTNode<T>*& root, T key)
{
	RBTNode<T> *pcur = root;
	RBTNode<T> *parent = nullptr;
	while (pcur != nullptr)
	{
		parent = pcur;
		if (key <= pcur->getKey())
		{
			pcur = pcur->getLeft();
		}
		else if (key > pcur->getKey())
		{
			pcur = pcur->getRight();
		}
		else
		{
			break;
		}
	}
	if (nullptr == pcur)
	{
		pcur = new RBTNode<T>(key, Red, nullptr, nullptr, nullptr);//new RBTNode<int>(1, Red, nullptr, nullptr, nullptr);
		pcur->setParent(parent);

		if (parent != nullptr)
		{
			if (pcur->getKey() > parent->getKey())
				parent->setRight(pcur);
			else
				parent->setLeft(pcur);
		}

		if (root == nullptr) {
			root = pcur;
			insert_case1(root);
		}
		else {
			insert_case1(pcur);
		}
	}
}

template<class T>
void RBTree<T>::print_ni(RBTNode<T>* n)
{
	if (n != nullptr) {
		if (n->getParent() == nullptr) {
			std::cout << n->getKey() << "(" << RBTColorS[n->getColor()] << ") is root" << std::endl;
		}
		else if (n->getParent()->getLeft() == n)
		{
			std::cout << n->getKey() << "(" << RBTColorS[n->getColor()] << ") is " << n->getParent()->getKey() << "'s " << "left child" << std::endl;
		}
		else
		{
			std::cout << n->getKey() << "(" << RBTColorS[n->getColor()] << ") is " << n->getParent()->getKey() << "'s " << "right child" << std::endl;
		}
		print_ni(n->getLeft());
		print_ni(n->getRight());
	}
}

template<class T>
void RBTree<T>::rotate_left(RBTNode<T>* n)
{
	RBTNode<T> *pivot = n->getRight();
	//pivot->parent = n->parent
	pivot->setParent(n->getParent()); // при этом, возможно, pivot становится корнем дерева 
	
	if (n->getParent() != nullptr) {
		if (n->getParent()->getLeft() == n)
			n->getParent()->setLeft(pivot);
		else
			n->getParent()->setRight(pivot);
	}

	//n->right = pivot->left;
	n->setRight(pivot->getLeft());
	if (pivot->getLeft() != nullptr)
		//pivot->left->parent = n;
		pivot->getLeft()->setParent(n);

	n->setParent(pivot);
	pivot->setLeft(n);
}

template<class T>
void RBTree<T>::rotate_right(RBTNode<T>* n)
{
	RBTNode<T> *pivot = n->getRight();
	//pivot->parent = n->parent;
	pivot->setParent(n->getParent()); // при этом, возможно, pivot становится корнем дерева 
	if (n->getParent() != nullptr) {
		if (n->getParent()->getLeft() == n)
			n->getParent()->setLeft(pivot);
		else
			n->getParent()->setRight(pivot);
	}

	n->setLeft(pivot->getRight());
	if (pivot->getRight() != nullptr)
		pivot->getRight()->setParent(n);

	n->setParent(pivot);
	pivot->setRight(n);
}

template<class T>
void RBTree<T>::insert_case1(RBTNode<T>* n)
{
	if (n->getParent() == nullptr)
		n->setColor(Black);
	else
		insert_case2(n);
}

template<class T>
void RBTree<T>::insert_case2(RBTNode<T>* n)
{
	if (n->getParent()->getColor() == Black)
		return; // Tree is still valid 
	else
		insert_case3(n);
}

template<class T>
void RBTree<T>::insert_case3(RBTNode<T>* n)
{
	RBTNode<T> *u = uncle(n), *g;

	if ((u != nullptr) && (u->getColor() == Red)) {
		// && (n->parent->color == RED) Второе условие проверяется в insert_case2, то есть родитель уже является красным.
		n->getParent()->setColor(Black);
		u->setColor(Black);
		g = grandparent(n);
		g->setColor(Red);
		insert_case1(g);
	}
	else {
		insert_case4(n);
	}
}

template<class T>
void RBTree<T>::insert_case4(RBTNode<T>* n)
{
	RBTNode<T> *g = grandparent(n);

	if ((n == n->getParent()->getRight()) && (n->getParent() == g->getLeft())) {
		rotate_left(n->getParent());

		/*
		 * rotate_left может быть выполнен следующим образом, учитывая что уже есть *g =  grandparent(n)
		 *
		 * struct node *saved_p=g->left, *saved_left_n=n->left;
		 * g->left=n;
		 * n->left=saved_p;
		 * saved_p->right=saved_left_n;
		 *
		 */

		n = n->getLeft();
	}
	else if ((n == n->getParent()->getLeft()) && (n->getParent() == g->getRight())) {
		rotate_right(n->getParent());

		/*
		 * rotate_right может быть выполнен следующим образом, учитывая что уже есть *g =  grandparent(n)
		 *
		 * struct node *saved_p=g->right, *saved_right_n=n->right;
		 * g->right=n;
		 * n->right=saved_p;
		 * saved_p->left=saved_right_n;
		 *
		 */

		n = n->getRight();
	}
	insert_case5(n);
}

template<class T>
void RBTree<T>::insert_case5(RBTNode<T>* n)
{
	RBTNode<T> *g = grandparent(n);

	n->getParent()->setColor(Black);
	g->setColor(Red);
	if ((n == n->getParent()->getLeft()) && (n->getParent() == g->getLeft())) {
		rotate_right(g);
	}
	else { /* (n == n->parent->right) && (n->parent == g->right) */
		rotate_left(g);
	}
}

template<class T>
RBTNode<T>* RBTree<T>::sibling(RBTNode<T>* n)
{
	if (n == n->getParent()->getLeft())
		return n->getParent()->getRight();
	else
		return n->getParent()->getLeft();
}

template<class T>
void RBTree<T>::replace_node(RBTNode<T>* n, RBTNode<T>* child)
{
	child->setParent(n->getParent());
	if (n == n->getParent()->getLeft()) {
		n->getParent()->setLeft(child);
	}
	else {
		n->getParent()->setRight(child);
	}
}

template<class T>
void RBTree<T>::delete_one_child(RBTNode<T>* n)
{
	/*
	 * Условие: n имеет не более одного ненулевого потомка.
	 */

	RBTNode<T>* child = nullptr;
	if (n->getLeft() != nullptr) {
		child = n->getLeft();
	}
	else if (n->getRight() != nullptr) {
		child = n->getRight();
	}
	else if(n->getLeft() == nullptr && n->getRight() == nullptr){
		if (n == n->getParent()->getLeft()) {
			n->getParent()->setLeft(nullptr);
		}
		else {
			n->getParent()->setRight(nullptr);
		}

		n = nullptr;
		delete n;
	}
		
	if(child != nullptr){
		replace_node(n, child);
		if (n->getColor() == Black) {
			if (child->getColor() == Red)
				child->setColor(Black);
			else
				delete_case1(child);
		}
		if (n == n->getParent()->getLeft()) {
			n->getParent()->setLeft(nullptr);
		}
		else {
			n->getParent()->setRight(nullptr);
		}

		n = nullptr;
		delete n;
	}
}

template<class T>
void RBTree<T>::delete_case1(RBTNode<T>* n)
{
	if (n->getParent() != nullptr)
		delete_case2(n);
}

template<class T>
void RBTree<T>::delete_case2(RBTNode<T>* n)
{
	RBTNode<T> *s = sibling(n);

	if (s->getColor() == Red) {
		n->getParent()->setColor(Red);
		s->setColor(Black);
		if (n == n->getParent()->getLeft())
			rotate_left(n->getParent());
		else
			rotate_right(n->getParent());
	}
	delete_case3(n);
}

template<class T>
void RBTree<T>::delete_case3(RBTNode<T>* n)
{
	RBTNode<T> *s = sibling(n);

	if ((n->getParent()->getColor() == Black) &&
		(s->getColor() == Black) &&
		(s->getLeft()->getColor() == Black) &&
		(s->getRight()->getColor() == Black)) {
		s->setColor(Red);
		delete_case1(n->getParent());
	}
	else
		delete_case4(n);
}

template<class T>
void RBTree<T>::delete_case4(RBTNode<T>* n)
{
	RBTNode<T> *s = sibling(n);

	if ((n->getParent()->getColor() == Red) &&
		(s->getColor() == Black) &&//
		(s->getLeft()->getColor() == Black) &&
		(s->getRight()->getColor() == Black)) {
		s->setColor(Red);
		n->getParent()->setColor(Black);
	}
	else
		delete_case5(n);
}

template<class T>
void RBTree<T>::delete_case5(RBTNode<T>* n)
{
	RBTNode<T> *s = sibling(n);

	if (s->getColor() == Black) { /* this if statement is trivial,
	due to case 2 (even though case 2 changed the sibling to a sibling's child,
	the sibling's child can't be red, since no red parent can have a red child). */
	/* the following statements just force the red to be on the left of the left of the parent,
    or right of the right, so case six will rotate correctly. */
		if ((n == n->getParent()->getLeft()) &&
			(s->getRight()->getColor() == Black) &&
			(s->getLeft()->getColor() == Red)) { /* this last test is trivial too due to cases 2-4. */
			s->setColor(Red);
			s->getLeft()->setColor(Black);
			rotate_right(s);
		}
		else if ((n == n->getParent()->getRight()) &&
			(s->getLeft()->getColor() == Black) &&
			(s->getRight()->getColor() == Red)) {/* this last test is trivial too due to cases 2-4. */
			s->setColor(Red);
			s->getRight()->setColor(Black);
			rotate_left(s);
		}
	}
	delete_case6(n);
}

template<class T>
void RBTree<T>::delete_case6(RBTNode<T>* n)
{
	RBTNode<T> *s = sibling(n);

	s->setColor(n->getParent()->getColor());
	n->getParent()->setColor(Black);

	if (n == n->getParent()->getLeft()) {
		s->getRight()->setColor(Black);
		rotate_left(n->getParent());
	}
	else {
		s->getLeft()->setColor(Black);
		rotate_right(n->getParent());
	}
}

template<class T>
void RBTree<T>::destroy(RBTNode<T>* &node)
{
	if (node == nullptr) {
		return;
	}

	destroy(node->getLeft());
	destroy(node->getRight());

	delete node;
	node = nullptr;
}


int main()
{
    std::cout << "Hello World!\n";

	RBTree<int> *rbt = new RBTree<int>();

	rbt->insert(13);
	rbt->insert(8);
	rbt->insert(17);
	rbt->insert(1);
	rbt->insert(11);
	rbt->insert(15);
	rbt->insert(25);
	rbt->insert(6);
	rbt->insert(22);
	rbt->insert(27);
	rbt->insert(21);

	rbt->print();
	std::cout << std::endl;

	//rbt->remove(1);//Also tested 6, 8, 11, 13, 17, 15, 25, 22, 27, 21
	//rbt->print();

	rbt->remove(17);
	std::cout << "del 17" << std::endl;
	rbt->print();
	std::cout << std::endl;

	rbt->remove(13);
	std::cout << "del 13" << std::endl;
	rbt->print();
	std::cout << std::endl;

	rbt->remove(1);
	std::cout << "del 1" << std::endl;
	rbt->print();
	std::cout << std::endl;

	rbt->remove(22);
	std::cout << "del 22" << std::endl;
	rbt->print();
	std::cout << std::endl;

	rbt->remove(26);
	std::cout << "del 26" << std::endl;
	rbt->print();
	std::cout << std::endl;

	rbt->remove(15);
	std::cout << "del 15" << std::endl;
	rbt->print();
	std::cout << std::endl;

	rbt->remove(27);
	std::cout << "del 27" << std::endl;
	rbt->print();
	std::cout << std::endl;

	rbt->remove(22);
	std::cout << "del 22" << std::endl;
	rbt->print();
	std::cout << std::endl;

	rbt->remove(27);
	std::cout << "del 27" << std::endl;
	rbt->print();
	std::cout << std::endl;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.


