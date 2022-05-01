#include <iostream>
#include <cmath>

using namespace std;

template <size_t N, typename T> 
class KDNode {
    T keys[N];
public:
    KDNode(T p[N], KDNode<N,T> *dad = 0);

    KDNode<N,T> *father;
    KDNode<N,T> *loson, *hison;

    T* getKeys(){return keys;}

    T& operator[](int);
    bool operator==(const KDNode &Node);
    friend float distance(KDNode A, KDNode B);
};

template <size_t N, typename T> 
KDNode<N,T>::KDNode(T p[N], KDNode<N,T> *dad) {
    for (int i = 0; i < N; i++) {
        keys[i] = p[i];
    }
    loson = 0;
    hison = 0;
    father = dad;
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
float distance(KDNode<N,T> A, KDNode<N,T> B) {
    T dis = 0;
    for (int i = 0; i < N; i++) {
        dis += (A[i] - B[i]) * (A[i] - B[i]);
    }
    return sqrt((double)dis);
}

// =======================================
// ============ CLASS KD-TREE ============
// =======================================

template <size_t N, typename T> 
class KDTree {
    KDNode<N,T> *root;

    int successor(KDNode<N,T>*ptr, T keys[N], int disc);
    KDNode<N,T>* minimum(KDNode<N,T>* ptr, int disc, int j);
    KDNode<N,T>* maximum(KDNode<N,T>* ptr, int disc, int j);
public:
    KDTree<N,T>() {root = 0;}
    ~KDTree<N,T>();

    void insertar(T keys[N]);
    bool buscar(T keys[N], KDNode<N,T>* &ptr, int &disc);
    void eliminar(T keys[N]);

    void NearestNeighbor(T keys[N], T result[N]);
    T** kNN(T keys[N], int k);
    T** rangeQuery(T keys[N], int k);
};

template <size_t N, typename T> 
KDTree<N,T>::~KDTree<N,T>() {
    while (root){
        eliminar(root->getKeys());
    }
}

// ======================================
// ======== FUNCIONES AUXILIARES ========
// ======================================

// Indica si las keys deben ir a la derecha o izq de ptr
// 1: hison
// 2: loson
// 0: ptr tiene las mismas llaves que keys
// disc indica la dimension de comparacion
template <size_t N, typename T>
int KDTree<N,T>::successor(KDNode<N,T>*ptr, T keys[N], int disc) {
    if (keys[disc] > (*ptr)[disc])
        return 1;
    if (keys[disc] < (*ptr)[disc])
        return 2;

    // de ser la misma llave, se comparan el resto una a una
    for (int i = 1 ; i < N; i++){
        if (keys[(disc + i)%N] > (*ptr)[(disc + i)%N])
            return 1;
        if (keys[(disc + i)%N] < (*ptr)[(disc + i)%N])
            return 2;
    }
    return 0;
}

// Funcion para encontrar el nodo con el minimo keys[disc]
// dentro de ptr en caso de borrar y remplazar x el hison
template <size_t N, typename T>
KDNode<N,T>* KDTree<N,T>::minimum(KDNode<N,T>* ptr, int disc, int j) {
    if (ptr->hison == 0 && ptr->loson == 0)
        return ptr;
    if (disc == j){
        if (ptr->loson)
            return minimum(ptr->loson, disc, (j+1)%N);
        return ptr;
    }
    if (ptr->hison == 0)
        return minimum(ptr->loson, disc, (j+1)%N);
    if (ptr->loson == 0)
        return minimum(ptr->hison, disc, (j+1)%N);
    // si tiene ambos hijos:
    // escoger el que tenga mejor resultado
    KDNode<N,T>* ls = minimum(ptr->loson, disc, (j+1)%N);
    KDNode<N,T>* hs = minimum(ptr->hison, disc, (j+1)%N);

    if ((*ls)[disc] < (*hs)[disc])
        return ls;
    return hs;
}

// Funcion para encontrar el nodo con el maximo keys[disc]
// dentro de ptr en caso de borrar y remplazar x el loson
template <size_t N, typename T>
KDNode<N,T>* KDTree<N,T>::maximum(KDNode<N,T>* ptr, int disc, int j) {
    if (ptr->hison == 0 && ptr->loson == 0)
        return ptr;
    if (disc == j){
        if (ptr->hison)
            return maximum(ptr->hison, disc, (j+1)%N);
        return ptr;
    }
    if (ptr->hison == 0)
        return maximum(ptr->loson, disc, (j+1)%N);
    if (ptr->loson == 0)
        return maximum(ptr->hison, disc, (j+1)%N);
    // si tiene ambos hijos:
    // escoger el que tenga mejor resultado
    KDNode<N,T>* ls = maximum(ptr->loson, disc, (j+1)%N);
    KDNode<N,T>* hs = maximum(ptr->hison, disc, (j+1)%N);

    if ((*ls)[disc] > (*hs)[disc])
        return ls;
    return hs;
}

// =======================================
// ======== FUNCIONES ESCENCIALES ========
// =======================================

template <size_t N, typename T>
void KDTree<N,T>::insertar(T keys[N])
{
    KDNode<N,T> *nodo = new KDNode<N,T>(keys,0);
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
                        nodo->father = q;
                        break;
                    }
                else
                    if (q->loson)
                        q = q->loson;
                    else {
                        q->loson = nodo;
                        nodo->father = q;
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
bool KDTree<N,T>::buscar(T keys[N], KDNode<N,T>* &ptr, int &disc)
{
    if (root == 0)
    {
        disc = -1;
        return false;
    }
    else
    {
        // puntero que bajara por el arbol
        ptr = root;
        disc = 0;
        while (true){
            int son = successor(ptr,keys,disc);
            if (son)
                if (son == 1)
                    if (ptr->hison)
                        ptr = ptr->hison;
                    else
                        return false;
                else
                    if (ptr->loson)
                        ptr = ptr->loson;
                    else 
                        return false;
            else
                return true;
            disc = (disc + 1) % N;
        }
    }
}

template <size_t N, typename T>
void KDTree<N,T>::eliminar(T keys[N]) {
    KDNode<N,T> *ptr;
    int disc;
    if (!buscar(keys, ptr, disc))
        return;

    // caso: hoja
    if (ptr->hison == 0 && ptr->loson == 0){
        if (root == ptr){
            root = 0;
            delete ptr;
            return;
        }
        if (ptr->father->hison == ptr)
            ptr->father->hison = 0;
        else
            ptr->father->loson = 0;
        delete ptr;
        return;
    }

    // caso: rama
    KDNode<N,T> *q;
    bool qson = true;   // hison: true, loson: false
    if (ptr->hison){
        q = minimum(ptr->hison, disc, (disc+1)%N);
        if (q->father->loson == q)
            qson = false;
    }
    else {
        q = maximum(ptr->loson, disc, (disc+1)%N);
        if (q->father->loson == q)
            qson = false;
    }

    if (qson)
        q->father->hison = 0;
    else
        q->father->loson = 0;
    
    q->hison = ptr->hison;
    q->loson = ptr->loson;
    q->father = ptr->father;
    if (q->loson)
        q->loson->father = q;
    if (q->hison)
        q->hison->father = q;
    if (root == ptr)
        root = q;
    
    delete ptr;
}

// =======================================
// ====== BUSQUEDA MULTIDIMENSIONAL ======
// =======================================

template <size_t N, typename T>
KDNode<N,T> *NN(KDNode<N,T>* ptr, KDNode<N,T>* target, int d, float &bd) {
    if (!ptr)
        return 0;

    float r = distance<N,T>(*ptr,*target);
    if (r < bd)
        bd = r;

    // bajar por la rama mas cercana al target
    KDNode<N,T> *q;
    bool rama = 0;  // 0:loson, 1:hison
    if ((*ptr)[d] < (*target)[d]){
        rama = 1;
        q = NN(ptr->hison,target,(d+1)%N,bd);
    } else {
        q = NN(ptr->loson,target,(d+1)%N,bd);
    }

    KDNode<N,T> *best = ptr;
    if (q && r > distance<N,T>(*q,*target))
        best = q;

    // analizar las ramas no visitadas
    float r_ = fabs((*ptr)[d] - (*target)[d]);
    r = distance<N,T>(*best,*target);
    if (r_ < r){
        if (rama == 0){
            q = NN(ptr->hison,target,(d+1)%N,bd);
        } else {
            q = NN(ptr->loson,target,(d+1)%N,bd);
        }
    }

    if (q && r > distance<N,T>(*q,*target))
        best = q;

    return best;
}

template <size_t N, typename T>
void KDTree<N,T>::NearestNeighbor(T keys[N], T result[N]) {
    KDNode<N,T> NodeKey(keys);
    KDNode<N,T> *target = &NodeKey;
    float best_distance = INFINITY;
    KDNode<N,T> *nn = NN(root,target,0,best_distance);

    for (int i = 0; i < N; i++)
        result[i] = (*nn)[i];
}

template <size_t N, typename T>
T** KDTree<N,T>::kNN(T keys[N], int k) {
    return 0;
}

template <size_t N, typename T>
T** KDTree<N,T>::rangeQuery(T keys[N], int k) {
    return 0;
}

// =======================================
// ================ MAIN =================
// =======================================

int main() {
    int M[][2] = {{5,4},{2,6},{13,3},{3,1},{10,2},{8,7}};
    KDTree<2,int> Tree;
    Tree.insertar(M[0]);
    Tree.insertar(M[1]);
    Tree.insertar(M[2]);
    Tree.insertar(M[3]);
    Tree.insertar(M[4]);
    Tree.insertar(M[5]);

    int query[2];
    int P[][2] = {{6,5},{9,4},{3,2},{9,9}};
    Tree.NearestNeighbor(P[0], query);
    cout << query[0] << ", " << query[1] << endl;

    Tree.NearestNeighbor(P[1], query);
    cout << query[0] << ", " << query[1] << endl;

    Tree.NearestNeighbor(P[2], query);
    cout << query[0] << ", " << query[1] << endl;

    Tree.NearestNeighbor(P[3], query);
    cout << query[0] << ", " << query[1] << endl;
    
    return 0;
}