#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <deque>
#include <locale.h>
#include <math.h>

using namespace std;

struct Node {
	Node *left, *right;
	int key;
	int height;
	Node(int k) : left(NULL), right(NULL), key(k), height(0){ }
};

// Print Tree 
struct TreePrinter {
	// Find the maximum height of the binary tree
	int maxHeight(Node *p) {
		if (!p) return 0;
		int leftHeight = maxHeight(p->left);
		int rightHeight = maxHeight(p->right);
		return (leftHeight > rightHeight) ? leftHeight + 1 : rightHeight + 1;
	}

	// Convert an integer value to string
	string intToString(int val) {
		ostringstream ss;
		ss << val;
		return ss.str();
	}

	// Print the arm branches (eg, /    \ ) on a line
	void printBranches(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const deque<Node*>& nodesQueue, ostream& out) {
		deque<Node*>::const_iterator iter = nodesQueue.begin();
		for (int i = 0; i < nodesInThisLevel / 2; i++) {
			out << ((i == 0) ? setw(startLen - 1) : setw(nodeSpaceLen - 2)) << "" << ((*iter++) ? "/" : " ");
			out << setw(2 * branchLen + 2) << "" << ((*iter++) ? "\\" : " ");
		}
		out << endl;
	}

	// Print the branches and node (eg, ___10___ )
	void printNodes(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const deque<Node*>& nodesQueue, ostream& out) {
		deque<Node*>::const_iterator iter = nodesQueue.begin();
		for (int i = 0; i < nodesInThisLevel; i++, iter++) {
			out << ((i == 0) ? setw(startLen) : setw(nodeSpaceLen)) << "" << ((*iter && (*iter)->left) ? setfill('_') : setfill(' '));
			out << setw(branchLen + 2) << ((*iter) ? intToString((*iter)->key) : "");
			out << ((*iter && (*iter)->right) ? setfill('_') : setfill(' ')) << setw(branchLen) << "" << setfill(' ');
		}
		out << endl;
	}

	// Print the leaves only (just for the bottom row)
	void printLeaves(int indentSpace, int level, int nodesInThisLevel, const deque<Node*>& nodesQueue, ostream& out) {
		deque<Node*>::const_iterator iter = nodesQueue.begin();
		for (int i = 0; i < nodesInThisLevel; i++, iter++) {
			out << ((i == 0) ? setw(indentSpace + 2) : setw(2 * level + 2)) << ((*iter) ? intToString((*iter)->key) : "");
		}
		out << endl;
	}

	// Pretty formatting of a binary tree to the output stream
	// @ param
	// level  Control how wide you want the tree to sparse (eg, level 1 has the minimum space between nodes, while level 2 has a larger space between nodes)
	// indentSpace  Change this to add some indent space to the left (eg, indentSpace of 0 means the lowest level of the left node will stick to the left margin)
	void printPretty(Node *root, int level, int indentSpace, ostream& out) {
		int h = maxHeight(root);
		int nodesInThisLevel = 1;

		int branchLen = 2 * ((int)pow(2.0, h) - 1) - (3 - level)*(int)pow(2.0, h - 1);  // eq of the length of branch for each node of each level
		int nodeSpaceLen = 2 + (level + 1)*(int)pow(2.0, h);  // distance between left neighbor node's right arm and right neighbor node's left arm
		int startLen = branchLen + (3 - level) + indentSpace;  // starting space to the first node to print of each level (for the left most node of each level only)

		deque<Node*> nodesQueue;
		nodesQueue.push_back(root);
		for (int r = 1; r < h; r++) {
			printBranches(branchLen, nodeSpaceLen, startLen, nodesInThisLevel, nodesQueue, out);
			branchLen = branchLen / 2 - 1;
			nodeSpaceLen = nodeSpaceLen / 2 + 1;
			startLen = branchLen + (3 - level) + indentSpace;
			printNodes(branchLen, nodeSpaceLen, startLen, nodesInThisLevel, nodesQueue, out);

			for (int i = 0; i < nodesInThisLevel; i++) {
				Node *currNode = nodesQueue.front();
				nodesQueue.pop_front();
				if (currNode) {
					nodesQueue.push_back(currNode->left);
					nodesQueue.push_back(currNode->right);
				}
				else {
					nodesQueue.push_back(NULL);
					nodesQueue.push_back(NULL);
				}
			}
			nodesInThisLevel *= 2;
		}
		printBranches(branchLen, nodeSpaceLen, startLen, nodesInThisLevel, nodesQueue, out);
		printLeaves(indentSpace, level, nodesInThisLevel, nodesQueue, out);
	}
};

struct Tree {
	Node *root;
	Tree() : root(NULL) { }
	TreePrinter TP;

	void fill(int s) {
		int v;
		for (int i = 0; i < s; i++) {
			v = rand() % 10;
			addNode(root, v);
		}
	}

	// Добавляем новый узел
	Node* addNode(Node* n, int k) {
		if (!n) return new Node(k);
		if (k < n->key)
			n->left = addNode(n->left, k);
		else if (k == n->key)
			cout << "Такой элемент уже есть." << endl;
		else if (k > n->key)
			n->right = addNode(n->right, k);
		return balance(n);
	}

	// Удаление ключа k из дерева n
	Node* removeNode(int k, Node* n) {
		if (!n) {
			cout << "Такого элемента нет." << endl;
			return NULL;
		}
		if (k < n->key)
			n->left = removeNode(k, n->left);
		else if (k > n->key)
			n->right = removeNode(k, n->right);
		else {
			Node* q = n->left;
			Node* r = n->right;
			cout << "Узел " << n->key << " удален." << endl;
			delete n;
			if (!r) return q;
			Node* min = findMinNode(r);
			min->right = removeMinNode(r);
			min->left = q;
			return balance(min);
		}
		return balance(n);
	}

