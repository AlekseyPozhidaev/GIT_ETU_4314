#include<iostream>
#include<random>
#include<string>
#include<vector>
using namespace std;

typedef long long ll;

int get_rand_int() {
	static mt19937 gen{ random_device{}() };
	static uniform_int_distribution<int> pick{ -1000000, 10000000 };
	return pick(gen);
}

struct Tree {
	ll value;
	int priority;
	Tree* left = nullptr;
	Tree* right = nullptr;

	Tree() {
		value = INT32_MIN;
		priority = INT32_MIN;
	}

	explicit Tree(ll x0) {
		value = x0;
		priority = get_rand_int();
	}

};
//делим по значению 
//(амперсанд для прямых измемнений)
void split(Tree* tree, Tree*& left, Tree*& right, ll x0) {
	if (tree == nullptr) {
		left = nullptr;
		right = nullptr;
		return;
	}
	if (tree->value < x0) {
		split(tree->right, tree->right, right, x0);
		left = tree;
	}
	else {
		split(tree->left, left, tree->left, x0);
		right = tree;
	}
}
//сливаем по ключу 
void merge(Tree*& tree, Tree* left, Tree* right) {
	if (left == nullptr) {
		tree = right;
		return;
	}
	if (right == nullptr) {
		tree = left;
		return;
	}
	if (left->priority > right->priority) {
		merge(left->right, left->right, right);
		tree = left;
	}
	else {
		merge(right->left, left, right->left);
		tree = right;
	}
}

void insert(Tree*& tree, Tree* x) {
	if (x == nullptr) return;
	if (tree == nullptr) {
		tree = x;
		return;
	}

	if (tree->priority <= x->priority) {
		split(tree, x->left, x->right, x->value);
		tree = x;
		return;
	}

	if (tree->value > x->value) {
		insert(tree->left, x);
		return;
	}
	else {
		insert(tree->right, x);
		return;
	}

}

void remove(Tree*& tree, ll value) {
	if (tree == nullptr) {
		return;
	}
	if (tree->value < value) {
		remove(tree->right, value);
	}
	else {
		if (tree->value > value) {
			remove(tree->left, value);
		}
		else {
			merge(tree, tree->left, tree->right);
		}
	}
}

bool exists(Tree*& tree, ll value) {
	if (tree == nullptr) {
		return false;
	}
	if (tree->value == value) {
		return true;
	}
	if (tree->value > value) {
		return exists(tree->left, value);
	}
	else {
		return exists(tree->right, value);
	}
}
//одна операция в разные стороны
Tree* near(Tree*& tree, ll value, bool down) {
	Tree* now = tree;
	Tree* ans = nullptr;

	while (now != nullptr) {
		switch (down) {
			case true:
				if (now->value < value) {
					ans = now;
					now = now->right;
					continue;
				}
				else {
					now = now->left;
					continue;
				}
			case false: {
					if (now->value > value) {
						ans = now;
						now = now->left;
						continue;
					}
					else {
						now = now->right;
						continue;
					}
				}
		}
	}

	return ans;
}

Tree* prev(Tree*& tree, ll value) {
	return near(tree, value, 1);
}
Tree* next(Tree*& tree, ll value) {
	return near(tree, value, 0);
}
// Вспомогательная функция для получения высоты дерева
int getHeight(Tree* tree) {
	if (tree == nullptr) return 0;
	return 1 + max(getHeight(tree->left), getHeight(tree->right));
}

// Вспомогательная функция для заполнения матрицы представления дерева
void fillTreeMatrix(Tree* tree, vector<vector<string>>& matrix, int level, int left, int right) {
	if (tree == nullptr) return;

	int mid = (left + right) / 2;
	matrix[level][mid] = to_string(tree->value);

	if (tree->left != nullptr) {
		fillTreeMatrix(tree->left, matrix, level + 1, left, mid - 1);
	}
	if (tree->right != nullptr) {
		fillTreeMatrix(tree->right, matrix, level + 1, mid + 1, right);
	}
}

// Функция для красивого вывода дерева в консоль
void printTree(Tree* tree) {
	if (tree == nullptr) {
		cout << "Tree is empty!" << endl;
		return;
	}

	int height = getHeight(tree);
	int width = (1 << height) - 1; // 2^height - 1

	vector<vector<string>> matrix(height, vector<string>(width, " "));
	fillTreeMatrix(tree, matrix, 0, 0, width - 1);

	cout << "Tree structure:" << endl;
	cout << string(width * 3, '-') << endl;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			cout << matrix[i][j];
			if (j < width - 1) cout << "  ";
		}
		cout << endl;
	}
	cout << string(width * 3, '-') << endl;
}

// Простой вывод в порядке in-order (для проверки)
void printInOrder(Tree* tree) {
	if (tree == nullptr) return;
	printInOrder(tree->left);
	cout << tree->value << " ";
	printInOrder(tree->right);
}

// Простой вывод в виде скобочной структуры
void printTreeBracket(Tree* tree) {
	if (tree == nullptr) return;
	cout << "(" << tree->value;
	if (tree->left != nullptr || tree->right != nullptr) {
		cout << " ";
		printTreeBracket(tree->left);
		cout << " ";
		printTreeBracket(tree->right);
	}
	cout << ")";
}
int countNodesWithAtMostTwoChildren(Tree* tree) {
	if (tree == nullptr) {
		return 0;
	}

	int count = 0;

	// Подсчитываем количество потомков у текущей вершины
	int childrenCount = 0;
	if (tree->left != nullptr) childrenCount++;
	if (tree->right != nullptr) childrenCount++;

	// Если потомков не более двух, увеличиваем счетчик
	if (childrenCount <= 2) {
		count = 1;
	}

	// Рекурсивно обходим левое и правое поддеревья
	count += countNodesWithAtMostTwoChildren(tree->left);
	count += countNodesWithAtMostTwoChildren(tree->right);

	return count;
}
int main() {
	string s;
	ll value;
	Tree* tree = nullptr;
	for (int i = 0; i < 10; i++) {
		value = get_rand_int() % 40;
		Tree* x = new Tree(value);
		if (!exists(tree, value)) {
			insert(tree, x);
		}
	}
	while (cin >> s >> value) {
		switch (s[0]) {
			case 'i': {
					Tree* x = new Tree(value);
					if (!exists(tree, value)) {
						insert(tree, x);
					}
					continue;
				}
			case 'e': {
					if (exists(tree, value)) {
						cout << "true";
					}
					else {
						cout << "false";
					}
					cout << "\n";
					continue;
				}
			case 'd': {
					if (exists(tree, value)) {
						remove(tree, value);
					}
					continue;
				}
			case 'n': {
					Tree* x;
					x = next(tree, value);
					if (x == nullptr) {
						cout << "none";
					}
					else {
						cout << x->value;
					}
					cout << "\n";
					continue;
				}
			case 'p': {
					Tree* x;
					x = prev(tree, value);
					if (x == nullptr) {
						cout << "none";
					}
					else {
						cout << x->value;
					}
					cout << "\n";
					continue;
				}
			case 'P': { // Для команды "print"
					cout << "Tree visualization:" << endl;
					printTree(tree);
					cout << "In-order traversal: ";
					printInOrder(tree);
					cout << endl;
					cout << "Bracket notation: ";
					printTreeBracket(tree);
					cout << endl;
					continue;
				}
			case 'c': { // Для команды "count"
					int count = countNodesWithAtMostTwoChildren(tree);
					cout << "Number of nodes with at most two children: " << count << endl;
					continue;
				}
		}
	}

	return 0;
}