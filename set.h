#include <set>
#include <cstddef>

template<typename T>
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

template<class T>
class Set;

template<class T>
class Set {
public:
    class iterator {
    private:
        const Set<T>* set_ = nullptr;
        Node<T>* t_ = nullptr;
    public:
        iterator() = default;

        iterator(Node<T>* t, const Set<T>* set): set_(set), t_(t) {}

        const T& operator*() const {
            return t_->value;
        }

        const T* operator->() const {
            return &t_->value;
        }

        iterator& operator++() {
            if (t_ == nullptr) {
                /// по-хорошему нужно бросать исключение
                return *this;
            }
            if (t_->right != nullptr) {
                t_ = t_->right;
                while (t_->left != nullptr) {
                    t_ = t_->left;
                }
                return *this;
            }
            while (true) {
                if (t_->parent == nullptr) {
                    t_ = nullptr;
                    return *this;
                }
                if (t_->parent->left == t_) {
                    t_ = t_->parent;
                    return *this;
                }
                t_ = t_->parent;
            }
        }

        iterator operator++(int) {
            iterator temp = *this;
            ++*this;
            return temp;
        }

        iterator& operator--() {
            if (t_ == nullptr) {
                t_ = set_->root_;
                if (t_ == nullptr) {
                    /// по-хорошему нужно бросать исключение
                    return *this;
                }
                while (t_->right != nullptr) {
                    t_ = t_->right;
                }
                return *this;
            }
            if (t_->left != nullptr) {
                t_ = t_->left;
                while (t_->right != nullptr) {
                    t_ = t_->right;
                }
                return *this;
            }
            while (true) {
                if (t_->parent == nullptr) {
                    /// по-хорошему нужно бросать исключение
                    return *this;
                }
                if (t_->parent->right == t_) {
                    t_ = t_->parent;
                    return *this;
                }
                t_ = t_->parent;
            }
        }

        iterator operator--(int)  {
            iterator temp = *this;
            --*this;
            return temp;
        }

        bool operator==(const iterator& it) const {
            return t_ == it.t_ && set_ == it.set_;
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
        Node<T>* t = root_;
        while (t->left != nullptr) {
            t = t->left;
        }
        return iterator(t, this);
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
    Node<T>* root_ = nullptr;
    size_t size_ = 0;

    void MakeRightSon(Node<T>* t, Node<T>* s) {
        if (t == nullptr) {
            return;
        }
        t->right = s;
        if (s != nullptr) {
            s->parent = t;
        }
    }

    void MakeLeftSon(Node<T>* t, Node<T>* s) {
        if (t == nullptr) {
            return;
        }
        t->left = s;
        if (s != nullptr) {
            s->parent = t;
        }
    }

    int Height(Node<T>* t) const {
        return (t == nullptr ? 0 : t->height);
    }

    int Diff(Node<T>* t) const {
        return (t == nullptr ? 0 : Height(t->right) - Height(t->left));
    }

    void UpdHeight(Node<T>* t) {
        if (t == nullptr) {
            return;
        }
        t->height = std::max(Height(t->left), Height(t->right)) + 1;
    }

    Node<T>* SmallLeftRotate(Node<T>* a) {
        Node<T>* b = a->right;

        MakeRightSon(a, b->left);

        b->left = a;
        b->parent = a->parent;
        a->parent = b;
        UpdHeight(a);
        UpdHeight(b);
        return b;
    }

    Node<T>* SmallRightRotate(Node<T>* a) {
        Node<T>* b = a->left;
        MakeLeftSon(a, b->right);

        b->right = a;
        b->parent = a->parent;
        a->parent = b;
        UpdHeight(a);
        UpdHeight(b);
        return b;
    }

    Node<T>* Balance(Node<T>* t) {
        UpdHeight(t);
        if (Diff(t) == 2) {
            if (Diff(t->right) < 0) {
                MakeRightSon(t, SmallRightRotate(t->right));
            }
            return SmallLeftRotate(t);
        } else if (Diff(t) == -2) {
            if (Diff(t->left) > 0) {
                MakeLeftSon(t, SmallLeftRotate(t->left));
            }
            return SmallRightRotate(t);
        }
        return t;
    }

    Node<T>* Insert(Node<T>* t, T val) {
        if (t == nullptr) {
            size_++;
            return new Node<T>(val);
        }
        if (val < t->value) {
            MakeLeftSon(t, Insert(t->left, val));
        } else if (t->value < val) {
            MakeRightSon(t, Insert(t->right, val));
        }
        return Balance(t);
    }

    Node<T>* FindMinSubtree(Node<T>* t) const {
        return (t->left == nullptr ? t : FindMinSubtree(t->left));
    }

    Node<T>* RemoveMinSubtree(Node<T>* t) {
        if (t->left == nullptr) {
            return t->right;
        }
        MakeLeftSon(t, RemoveMinSubtree(t->left));
        return Balance(t);
    }

    Node<T>* Erase(Node<T>* t, T val) {
        if (t == nullptr) {
            return nullptr;
        }
        if (val < t->value) {
            MakeLeftSon(t, Erase(t->left, val));
            return Balance(t);
        } else if (t->value < val) {
            MakeRightSon(t, Erase(t->right, val));
            return Balance(t);
        }
        size_--;
        Node<T>* l = t->left;
        Node<T>* r = t->right;
        delete t;
        if (l != nullptr) {
            l->parent = nullptr;
        }
        if (r != nullptr) {
            r->parent = nullptr;
        }
        if (r == nullptr) {
            return l;
        }
        Node<T>* min = FindMinSubtree(r);
        MakeRightSon(min, RemoveMinSubtree(r));
        MakeLeftSon(min, l);
        return Balance(min);
    }

    Node<T>* Find(Node<T>* t, T x) const {
        if (t == nullptr) {
            return nullptr;
        }
        if (x < t->value) {
            return Find(t->left, x);
        } else if (t->value < x) {
            return Find(t->right, x);
        } else {
            return t;
        }
    }

    Node<T>* LowerBound(Node<T>* t, T x, Node<T>* cur_best) const {
        if (t == nullptr) {
            return cur_best;
        }
        if (t->value < x) {
            return LowerBound(t->right, x, cur_best);
        }
        cur_best = t;
        return LowerBound(t->left, x, cur_best);
    }

    void DeleteTree(Node<T>* t) {
        if (t == nullptr) {
            return;
        }
        DeleteTree(t->left);
        DeleteTree(t->right);
        delete t;
    }

    Node<T>* Copy(Node<T>* t) {
        if (t == nullptr) {
            return nullptr;
        }
        Node<T>* l = Copy(t->left);
        Node<T>* r = Copy(t->right);

        Node<T>* new_t = new Node<T>;
        *new_t = *t;
        new_t->left = l;
        if (l != nullptr) {
            l->parent = new_t;
        }
        new_t->right = r;
        if (r != nullptr) {
            r->parent = new_t;
        }
        return new_t;
    }
};
