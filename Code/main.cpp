/*
    Raymond's Algorithm is a lock based algorithm for mutual exclusion on a distributed system. 
    This is a C++ implementation intended to demonstrate how it works and is intended for educational purposes only.
    Copyright (C) 2021 Pauras Ranade (PydraxAlpta)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <vector>

// this enum is used to implement the "temporary reversal of link direction" that the basic tree structure cannot incorporate
enum TokenPolarity
{
    LEFT,
    RIGHT,
    UP
};

struct Node
{
    int id;
    bool hasToken;
    int tokenPolarity;
    std::vector<int> requests;
    Node *parent, *left, *right;
    void sendRequest()
    {
        using std::cout;
        requests.push_back(this->id);
        cout << this->id << " added request to own queue\n";
        if (tokenPolarity == UP)
        {
            if (parent == nullptr)
            {
                return;
            }
            else
            {
                parent->receiveRequest(this->id);
                cout << "Request sent to Node " << parent->id << "\n";
            }
        }
        else if (tokenPolarity == LEFT)
        {
            if (left == nullptr)
            {
                return;
            }
            else
            {
                left->receiveRequest(this->id);
                cout << "Request sent to Node " << left->id << "\n";
            }
        }
        else
        {
            if (right == nullptr)
            {
                return;
            }
            else
            {
                right->receiveRequest(this->id);
                cout << "Request sent to Node " << right->id << "\n";
            }
        }
    }
    void receiveRequest(int senderid)
    {
        using std::cout;
        for (auto &&i : requests)
        {
            if (i == senderid)
                return;
        }
        requests.push_back(senderid);
        cout << "Added request of " << senderid << " to queue of " << this->id << "\n";
        if (!this->hasToken)
        {
            if (tokenPolarity == UP)
            {
                if (parent == nullptr)
                {
                    return;
                }
                else
                {
                    parent->receiveRequest(this->id);
                    cout << "Request sent to Node " << parent->id << "\n";
                }
            }
            else if (tokenPolarity == LEFT)
            {
                if (left == nullptr)
                {
                    return;
                }
                else
                {
                    left->receiveRequest(this->id);
                    cout << "Request sent to Node " << left->id << "\n";
                }
            }
            else
            {
                if (right == nullptr)
                {
                    return;
                }
                else
                {
                    right->receiveRequest(this->id);
                    cout << "Request sent to Node " << right->id << "\n";
                }
            }
        }
    }
};

class Tree
{
private:
    Node *root;
    int tokenID;
    void remove(Node *);
    Node *getNodeByID(int);

public:
    Tree();
    ~Tree();
    void request(int);
    void display();
    void release();
} T;

Tree::Tree() //creating 2-level, 5 node binary tree
{
    tokenID = 1;
    //level 0
    root = new Node;
    root->parent = nullptr;
    root->id = 1;
    root->hasToken = true;
    root->tokenPolarity = UP;
    //level 1
    Node *left = root->left = new Node;
    left->parent = root;
    left->id = 2;
    left->hasToken = false;
    left->tokenPolarity = UP;
    Node *right = root->right = new Node;
    right->parent = root;
    right->left = right->right = nullptr;
    right->id = 5;
    right->hasToken = false;
    right->tokenPolarity = UP;
    //level 2
    Node *lleft = left->left = new Node;
    lleft->parent = left;
    lleft->left = lleft->right = nullptr;
    lleft->id = 3;
    lleft->hasToken = false;
    lleft->tokenPolarity = UP;
    Node *lright = left->right = new Node;
    lright->parent = left;
    lright->left = lright->right = nullptr;
    lright->id = 4;
    lright->hasToken = false;
    lright->tokenPolarity = UP;
}
Tree::~Tree()
{
    remove(root);
}
void Tree::remove(Node *N)
{
    if (N == nullptr)
    {
        return;
    }
    Node *parent = N->parent;
    if (parent == nullptr)
    {
        root = nullptr;
    }
    else if (parent->left == N)
    {
        parent->left = nullptr;
    }
    else
    {
        parent->right = nullptr;
    }
    remove(N->left);
    remove(N->right);
    delete N;
}

int main(int argc, char const *argv[])
{
    int ch;
    while (true)
    {
        std::cout << "1. Create request for node n\n"
                  << "2. Release token from current holder\n"
                  << "3. Display data\n"
                  << "4. Exit\n"
                  << "Enter your choice: ";
        std::cin >> ch;
        if (ch == 1)
        {
            std::cout << "Enter node to make request with (1-5): ";
            std::cin >> ch;
            if (ch > 5 || ch < 1)
                std::cout << "Invalid node\n";
            else
                T.request(ch);
        }
        else if (ch == 2)
        {
            T.release();
        }
        else if (ch == 3)
        {
            T.display();
        }
        else if (ch == 4)
        {
            std::cout << "Exiting\n";
            return 0;
        }
    }
    return 0;
}

void Tree::request(int id)
{
    Node *N = getNodeByID(id);
    if (N->hasToken == true)
    {
        std::cerr << "Node already has token\n";
        return;
    }
    N->sendRequest();
}

void Tree::display()
{
    void displayHelper(Node *);
    displayHelper(root);
}

void displayHelper(Node *N)
{
    using std::cout;
    if (N == nullptr)
        return;
    if (N->hasToken)
        cout << N->id << "* :";
    else
        cout << N->id << "  :";
    cout << "Request queue: [";
    for (auto &&i : N->requests)
    {
        cout << i << " ";
    }
    cout << "]\n";
    if (N->left)
        displayHelper(N->left);
    if (N->right)
        displayHelper(N->right);
}
void Tree::release()
{
    using std::cout;
    Node *N = getNodeByID(tokenID);
    if (N->requests.empty())
    {
        cout << "No outstanding requests, token remains with current holder\n";
        return;
    }
    Node *R = getNodeByID(N->requests.front());
    if (R == N->parent)
    {
        N->tokenPolarity = UP;
    }
    else if (R == N->left)
    {
        N->tokenPolarity = LEFT;
    }
    else
    {
        N->tokenPolarity = RIGHT;
    }
    N->hasToken = false;
    cout << N->id << " has released token\n";
    R->hasToken = true;
    cout << R->id << " has obtained token\n";
    tokenID = R->id;
    N->requests.erase(N->requests.begin());
    cout << R->id << " removed from the queue of " << N->id << "\n";
    if (!N->requests.empty())
    {
        R->receiveRequest(N->id);
    }
    if (R->id == R->requests.front())
    {
        R->requests.erase(R->requests.begin());
        cout << R->id << " removed from the queue of " << R->id << "\n";
    }
    else
        release();
}

Node *Tree::getNodeByID(int id)
{
    Node *N = nullptr;
    switch (id)
    {
    case 1:
        N = root;
        break;
    case 2:
        N = root->left;
        break;
    case 3:
        N = root->left->left;
        break;
    case 4:
        N = root->left->right;
        break;
    case 5:
        N = root->right;
        break;
    default:
        std::cerr << "Incorrect id passed\n";
        return nullptr;
    }
    return N;
}
