#include <cstdio>
#define stature(p) (static_cast<int>((p)?(p)->height:-1))
#define BalFac(x) (stature((x)->left)- stature((x)->right))
#define AvlBalanced(x) ((BalFac(x)>-2)&&(BalFac(x)<2))
#define max(x,y) (x>y?x:y)
#define isRoot(x) (!((x)->parent))
#define isLChild(x) (!isRoot(x) && (x)->parent->left == (x))
#define isRChild(x) (!isRoot(x) &&(x)->parent->right == (x))
#define tallerChild(x) ( stature( (x)->left ) > stature( (x)->right ) ? (x)->left : (stature( (x)->left ) < stature( (x)->left ) ? (x)->right : (isLChild((x)) ? (x)->left : (x)->right)))
//#define FromParentTo( x ) 
using namespace std;

void swap(int &a, int &b){
    int t = a;
    a = b;
    b = t;
}

struct AVL{
    int data;
    int height;
    AVL *left, *right, *parent;
    AVL(int data, AVL* p){
        this->data = data;
        this->left = this->right = nullptr;
        this->parent = p;
        height = 0;
    }
    AVL(){
        this->data = 0;
        this->left = this->right = this->parent = nullptr;
        height = 0;
    }
    ~AVL(){
        if(left) delete left;
        if(right) delete right;
        left = right = nullptr;
        if(this->parent == nullptr) return;
        if(isLChild(this)) this->parent->left = nullptr;
        else this->parent->right = nullptr;
    }

    void updateHeightAbove();

    int updateHeight();

    AVL* succ();

    AVL* pred();

    AVL* removeAt(AVL* &x, AVL* &hot);

};


AVL* AVL::succ(){
    AVL* x = this;
    if(x->right){
        x = x->right;
        while(x->left) x = x->left;
    }else{
        while(x->parent&&isRChild(x)) x = x->parent;
        x = x->parent;
    }
    return x;
}

AVL* AVL::pred(){
    AVL* x = this;
    if(x->left){
        x = x->left;
        while(x->right) x = x->right;
    }else{
        while(x->parent&&isLChild(x)) x = x->parent;
        x = x->parent;
    }
    return x;
}

AVL* removeAt(AVL* &x, AVL* &hot){
    AVL * w = x;
    AVL * succ = nullptr;
    if(!x->left) succ = x = x->right;
    else if(!x->right) succ = x = x->left;
    else{
        w = w->succ();
        swap(x->data,w->data);
        AVL* u = w->parent;
        (u==x? u->right:u->left) = succ = w->right;
    }
    hot = w->parent;
    if(succ) succ->parent = hot;
    delete w;
    return succ;
}

void AVL::updateHeightAbove(){
    AVL* x = this;
    while(x){
        x->height = max(stature(x->left),stature(x->right))+1;
        x = x->parent;
    }
}

int AVL::updateHeight(){
    return (this->height = max(stature(this->left),stature(this->right))+1);
}

struct AVLTree{
    int size;
    AVL *root;
    AVL * hot;
    AVLTree(){
        hot = root = nullptr;
        size = 0;
    }
    AVL* & search(int d);
    AVL* insert(int d);
    AVL* connect34(AVL* a, AVL* b, AVL* c, AVL* T0, AVL* T1, AVL* T2, AVL* T3);
    AVL* rotateAt(AVL* v);
    bool remove(int d);
    int leftEqual(int d);
    AVL*& FromParentTo(AVL *x);
};

AVL*& AVLTree::FromParentTo(AVL* x){
    if(isRoot(x)) 
        return root;
    if(isLChild(x))
        return (x)->parent->left;
    return (x)->parent->right;
}

AVL* & AVLTree::search(int d){
    if(!root||d == root->data) {
        { hot = nullptr; return root; }
    }
    hot = root;
    while(true){
        AVL* & v = (d<hot->data)? hot->left:hot->right;
        if(!v||d==v->data) return v;
        hot = v;
    }
}

AVL* AVLTree::connect34(AVL* a, AVL* b, AVL* c, AVL* T0, AVL* T1, AVL* T2, AVL* T3){
    a->left = T0; if(T0) T0->parent = a;
    a->right = T1; if(T1) T1->parent = a; 
    c->left = T2; if(T2) T2->parent = c;
    c->right = T3; if(T3) T3->parent = c;
    b->left = a; a->parent = b;
    b->right = c; c->parent = b; 
    a->updateHeight();
    c->updateHeight();
    b->updateHeight();
    return b;
}

AVL* AVLTree::rotateAt(AVL* v){
    AVL* p = v->parent;
    AVL* g = p->parent;
    if(isLChild(p)){
        if(isLChild(v)){
            p->parent = g->parent;
            return connect34(v,p,g,v->left,v->right,p->right,g->right);
        }else{
            v->parent = g->parent;
            return connect34(p,v,g,p->left,v->left,v->right,g->right);
        }
    }else{
        if(isRChild(v)){
            p->parent = g->parent;
            return connect34(g,p,v,g->left,p->left,v->left,v->right);
        }else{
            v->parent = g->parent;
            return connect34(g,v,p,g->left,v->left,v->right,p->right);
        }
    }
}

AVL* AVLTree::insert(int d){
    AVL* & x = search(d);
    if(x) return x;
    AVL* xx = x = new AVL(d,hot);
    size++;
    for(AVL* g = hot;g;g = g->parent){
        if(!AvlBalanced(g)){
            FromParentTo(g) = rotateAt(tallerChild(tallerChild(g)));
            break;
        }else g->updateHeight();
    }
    return xx;
}

bool AVLTree::remove(int d){
    AVL* x = search(d);
    if(!x) return false;
    removeAt(x,hot);
    size--;
    for(AVL* g = hot;g;g = g->parent){
        if(!AvlBalanced(g))
           g = FromParentTo(g) = rotateAt(tallerChild(tallerChild(g)));
        g->updateHeight();
    }
    return true;
}

int AVLTree::leftEqual(int d){
    AVL* x = search(d);
    if(x){
        return x->data;
    }
    if(hot->data<d) return hot->data;
    else {
        x = hot->pred();
        if(!x){
            return -1;
        }
        return x->data;
    }
}

AVLTree tree;

void operations(){
    int n;
    scanf("%d",&n);
    for(int i = 0;i<n;i++){
        char op;
        int d;
        scanf("\n%c %d",&op,&d);
        if(op == 'A'){
            tree.insert(d);
        }else if(op == 'B'){
            tree.remove(d);
        }else if(op == 'C'){
            printf("%d\n",tree.leftEqual(d));
        }
    }
}

struct Splay{
    
};

int main(){
    operations();
    return 0;
}