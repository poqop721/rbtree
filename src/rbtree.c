#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>


//연결 리스트로 구현한 스택
typedef struct LinkedList {
  node_t* node;
  struct LinkedList* link;
} Node;
Node* top = NULL;

//스택 에러 처리 함수
void error(char* str){
  fprintf(stderr, "%s\n", str);
  exit(1);
}

//스택이 비어있는지 확인
int is_empty(){ return top == NULL; }

//스택 초기화
void init_stack(){ top = NULL; }

//스택 사이즈
int size(){ 
  Node *p;
  int count = 0;
  for(p = top; p != NULL; p = p->link) count++;
  return count;
}

//push
void push(node_t* node){
  Node* p = (Node*)malloc(sizeof(Node));
  p->node = node;

  p->link = top;
  top = p;
}


//pop
node_t* pop(){
  Node* p;
  node_t* e;
  if(is_empty())
    error("스택 공백 에러");
  
  p = top;
  top = p->link;
  e = p->node;
  free(p);
  return e;
}

//top 노드 반환
node_t* peek(){
  if(is_empty())
    error("스택 공백 에러");
  return top->node;
} 

//스택 전체 free
void destroy_stack(){
  while (is_empty() == 0)
    pop();
}
//


rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); // malloc 대신 calloc을 쓰는 이유는 c언어에서는 0이 null 이기 때문. 이렇게 하면 처음에 nil로 초기화 시켜 줄 필요 x
  node_t *NIL = (node_t*)calloc(1,sizeof(node_t)); // NIL 을 표현 할 노드 생성.
  NIL->color = RBTREE_BLACK; //NIL 의 속성 지정
  p->nil = NIL; //트리의 초기값
  p->root = NIL; //트리의 초기값
  return p;
}

//후위순회하며 모든 노드 삭제
void delete_node(rbtree* t, node_t* n)
{
  if (n == t->nil){
    return;
  }
  delete_node(t, n->left);
  delete_node(t, n->right);
  free(n);
}

void delete_rbtree(rbtree *t) {
  // //스택으로 구현 => 메모리 누수 발생
  // init_stack();
  // node_t *node = t->root; 
  // while (1)
  // {
  //   while (node != NULL)
  //   {
  //     push(node);
  //     push(node);
  //     node = node->left;
  //   }
    
  //   if(!is_empty()){
  //     node_t * node = pop();
  //     if(!is_empty() && node == peek())
  //       node = node->right;
  //     else if(node){
  //       free(node);
  //       node = NULL;
  //     }
  //   }
  //   else
  //     break;
  // }
  // destroy_stack();
  // free(t);

  delete_node(t, t->root);
  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t, node_t *x){
  node_t *y = x->right;
  x->right = y->left;
  if(y->left != t->nil)
    y->left->parent = x;
  y->parent = x->parent;
  if(x->parent == t->nil)
    t->root = y;
  else if(x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *x){
  node_t *y = x->left;
  x->left = y->right;
  if(y->right != t->nil)
    y->right->parent = x;
  y->parent = x->parent;
  if(x->parent == t->nil)
    t->root = y;
  else if(x == x->parent->right)
    x->parent->right = y;
  else
    x->parent->left = y;
  y->right = x;
  x->parent = y;
}

void insert_fixup(rbtree *t, node_t *z){
  while(z->parent->color == RBTREE_RED){
    if(z->parent == z->parent->parent->left){
      node_t *y = z->parent->parent->right;
      if(y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else {
        if(z == z->parent->right){
          z = z->parent;
          left_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,z->parent->parent);
      }
    }
    else{
      node_t *y = z->parent->parent->left;
      if(y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else {
        if(z == z->parent->left){
          z = z->parent;
          right_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t,z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->key = key;
  node_t *y = t->nil;
  node_t *x = t->root;

  while(x != t->nil){
    y = x;
    if(z->key < x->key)
      x = x->left;
    else
      x = x->right;
  }
  z->parent = y;
  if(y == t->nil)
    t->root = z;
  else if(z->key < y->key)
    y->left = z;
  else
    y->right = z;
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  insert_fixup(t,z);
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // //스택으로 구현 => 실패
  // init_stack();
  // node_t *search = t->root;
  // push(search);
  // while (!is_empty() && search != t->nil)
  // {
  //   search = pop();
  //   if(search != t->nil){
  //     if(search->key > key)
  //       push(search->left);
  //     else if(search->key < key)
  //       push(search->right);
  //     else{
  //       destroy_stack();
  //       return search;
  //     }
  //   }
  // }
  // destroy_stack();
  // return NULL;


  node_t* search = t->root;
  while (search != t->nil)
  {
    if (search->key > key)
      search = search-> left;
    else if (search->key < key)
      search = search ->right;
    else
      return search;
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *search = t->root;
  while (search->left != t->nil)
  {
        search = search->left;
  }
  return search;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *search = t->root;
  while (search->right != t->nil)
  {
        search = search->right;
  }
  return search;
}

void transplant(rbtree *t, node_t *u, node_t *v){
  if(u->parent == t->nil)
    t->root = v;
  else if(u == u->parent->left)
    u->parent->left = v;
  else  
    u->parent->right = v;

  v->parent = u->parent;
}

node_t *minimum_for_erase(rbtree *t , node_t *x){
  while (x->left != t->nil)
  {
        x = x->left;
  }
  return x;
}

void delete_fixup(rbtree *t, node_t *x){
  node_t *w;
  while(x != t->root && x->color == RBTREE_BLACK){
    if(x == x->parent->left){
      w = x->parent->right;
      //type 1
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t,x->parent);
        w = x->parent->right;
      }
      //type 2
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        //type 3
        if(w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t,w);
          w = x->parent->right;
        }
        //type 4
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t,x->parent);
        x = t->root;
      }
    }
    else{ //x == x->parent->right
      w = x->parent->left;
      //type 1
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t,x->parent);
        w = x->parent->left;
      }
      //type 2
      if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        //type 3
        if(w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t,w);
          w = x->parent->left;
        }
        //type 4
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t,x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *x;
  node_t *y = p;
  color_t y_orig_color = y->color;
  
  //case 1
  if(p->left == t->nil){
    x = p->right;
    transplant(t,p,p->right);
  }
  //case 2
  else if(p->right == t->nil){
    x = p->left;
    transplant(t,p,p->left);
  }
  //case 3
  else{
    y = minimum_for_erase(t,p->right);
    y_orig_color = y->color;
    x = y->right;
    if(y->parent == p) // y가 z의 자식이면서 y는 두 자식 모두 nil 일 때
      x->parent = y; // 이 nil 노드의 부모가 y라고 알림. nil은 공용이지만 어차피 nil->parent의 쓰임새는 이거 말고는 없으니 ㄱㅊ
    else{
      transplant(t,y,y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    transplant(t,p,y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }

  if(y_orig_color == RBTREE_BLACK)
    delete_fixup(t,x);
  free(p); // delete 된 노드들은 트리 순회하며 free 할 때 같이 free 되지 않는다.
  return 0;
}


int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  //스택으로 dfs 돌면서 arr에 추가
  init_stack();
  node_t *node = t->root;
  int count = 0;
  push(t->nil);
  while (1)
  {
    while(node != t->nil){
      push(node);
      node = node->left;
    }
    node = pop();
    if(node == t->nil){
      break;
    }
    else{
      arr[count] = node->key;
      count++;
      node = node->right;
    }
  }
  destroy_stack();
  return 0;
}
