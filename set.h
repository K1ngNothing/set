#include <set>
#include <cstddef>

template<class T>
class Set {
private:
    struct Node {
        Node() {}
        T value;
        int height;
        Node* left;
        Node* right;
        Node* parent;
        explicit Node(const T& k) {
            value = k;
            height = 1;
            left = nullptr;
            right = nullptr;
            parent = nullptr;
        }
    };

public:
    class iterator {
    private:
        const Set<T>* set_ = nullptr;
        Node* vertex_ = nullptr;
    public:
        iterator() = default;

        iterator(Node* vertex, const Set<T>* set): set_(set), vertex_(vertex) {}

        const T& operator*() const {
            return vertex_->value;
        }

        const T* operator->() const {
            return &vertex_->value;
        }

        iterator& operator++() {
            if (vertex_ == nullptr) {
                /// по-хорошему нужно бросать исключение
                return *this;
            }
            if (vertex_->right != nullptr) {
                vertex_ = vertex_->right;
                while (vertex_->left != nullptr) {
                    vertex_ = vertex_->left;
                }
                return *this;
            }
            while (true) {
                if (vertex_->parent == nullptr) {
                    vertex_ = nullptr;
                    return *this;
                }
                if (vertex_->parent->left == vertex_) {
                    vertex_ = vertex_->parent;
                    return *this;
                }
                vertex_ = vertex_->parent;
            }
        }

        iterator operator++(int) {
            iterator temp = *this;
            ++*this;
            return temp;
        }

        iterator& operator--() {
            if (vertex_ == nullptr) {
                vertex_ = set_->root_;
                if (vertex_ == nullptr) {
                    /// по-хорошему нужно бросать исключение
                    return *this;
                }
                while (vertex_->right != nullptr) {
                    vertex_ = vertex_->right;
                }
                return *this;
            }
            if (vertex_->left != nullptr) {
                vertex_ = vertex_->left;
                while (vertex_->right != nullptr) {
                    vertex_ = vertex_->right;
                }
                return *this;
            }
            while (true) {
                if (vertex_->parent == nullptr) {
                    /// по-хорошему нужно бросать исключение
                    return *this;
                }
                if (vertex_->parent->right == vertex_) {
                    vertex_ = vertex_->parent;
                    return *this;
                }
                vertex_ = vertex_->parent;
            }
        }

        iterator operator--(int)  {
            iterator temp = *this;
            --*this;
            return temp;
        }

        bool operator==(const iterator& it) const {
            return vertex_ == it.vertex_ && set_ == it.set_;
        }

        bool operator!=(iterator it) const {
            return !(*this == it);
        }
    };

    Set() {
        root_ = nullptr;
        size_ = 0;
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        for (Iterator cur = first; cur != last; ++cur) {
            insert(*cur);
        }
    }

