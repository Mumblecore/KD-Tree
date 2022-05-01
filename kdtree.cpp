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
    friend T distance(KDNode A, KDNode B);
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

    int successor(KDNode<N,T>*ptr, T keys[N], int disc);
    KDNode<N,T>* minimum(KDNode<N,T>* ptr, int disc, int j);
    KDNode<N,T>* maximum(KDNode<N,T>* ptr, int disc, int j);
public:
    KDTree<N,T>() {root = 0;}
    ~KDTree<N,T>();

    void insertar(T keys[N]);
    bool buscar(T keys[N], KDNode<N,T>* &ptr, int &disc);
    void eliminar(T keys[N]);

    T*  NN(T keys[N]);
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
        KDNode<N,T> *q = root;
        disc = 0;
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
            else{
                ptr = q;
                return true;
            }
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
        if (ptr->father->hison == ptr)
            ptr->father->hison = 0;
        else
            ptr->father->loson = 0;
        if (root == ptr)
            root = 0;
        delete ptr;
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

    if (root == ptr)
        root = q;
    
    delete ptr;
}

// =======================================
// ====== BUSQUEDA MULTIDIMENSIONAL ======
// =======================================

    // KDNode<N,T> *result = NearestNeighbor(root, keys, 0);


// template <size_t N, typename T>
// KDNode<N,T> *NearestNeighbor(KDNode<N,T>* p, T k[N], int d, float bd, KDNode<N,T> *bc) {
//     if (ptr->hison && (*ptr)[disc] > keys[disc]){
//         ptr = ptr->hison;
//     }
// }

template <size_t N, typename T>
T*  KDTree<N,T>::NN(T keys[N]) {
    // Creamos un nodo con las keys solicitadas
    KDNode<N,T> target(keys);
    KDNode<N,T> *query = &target;

    // inicializamos nuestras variables
    KDNode<N,T> *ptr = root;
    int disc = 0;
    double best_dist = INFINITY;
    KDNode<N,T> *best_choise = 0;

    buscar(keys, ptr, disc);
    cout << ptr->keys[0] << ", " << ptr->keys[1] << endl;
    
    // bajamos
    // if (distance<N,T>(ptr,query) < best_dist){
    //     if (ptr->hison == 0 && ptr->loson == 0){
    //         // subimos
    //     }
    //     else if (ptr-> hison) {

    //     }
    // }

    // if (ptr->hison && (*ptr)[disc] > keys[disc]){
    //     ptr = ptr->hison;
    // }

    return ptr->getKeys();
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
    int M[][2] = {{30,40},{5,25},{70,70},{10,12},{50,30},{35,45}};
    KDTree<2,int> Tree;
    Tree.insertar(M[0]);
    Tree.insertar(M[1]);
    Tree.insertar(M[2]);
    Tree.insertar(M[3]);
    Tree.insertar(M[4]);
    Tree.insertar(M[5]);

    int q[] = {20,37};
    Tree.NN(q);
    
    return 0;
}