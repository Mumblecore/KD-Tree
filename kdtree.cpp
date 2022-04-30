#include <iostream>
#include <cmath>

using namespace std;

template <size_t N, typename T> 
class KDNode {
    T keys[N];
public:
    KDNode(T p[N]);

    KDNode<N,T> *loson, *hison;

    T& operator[](int);
    bool operator==(const KDNode &Node);
    friend T distance(KDNode A, KDNode B);
};

template <size_t N, typename T> 
KDNode<N,T>::KDNode(T p[N]) {
    for (int i = 0; i < N; i++) {
        keys[i] = p[i];
    }
    loson = 0;
    hison = 0;
}

template <size_t N, typename T> 
T& KDNode<N,T>::operator[] (int index) {
    return keys[index];
}

template <size_t N, typename T> 
bool KDNode<N,T>::operator== (const KDNode &Node) {
    for (int i = 0; i < N; i++) {
        if (keys[i] != Node.keys[i])
            return false;
    }
    return true;
}

template <size_t N, typename T> 
T distance(KDNode<N,T> A, KDNode<N,T> B) {
    T dis = 0;
    for (int i = 0; i < N; i++) {
        dis += (A.keys[i] - B.keys[i]) * (A.keys[i] - B.keys[i]);
    }
    return sqrt((double)dis);
}

// =======================================
// ============ CLASS KD-TREE ============
// =======================================

template <size_t N, typename T> 
class KDTree {
    KDNode<N,T> *root;
public:
    KDTree<N,T>() {root = 0;}
    void insertar(T keys[N]);
    bool buscar(T keys[N], KDNode<N,T>* &ptr);
    void eliminar(T keys[N]);
};

// Indica si las keys deben ir a la derecha o izq de ptr
// 1: hison
// 2: loson
// 0: ptr tiene las mismas llaves que keys
// disc indica la dimension de comparacion
template <size_t N, typename T>
int successor(KDNode<N,T>*ptr, T keys[N], int disc) {
    if (keys[disc] > (*ptr)[disc])
        return 1;
    if (keys[disc] < (*ptr)[disc])
        return 2;

    // de ser la misma llave, se comparan el resto una a una
    for (int i = 1 ; i < N; i++){
        if (keys[(disc + i)%N] > (*p)[(disc + i)%N])
            return 1;
        if (keys[(disc + i)%N] < (*p)[(disc + i)%N])
            return 2;
    }
    return 0;
}

template <size_t N, typename T>
bool hasKeys(KDNode<N,T>*p, T keys[N]) {
    for (int i = 0 ; i < N; i++)
        if ((*p)[i] != keys[i])
            return false;
    return true;
}

template <size_t N, typename T>
void KDTree<N,T>::insertar(T keys[N])
{
    KDNode<N,T> *nodo = new KDNode<N,T>(keys);
    if (root == 0)
    {
        root = nodo;
    }
    else
    {
        // puntero que bajara por el arbol
        KDNode<N,T> *q = root;
        int disc = 0;
        while (true){
            int son = successor(q,keys,disc);
            if (son){
                if (son == 1)
                    if (q->hison)
                        q = q->hison;
                    else {
                        q->hison = nodo;
                        break;
                    }
                else
                    if (q->loson)
                        q = q->loson;
                    else {
                        q->loson = nodo;
                        break;
                    }
            }
            else
            // ese nodo ya se encuentra dentro del arbol  --> no insertamos
                return;
            disc = (disc + 1) % N;
        }
    }
}

template <size_t N, typename T>
bool KDTree<N,T>::buscar(T keys[N], KDNode<N,T>* &ptr)
{
    if (root == 0)
    {
        return false;
    }
    else
    {
        // puntero que bajara por el arbol
        KDNode<N,T> *q = root;
        int disc = 0;
        while (true){
            int son = successor(q,keys,disc);
            if (son){
                if (son == 1)
                    if (q->hison)
                        q = q->hison;
                    else
                        return false;
                else
                    if (q->loson)
                        q = q->loson;
                    else 
                        return false;
            }
            else
                return true;
            disc = (disc + 1) % N;
        }
    }
}

template <size_t N, typename T>
void KDTree<N,T>::eliminar(T keys[N]) {
    KDNode<N,T> *ptr;
    if (!buscar(keys, ptr))
        return;

    if (ptr->hison == 0 && ptr->loson == 0){
        // *ptr = 0;
        delete ptr;
    }

}

int main() {
    int M[3][2] = {{2,5},{5,1},{9,6}};
    KDTree<2,int> Tree;
    Tree.insertar(M[0]);
    Tree.insertar(M[1]);
    Tree.insertar(M[2]);
    return 0;
}