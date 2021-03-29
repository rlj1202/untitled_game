#ifndef H_QUADTREE
#define H_QUADTREE

template <typename T>
class QuadTree {
public:
    QuadTree();
    QuadTree(const QuadTree &o);
    QuadTree(QuadTree &&o);
    ~QuadTree();

    T& At(int x, int y);
    void Set(int x, int y, T data);

private:
    T data;

    QuadTree childs[4];
};

#endif