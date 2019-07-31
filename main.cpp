#include <iostream>
#include <vector>
#include <algorithm>
#include <string.h>

using namespace std;

class NODE;

class BTree
{
private:
	bool is_empty;
	NODE *root;
        char (*compare_keys)(void *, void *); //-1 if left>right, 0 if left=right, 1 if left<right
	void (*delete_key)(void *);
	void (*delete_value)(void *);
	NODE *get_node(void *key);
	void delete_nodes(NODE *cur);
	void print_nodes(NODE *cur);
	friend class NODE;
public:
	int remove_node(void *key);
	int insert_node(void *key, void *value);
	BTree(char (*compare_keys)(void *, void *), void (*delete_key)(void *), void (*delete_value)(void *));
	BTree(vector<NODE> nodes, char (*compare_keys)(void *, void *), void (*delete_key)(void *), void (*delete_value)(void *));
	~BTree();
	void print_tree();
};

class NODE
{
private:
	BTree *tree;
public:
	void *key;
	void *value;
        NODE *l_child; //0=left, 1=right
	NODE *r_child;
	NODE *parent;
	NODE(BTree *tree, NODE *parent);
	NODE(void *key, void *value, BTree *tree, NODE *parent);
	~NODE();
};

NODE::NODE(BTree *tree, NODE *parent)
{
	this->key = this->value = this->l_child = this->r_child = NULL;
	this->tree = tree;
	this->parent = parent;
}

NODE::NODE(void *key, void *value, BTree *tree, NODE *parent)
{
	this->key = key;
	this->value = value;
	this->l_child = this->r_child = NULL;
	this->tree = tree;
	this->parent = parent;
}

NODE::~NODE()
{
	if (key)
		tree->delete_key(key);
	if (value)
	        tree->delete_value(value);
}

NODE *BTree::get_node(void *key)
{
	if (is_empty)
		return NULL;
	int tmp;
	NODE *cur = root;
	while (cur)
	{
		tmp = compare_keys(key, cur->key);
		if (tmp == -1)
			cur = cur->r_child;
		else if (tmp == 1)
			cur = cur->l_child;
		else
			return cur;
	}
	return cur;
}

int BTree::remove_node(void *key)
{
	NODE *node = get_node(key);
	//TODO: be able to remove root node
	if (node == root)
	{
		if (root->l_child)
		{
			root = node->l_child;
			root->parent = NULL;
			root->r_child = node->r_child;
			if (node->r_child)
				node->r_child->parent = root;
		}
		else if (root->r_child)
		{
			root = node->r_child;
			root->parent = NULL;
			root->l_child = node->l_child;
			if (node->l_child)
				node->l_child->parent = root;
		}
		else
		{
			this->is_empty = true;
			return 1;
		}
	}
	else if (node == node->parent->l_child)
	{
		if (node->r_child)
		{
			node->parent->l_child = node->r_child;
			node->r_child->parent = node->parent;
			NODE **i = &(node->r_child);
			while ((*i)->l_child)
				i = &((*i)->l_child);
			(*i)->l_child = node;
			node->parent = *i;
		}
		else
		{
			node->parent->l_child = node->l_child;
			if (node->l_child)
				node->l_child->parent = node->parent;
		}
	}
	else if (node == node->parent->r_child)
	{
		if (node->l_child)
		{
			node->parent->r_child = node->l_child;
			node->l_child->parent = node->parent;
			NODE **i = &(node->l_child);
			while ((*i)->r_child)
				i = &((*i)->r_child);
			(*i)->r_child = node;
			node->parent = *i;
		}
		else
		{
			node->parent->r_child = node->r_child;
			if (node->r_child)
				node->r_child->parent = node->parent;
		}
	}
	else
		return -1;
	delete node;
	return 1;
}

int BTree::insert_node(void *key, void *value)
{
	if (is_empty)
	{
		root->key = key;
		root->value = value;
		is_empty = false;
		return 1;
	}
	NODE **i = &root;
	NODE *parent = root;
	do
	{
		int dir = compare_keys(key, (*i)->key);
		switch(dir)
		{
		case -1:
		{
			i = &((*i)->r_child);
			if (*i)
				parent = *i;
			break;
		}
		case 1:
		{
			i = &((*i)->l_child);
			if (*i)
				parent = *i;
			break;
		}
		default:
			return 0;
		}
	} while ((*i));
	*i = new NODE(key, value, this, parent);
	return 1;
}

BTree::BTree(char (*compare_keys)(void *, void *), void (*delete_key)(void *), void (*delete_value)(void *))
{
	this->compare_keys = compare_keys;
	this->delete_key = delete_key;
	this->delete_value = delete_value;
	this->root = new NODE(this, NULL);
	is_empty = true;
}

BTree::BTree(vector<NODE> nodes, char (*compare_keys)(void *, void *), void (*delete_key)(void *), void (*delete_value)(void *))
{
	this->compare_keys = compare_keys;
	this->delete_key = delete_key;
	this->delete_value = delete_value;
	this->root = new NODE(this, NULL);
	is_empty = true;
	for (auto i : nodes)
	{
		insert_node(i.key, i.value);
	}
}

void BTree::delete_nodes(NODE *cur)
{
	if (cur->l_child)
		delete_nodes(cur->l_child);
	if (cur->r_child)
		delete_nodes(cur->r_child);
	delete cur;
}

BTree::~BTree()
{
	if (!is_empty)
		delete_nodes(root);
}

void BTree::print_nodes(NODE *cur)
{
	if (cur->l_child)
		print_nodes(cur->l_child);
	cout << *(int*)cur->key << endl;
	if (cur->r_child)
		print_nodes(cur->r_child);
}

void BTree::print_tree()
{
	if (!is_empty)
		print_nodes(root);
}

char comp(void *a, void *b)
{
	if (*(int*)a == *(int*)b)
		return 0;
	if (*(int*)a < *(int*)b)
		return 1;
	return -1;
}

void delk(void *ting)
{

}

void delv(void *ting)
{

}

int main(int argc, char **argv)
{
	BTree test(comp, delk, delv);
	int a = 5;
	int b = 7;
	int c = 2;
	int d = 68;
	int e = 420;
	int f = 2;
	int g = 4;
	int h = 893;
	int z = 34;
	test.insert_node(&a, &a);
	test.insert_node(&b, &b);
	test.insert_node(&c, &c);
	test.insert_node(&d, &d);
	test.insert_node(&e, &e);
	test.insert_node(&f, &f);
	test.insert_node(&g, &g);
	test.insert_node(&h, &h);
	test.insert_node(&z, &z);
	//test.remove_node(&a);
	//test.remove_node(&b);
	test.print_tree();
	return 0;
}