    Set(std::initializer_list<T> elems) {
        for (T elem : elems) {
            insert(elem);
        }
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void insert(const T& elem) {
        root_ = Insert(root_, elem);
        if (root_ != nullptr)
            root_->parent = nullptr;
    }

    void erase(const T& elem) {
        root_ = Erase(root_, elem);
        if (root_ != nullptr)
            root_->parent = nullptr;
    }

    iterator begin() const {
        if (root_ == nullptr) {
            return iterator(nullptr, this);
        }
        Node* vertex = root_;
        while (vertex->left != nullptr) {
            vertex = vertex->left;
        }
        return iterator(vertex, this);
    }

    iterator end() const {
        return iterator(nullptr, this);
    }

    iterator find(const T& elem) const {
        return iterator(Find(root_, elem), this);
    }

    iterator lower_bound(const T& elem) const {
        return iterator(LowerBound(root_, elem, nullptr), this);
    }


    ~Set() {
        DeleteTree(root_);
    }

    Set(const Set& other) {
        if (this != &other) {
            root_ = Copy(other.root_);
            size_ = other.size_;
        }
    }

    Set& operator=(const Set& other) {
        if (this != &other) {
            DeleteTree(root_);
            root_ = Copy(other.root_);
            size_ = other.size_;
        }
        return *this;
    }


private:

    Node* root_ = nullptr;
    size_t size_ = 0;

    void MakeRightSon(Node* vertex, Node* s) {
        if (vertex == nullptr) {
            return;
        }
        vertex->right = s;
        if (s != nullptr) {
            s->parent = vertex;
        }
    }

    void MakeLeftSon(Node* vertex, Node* s) {
        if (vertex == nullptr) {
            return;
        }
        vertex->left = s;
        if (s != nullptr) {
            s->parent = vertex;
        }
    }

    int Height(Node* vertex) const {
        return (vertex == nullptr ? 0 : vertex->height);
    }

    int Diff(Node* vertex) const {
        return (vertex == nullptr ? 0 : Height(vertex->right) - Height(vertex->left));
    }

    void UpdHeight(Node* vertex) {
        if (vertex == nullptr) {
            return;
        }
        vertex->height = std::max(Height(vertex->left), Height(vertex->right)) + 1;
    }

    Node* SmallLeftRotate(Node* a) {
        Node* b = a->right;

        MakeRightSon(a, b->left);

        b->left = a;
        b->parent = a->parent;
        a->parent = b;
        UpdHeight(a);
        UpdHeight(b);
        return b;
    }

    Node* SmallRightRotate(Node* a) {
        Node* b = a->left;
        MakeLeftSon(a, b->right);

        b->right = a;
        b->parent = a->parent;
        a->parent = b;
        UpdHeight(a);
        UpdHeight(b);
        return b;
    }

    Node* Balance(Node* vertex) {
        UpdHeight(vertex);
        if (Diff(vertex) == 2) {
            if (Diff(vertex->right) < 0) {
                MakeRightSon(vertex, SmallRightRotate(vertex->right));
            }
            return SmallLeftRotate(vertex);
        } else if (Diff(vertex) == -2) {
            if (Diff(vertex->left) > 0) {
                MakeLeftSon(vertex, SmallLeftRotate(vertex->left));
            }
            return SmallRightRotate(vertex);
        }
        return vertex;
    }

    Node* Insert(Node* vertex, T val) {
        if (vertex == nullptr) {
            size_++;
            return new Node(val);
        }
        if (val < vertex->value) {
            MakeLeftSon(vertex, Insert(vertex->left, val));
        } else if (vertex->value < val) {
            MakeRightSon(vertex, Insert(vertex->right, val));
        }
        return Balance(vertex);
    }

    Node* FindMinSubtree(Node* vertex) const {
        return (vertex->left == nullptr ? vertex : FindMinSubtree(vertex->left));
    }

    Node* RemoveMinSubtree(Node* vertex) {
        if (vertex->left == nullptr) {
            return vertex->right;
        }
        MakeLeftSon(vertex, RemoveMinSubtree(vertex->left));
        return Balance(vertex);
    }

    Node* Erase(Node* vertex, T val) {
        if (vertex == nullptr) {
            return nullptr;
        }
        if (val < vertex->value) {
            MakeLeftSon(vertex, Erase(vertex->left, val));
            return Balance(vertex);
        } else if (vertex->value < val) {
            MakeRightSon(vertex, Erase(vertex->right, val));
            return Balance(vertex);
        }
        size_--;
        Node* left_son = vertex->left;
        Node* right_son = vertex->right;
        delete vertex;
        if (left_son != nullptr) {
            left_son->parent = nullptr;
        }
        if (right_son != nullptr) {
            right_son->parent = nullptr;
        }
        if (right_son == nullptr) {
            return left_son;
        }
        Node* min = FindMinSubtree(right_son);
        MakeRightSon(min, RemoveMinSubtree(right_son));
        MakeLeftSon(min, left_son);
        return Balance(min);
    }

    Node* Find(Node* vertex, T x) const {
        if (vertex == nullptr) {
            return nullptr;
        }
        if (x < vertex->value) {
            return Find(vertex->left, x);
        } else if (vertex->value < x) {
            return Find(vertex->right, x);
        } else {
            return vertex;
        }
    }

    Node* LowerBound(Node* vertex, T x, Node* cur_best) const {
        if (vertex == nullptr) {
            return cur_best;
        }
        if (vertex->value < x) {
            return LowerBound(vertex->right, x, cur_best);
        }
        cur_best = vertex;
        return LowerBound(vertex->left, x, cur_best);
    }

    void DeleteTree(Node* vertex) {
        if (vertex == nullptr) {
            return;
        }
        DeleteTree(vertex->left);
        DeleteTree(vertex->right);
        delete vertex;
    }

    Node* Copy(Node* vertex) {
        if (vertex == nullptr) {
            return nullptr;
        }
        Node* left_son = Copy(vertex->left);
        Node* right_son = Copy(vertex->right);

        Node* new_vertex = new Node;
        *new_vertex = *vertex;
        new_vertex->left = left_son;
        if (left_son != nullptr) {
            left_son->parent = new_vertex;
        }
        new_vertex->right = right_son;
        if (right_son != nullptr) {
            right_son->parent = new_vertex;
        }
        return new_vertex;
    }
};
