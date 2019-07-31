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
			this->is_empty = true;
		return 1;
	}
	if (node == node->parent->l_child)
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
	if (get_node(key))
		return 0;
	NODE **i = &root;
	while ((*i)->l_child || (*i)->r_child)
	{
		int dir = compare_keys(key, (*i)->key);
		switch(dir)
		{
		case -1:
		{
			if ((*i)->r_child)
			{
				if (compare_keys((*i)->r_child->key, key) == 1)
				{
					i = &((*i)->r_child);
					break;
				}
			}
			NODE *add = new NODE(key, value, this, *i);
			add->r_child = (*i)->r_child;
			if (add->r_child)
				add->r_child->parent = add;
			(*i)->r_child = add;
			return 1;
		}
		case 1:
		{
			int l_tmp, r_tmp;
			if ((*i)->l_child)
			{
				l_tmp = compare_keys((*i)->l_child->key, key);
				if (l_tmp == -1)
				{
					i = &((*i)->l_child);
					break;
				}
			}
			NODE *add = new NODE(key, value, this, *i);
			add->l_child = (*i)->l_child;
			if (add->l_child)
				add->l_child->parent = add;
			(*i)->l_child = add;
			return 1;
			
		}
		default:
			return -1;
		}
	}
	int side = (compare_keys(key, (*i)->key) == 1) ? 0 : 1;
	if (side == 0)
		(*i)->l_child = new NODE(key, value, this, *i);
	else
		(*i)->r_child = new NODE(key, value, this, *i);
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
	is_empty = false;
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
	test.insert_node(&a, &a);
	test.insert_node(&b, &b);
	test.insert_node(&c, &c);
	test.remove_node(&a);
	test.print_tree();
	return 0;
}
