



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
typedef struct s_list {

        char *data;
        struct s_list *right;
        struct s_list *left;

}t_list;

struct s_list *node_creation(char  *data){
      
         t_list *first_node = malloc(sizeof(t_list));
            
           first_node->data = data;
            first_node->left = NULL;
            first_node->right  = NULL;
            return first_node;
}
t_list  *insert_right(t_list *root , char * data){

        root->right  = node_creation(data);
         return root->right;
}

t_list  *insert_left(t_list *root , char *data){
     root->left = node_creation(data) ;
       return root->left;
}

void print_tree(t_list *root){  
          if(root == NULL) return;

         print_tree(root->left);
  printf("The value is %s\n",root->data);
         print_tree(root->right);

}

int main(){

        t_list *root = node_creation("|");
     insert_right(root,">"); // right 42 
     insert_left(root,"ls");
        insert_right(root->right,"output.txt");
   

      print_tree(root);

     
   

}