	// Ищем узел c ключом k и возвращаем кол-во шагов
	int findNode(int k, Node* n) {
		if (!n) return -100;
		if (n->key == k)
			return 1;
		else if (k < n->key)
			return findNode(k, n->left) + 1;
		else
			return findNode(k, n->right) + 1;
	}

	// Выводим дерево
	void printTree() {
		ofstream fout("Tree.txt");
		if (TP.maxHeight(root) < 6) {
			cout << "Дерево:" << endl;
			TP.printPretty(root, 1, 0, cout);
		}
		else {
			cout << "Дерево не поместилось в консоли и сохранено в файле." << endl;
		}
		TP.printPretty(root, 1, 0, fout);
	}

	// Возвращаем значение самого правого листа
	int mostRightKey(Node* n) {
		return (n->right) ? mostRightKey(n->right) : n->key;
	}

private:
	// Получаем высоту дерева n
	int height(Node *n) {
		return n ? n->height : 0;
	}

	// Получаем разницу в высотах дерева n
	int balanceFactor(Node *n) {
		if (!n) return 0;
		else
			return height(n->left) - height(n->right);
	}

	// Исправляем высоту дерева n
	void fixHeight(Node *n) {
		int hl = height(n->left);
		int hr = height(n->right);
		if (!n->left && !n->right) {
			n->height = 0;
		}
		else {
			n->height = (hl > hr ? hl : hr) + 1;
		}
	}

	// Малое правое вращение вокруг узла n
	Node* smallRightRotation(Node* n) {
		Node* q = n->left;
		n->left = q->right;
		q->right = n;
		fixHeight(n);
		fixHeight(q);
		return q;
	}

	// Малое левое вращение
	Node* smallLeftRotation(Node* n) {
		Node* q = n->right;
		n->right = q->left;
		q->left = n;
		fixHeight(n);
		fixHeight(q);
		return q;
	}

	// Балансируем дерево с корнем n и балансом 2.
	// Достаточно выполнить либо простой поворот влево вокруг n,
	// либо большое вращение влево вокруг того же n
	// Для балансировки дерева с корнем n и балансом -2 - симметричные действия
	Node* balance(Node* n) {
		fixHeight(n);
		// Большое правое вращение
		if (balanceFactor(n) == 2) {
			if (balanceFactor(n->right) <= 0) {
				n->left = smallLeftRotation(n->left);
			}
			return smallRightRotation(n);
		}
		// Большое левое вращение
		else if (balanceFactor(n) == -2) {
			if (balanceFactor(n->right) > 0) {
				n->right = smallRightRotation(n->right);
			}
			return smallLeftRotation(n);
		}
		// Балансировка не нужна
		else {
			return n;
		}
	}

	// Поиск узла с минимальным ключом в дереве n
	Node* findMinNode(Node* n) {
		return n->left ? findMinNode(n->left) : n;
	}

	// Удаление узла с минимальным ключом в дереве n
	Node* removeMinNode(Node* n) {
		if (!n->left)
			return n->right;
		n->left = removeMinNode(n->left);
		return balance(n);
	}
};

int getCommand();

int main() {
	setlocale(LC_ALL, "rus");

	srand(static_cast<unsigned int>(time(NULL)));
	Tree T;
	TreePrinter TP;

	for (int i = 1; i < 21; i++) {
		T.root = T.addNode(T.root, i);
	}

	int cmd = 0;
	do {
		cmd = getCommand();
		int k;
		int steps = 0;
		switch (cmd) {
		case 1:
			cout << "Введи ключ: ";
			cin >> k;
			T.root = T.addNode(T.root, k);
			T.printTree();
			cout << endl;
			break;
		case 2:
			if (T.root) {
				cout << "Введи ключ для удаления: ";
				cin >> k;
				T.root = T.removeNode(k, T.root);
				T.printTree();
			}
			else
				cout << "Дерево пустое." << endl;
			cout << endl;
			break;
		case 3:
			if (T.root) {
				cout << "Введи ключ для поиска: ";
				cin >> k;
				cout << "Элемент " << k;
				steps = T.findNode(k, T.root);
				steps < 0 ? cout << " не найден." << endl : cout << " найден за " << steps << " шагов" << endl;
			}
			else
				cout << "Дерево пустое." << endl;
			cout << endl;
			break;
		case 4:
			if (T.root) {
				cout << "Значение самого правого листа: " << T.mostRightKey(T.root) << endl;
			}
			else
				cout << "Дерево пустое." << endl;
			cout << endl;
			break;
		case 5:
			if (T.root)
				T.printTree();
			else
				cout << "Дерево пустое." << endl;
			cout << endl;
			break;
		case 0:
			cout << "Пока!" << endl;
			cout << endl;
			break;
		default:
			cerr << "Uknown error" << endl;
			cout << endl;
			break;
		}
	} while (cmd);
	return 0;
}

int getCommand() {
	cout << "1. Добавить элемент." << endl;
	cout << "2. Удалить элемент." << endl;
	cout << "3. Найти элемент." << endl;
	cout << "4. Вывести значение самого правого листа." << endl; // с кол-вом шагов
	cout << "5. Распечатать дерево." << endl;
	cout << "0. Выход." << endl;
	int cmd;
	cin >> cmd;

	while (!cin.good() && cmd < 0 && cmd > 5) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cerr << "Неверная команда! Повторите ввод: ";
		cin >> cmd;
	}

	return cmd;
}